/*
** Copyright 2014-2016 Robert Fratto. See the LICENSE.txt file at the top-level
** directory of this distribution.
**
** Licensed under the MIT license <http://opensource.org/licenses/MIT>. This file
** may not be copied, modified, or distributed except according to those terms.
*/

#include <libast/MemberAccess.h>
#include <libast/MemberVarDecl.h>
#include <libast/ClassDecl.h>
#include <libast/ClassMethod.h>
#include <libast/Valued.h>
#include <libast/ASTNode.h>
#include <libast/Module.h>
#include <libast/IDReference.h>
#include <libast/Parameter.h>
#include <libast/Protectable.h>

#include <libast/types/Type.h>

#include <libast/exceptions/code_error.h>
#include <libast/exceptions/access_denied_error.h>

#include <util/assertions.h>
#include <util/llvmassertions.h>

#include <llvm/IR/IRBuilder.h>

ASTNode* MemberAccess::copy() const
{
	return new MemberAccess(*this);
}

std::vector<ObjectBase**> MemberAccess::getMemberNodes()
{
	auto list = defMemberNodes();
	list.insert(list.end(), {
		(ObjectBase**)&m_class,
		(ObjectBase**)&m_member,
		(ObjectBase**)&m_valued
	});
	return list;
}

std::vector<std::vector<ObjectBase *>*> MemberAccess::getMemberLists()
{
	return defMemberLists();
}

const ClassDecl* MemberAccess::getClass() const
{
	return m_class;
}

const Valued* MemberAccess::getValued() const
{
	return m_valued;
}

const MemberVarDecl* MemberAccess::getMember() const
{
	return m_member;
}

const OString& MemberAccess::getName() const
{
	return m_name;
}

bool MemberAccess::hasPointer() const
{
	return true;
}

llvm::Value* MemberAccess::getPointer() const
{
	return m_value;
}

llvm::Value* MemberAccess::getValue() const
{
	return IRBuilder()->CreateLoad(m_value);
}

bool MemberAccess::isAccessible() const
{
	return m_member->isAccessible();
}

Expression* MemberAccess::access(OString name, const ASTNode *hint) const
{
	return m_member->access(name, this);
}

void MemberAccess::initialize()
{
	m_member = m_class->getMember(m_name);
	
	if (m_member == nullptr)
	{
		auto name = m_name;
		auto classDecl= m_class;
		throw code_error((ASTNode *)m_class,
						 [&name, classDecl] () -> std::string
						 {
							 std::stringstream ss;
							 ss << "Object of class " << classDecl->getName().str()
							 << " has no member " << name.str();
							 return ss.str();
						 });
	}
}

void MemberAccess::findDependencies()
{
	ASTNode::findDependencies();
	
	// If we have no class, we should look for all the content now.
	if (m_class == nullptr)
	{
		m_class = findParent<ClassDecl *>();
		assertExists(m_class, "Not in a class!");
		m_member = m_class->getMember(m_name);
		
    	if (m_member == nullptr)
    	{
			auto name = m_name;
			auto classDecl = m_class;
			throw code_error((ASTNode *)classDecl,
							 [&name, classDecl] () -> std::string
							 {
								 std::stringstream ss;
								 ss << "Object of class "
								    << classDecl->getName().str()
								    << " has no member " << name.str();
								 return ss.str();
							 });
    	}
	}

	addDependency((ASTNode *)m_class);
	addDependency(m_member);
	
	if (m_valued != nullptr)
	{
    	addDependency(m_valued->as<ASTNode *>());
	}
	else
	{
		auto method = findParent<ClassMethod *>();
		auto pclass = findParent<ClassDecl *>();
		if (method == nullptr || pclass == nullptr)
		{
			throw code_error(this, [] () -> std::string
				{
					return "Cannot use 'this' outside of a function.";
				});
		}
		
		// We need to depend on the this parameter of the method.
		addDependency(method->getThisParam());
	}
}

void MemberAccess::resolve()
{
	if (getMember()->is<Protectable *>() &&
		getMember()->as<Protectable *>()->usableFrom(this) == false)
	{
		throw access_denied_error(getMember(), this, "member");
	}
	
	if (m_valued == nullptr)
	{
		auto this_param = new IDReference("this");
		addChild(this_param);
		
		getModule()->process(this_param);
		
		m_valued = this_param;
		
		if (m_valued->as<Typed *>()->getType()->isVarTy())
		{
			throw fatal_error("this param never resolved type");
		}
	}
	
	setType(m_member->getType());
}

#include <llvm/IR/Module.h>
void MemberAccess::build()
{
	auto offset = getMember()->getOffset();
	
	for (auto& child : getChildren())
	{
		getModule()->build(child);
	}
	
	std::vector<llvm::Value *> offsets;
	offsets.push_back(llvm::ConstantInt::get(getModule()->getLLVMContext(),
											 llvm::APInt(32, 0)));
	offsets.push_back(llvm::ConstantInt::get(getModule()->getLLVMContext(),
											 llvm::APInt(32, offset)));
	
	// Continously load val until it is a single pointer.
	auto val = m_valued->getPointer();
	while (val->getType()->isPointerTy() &&
		   val->getType()->getPointerElementType()->isPointerTy())
	{
		val = IRBuilder()->CreateLoad(val);
	}
	

	if (val->getType()->getPointerElementType()->isIntegerTy())
	{
		throw fatal_error("Not a pointer to a class!");
	}
	
	// We need to verify that we're operating on the same type of class
	// as the class our memberbelongs to. If not, val should be casted.
	auto root_class_ty = m_valued->as<Typed *>()->getType()->getRootTy();
	if (root_class_ty->matches(getClass()->getType()) == false)
	{
		auto current_ty = m_valued->as<Typed *>()->getType()->getRootTy();
		current_ty = current_ty->getPointerTo();
		
		auto target_ty = getClass()->getType()->getPointerTo();
		
		val = IRBuilder()->CreateBitCast(val, target_ty->getLLVMType());
	}
	
	auto conv_ty = (llvm::StructType *)val->getType();
	conv_ty = (llvm::StructType *)conv_ty->getElementType(0);
	
	assertEqual(conv_ty->getElementType(offset),
				getLLVMType(), "Internal struct type does not match expected");
	
	m_value = IRBuilder()->CreateInBoundsGEP(val, offsets);
}

MemberAccess::MemberAccess(const ClassDecl* classDecl, Valued* valued,
						   const OString& name)
: m_name(name)
{
	if (name == "")
	{
		throw fatal_error("MemberAccess ctor called with empty name");
	}
	
	assertExists(classDecl, "MemberAccess ctor called with no ClassDecl");
	assertExists(valued, "MemberAccess ctor called with no Valued");
	
	m_class = classDecl;
	m_valued = valued;
}

MemberAccess::MemberAccess(const MemberAccess& other)
{
	m_class = other.m_class;
	m_valued = other.m_valued;
	
	m_name = other.m_name;
	m_super = other.m_super;
	other.defineCopy(this);
}

