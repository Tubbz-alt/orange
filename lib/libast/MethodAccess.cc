/*
** Copyright 2014-2016 Robert Fratto. See the LICENSE.txt file at the top-level
** directory of this distribution.
**
** Licensed under the MIT license <http://opensource.org/licenses/MIT>. This file
** may not be copied, modified, or distributed except according to those terms.
*/

#include <libast/MethodAccess.h>
#include <libast/ClassDecl.h>
#include <libast/ExpressionCall.h>
#include <libast/AccessExpr.h>
#include <libast/Module.h>
#include <libast/ReferenceExpr.h>
#include <libast/ClassMethod.h>

#include <libast/types/Type.h>
#include <libast/types/FunctionType.h>

#include <libast/exceptions/code_error.h>

#include <util/assertions.h>

void MethodAccess::findDependencies()
{
	auto&& all_named = m_class->getAllNamed(m_name, this);
	for (auto&& named : all_named)
	{
		addDependency(named->as<ASTNode *>());
	}
}

ASTNode* MethodAccess::copy() const
{
	return new MethodAccess(*this);
}

std::vector<ObjectBase**> MethodAccess::getMemberNodes()
{
	return defMemberNodes();
}

std::vector<std::vector<ObjectBase *>*> MethodAccess::getMemberLists()
{
	return defMemberLists();
}

bool MethodAccess::isStaticMethod()
{
    auto call = findParent<ExpressionCall *>();
	
	SearchSettings settings;
	settings.createGeneric = false;
	settings.forceTypeMatch = false;
	settings.includeLimit = false;
	settings.searchWholeTree = false;
	settings.filter = [] (Named* named) -> bool
	{
		return named->is<ClassMethod *>();
	};
	
	auto named = m_class->getNamed(m_name, call->expectedFunctionTy(), nullptr,
								   settings);
	
	if (named == nullptr)
	{
		return false;
	}
	
	return named->as<ClassMethod *>()->getStatic();
}

void MethodAccess::resolve()
{
	// The first thing to do is verify that we're being used in the
	// context of a function call.
	auto call = findParent<ExpressionCall *>();
	if (call == nullptr)
	{
		throw code_error(this, [] () { return "Accessing a method must be used "
			"in the context of a method call."; });
	}
	
	// Now, we want to get the "this" parameter from the nearest AccessExpr.
	auto access = findParent<AccessExpr *>();
	if (access == nullptr)
	{
		throw code_error(this, [] () { return "Method call not being used in "
			"the context of an object access."; });
	}
	
	// We depend on all the arguments of the function, so
	// let's just ask module to resolve them for us.
	for (auto arg : call->getArgs())
	{
		getModule()->process(arg);
	}
	
	bool is_static = isStaticMethod();
	Expression* this_arg = nullptr;
	
	if (is_static == false)
	{
		this_arg = new NodeReference(access->getLHS());
    	getModule()->process(this_arg);
		
		if (this_arg->getType()->isPointerTy() == false)
		{
			this_arg = new ReferenceExpr(this_arg);
			getModule()->process(this_arg);
		}
	}


	
	if (this_arg != nullptr)
	{
    	call->addArgument(this_arg, 0);
	}
	
	SearchSettings settings;
	settings.createGeneric = true;
	settings.forceTypeMatch = false;
	settings.includeLimit = false;
	settings.searchWholeTree = false;
	
	auto named = m_class->getNamed(m_name, call->expectedFunctionTy(), this, settings);
	setNode(named->as<ASTNode *>());
	
	NodeReference::resolve();
}

MethodAccess::MethodAccess(const ClassDecl* theClass, OString name)
{
	assertExists(theClass, "MethodAccess ctor: m_class is nullptr");
	
	if (name == "")
	{
		throw fatal_error("Creating a method access with an empty name");
	}
	
	m_class = theClass;
	m_name = name;
}

MethodAccess::MethodAccess(const MethodAccess& other)
{
	m_class = other.m_class;
	m_name = other.m_name;
	other.defineCopy(this);
}