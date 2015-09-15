/*
** Copyright 2014-2015 Robert Fratto. See the LICENSE.txt file at the top-level
** directory of this distribution.
**
** Licensed under the MIT license <http://opensource.org/licenses/MIT>. This file
** may not be copied, modified, or distributed except according to those terms.
*/
#include <orange/BinOpExpr.h>
#include <orange/VarExpr.h>
#include <orange/generator.h>
#include <orange/AnyID.h>

bool BinOpExpr::LHSShouldNotBeNull() {
	// For the moment, LHS should never be null.
	return true;
}

bool BinOpExpr::RHSShouldNotBeNull() {
	// For the moment, RHS should never be null.
	return true;
}

bool BinOpExpr::Validate(Value* LHS, Value* RHS) {
	// First, validate that LHS and RHS exist as needed.
	if (LHSShouldNotBeNull() && LHS == nullptr) {
		throw std::runtime_error("The LHS of this expression did not generate a value!");
	}

	if (RHSShouldNotBeNull() && RHS == nullptr) {
		throw std::runtime_error("The RHS of this expression did not generate a value!");
	}

	// If we're assigning, this would mean that LHS *has* to return a pointer for us to load in.
	// If it didn't, then something is wrong.
	if (IsAssignOp(m_op) && m_LHS->returnsPtr() == false) {
		throw std::runtime_error("LHS of this expression is not assignable.");
	}

	// The types of LHS and RHS must be compatible.
	if (LHS && RHS && CastingEngine::AreNodesCompatible(m_LHS, m_RHS) == false) {
		throw std::runtime_error("Cannot do operation.");
	}

	return true;
}

bool BinOpExpr::isVarExpr(Expression* expr) {
	if (expr == nullptr) return false;

	if (expr->getClass() == "VarExpr") return true;

	if (expr->getClass() == "AnyID") {
		AnyID* anything = (AnyID *)expr;
		return anything->expression()->getClass() == "VarExpr";
	}

	return false;
}

VarExpr* BinOpExpr::getVarExpr(Expression* expr) {
	if (expr == nullptr) return nullptr;

	if (expr->getClass() == "VarExpr") return (VarExpr *)expr;

	if (expr->getClass() == "AnyID") {
		AnyID* anything = (AnyID *)expr;

		if (anything->expression()->getClass() == "VarExpr") {
			return (VarExpr *)anything->expression();
		}
	}

	return nullptr;
}


bool BinOpExpr::IsAssignOp(std::string op) {
	return op == "=" || op == "+=" || op == "-=" || op == "/=" ||
		op == "*=";
}

bool BinOpExpr::IsCompareOp(std::string op) {
	return op == "<" || op == "<=" || op == ">" || op == ">=" ||
		op == "==" || op == "!=";
}

bool BinOpExpr::IsCustomOp(std::string op) {
	return op == "&&" || op == "and" || op == "||" || op == "or";
}


Instruction::BinaryOps BinOpExpr::GetBinOpFunction(Value* value1, bool signed1, StrElement op, Value* value2, bool signed2) {
	if ((value1->getType()->isFloatingPointTy() && value2->getType()->isFloatingPointTy() == false) ||
		(value2->getType()->isFloatingPointTy() && value1->getType()->isFloatingPointTy() == false)) {
		throw CompilerMessage(op, "Can't do operation with a float and non-float value.");
	}

	bool isFPOp = value1->getType()->isFloatingPointTy() && value2->getType()->isFloatingPointTy();

	if (op == "+") {
		return isFPOp ? Instruction::FAdd : Instruction::Add;
	} else if (op == "-") {
		return isFPOp ? Instruction::FSub : Instruction::Sub;
	} else if (op == "*") {
		return isFPOp ? Instruction::FMul : Instruction::Mul;
	} else if (op == "/") {
		if (isFPOp) {
			return Instruction::FDiv;
		} else {
			return (signed1 || signed2) ? Instruction::SDiv : Instruction::UDiv;
		}
	}	else if (op == "%" || op == "mod") {
		if (isFPOp) {
			return Instruction::FRem;
		} else {
			return (signed1 || signed2) ? Instruction::SRem : Instruction::URem;
		}
	} else if (op == "|") {
		if (isFPOp) {
			throw CompilerMessage(op, "Cannot do |; neither LHS or RHS can be a float!");
		} else {
			return Instruction::Or;
		}
	} else if (op == "&") {
		if (isFPOp) {
			throw CompilerMessage(op, "Cannot do |; neither LHS or RHS can be a float!");
		} else {
			return Instruction::And;
		}
	} else if (op == "^") {
		if (isFPOp) {
			throw CompilerMessage(op, "Cannot do |; neither LHS or RHS can be a float!");
		} else {
			return Instruction::Xor;
		}
	}

	throw CompilerMessage(op, "Unhandled operation " + op);
}

CmpInst::Predicate BinOpExpr::GetBinOpPredComp(Value* value1, bool signed1, StrElement op, Value* value2, bool signed2) {
	if ((value1->getType()->isFloatingPointTy() && value2->getType()->isFloatingPointTy() == false) ||
		(value2->getType()->isFloatingPointTy() && value1->getType()->isFloatingPointTy() == false)) {
		throw CompilerMessage(op, "Can't do operation with a float and non-float value.");
	}

	bool isFPOp = value1->getType()->isFloatingPointTy() && value2->getType()->isFloatingPointTy();

	if (op == ">") {
		if (isFPOp) {
			return CmpInst::FCMP_OGT;
		} else {
			return (signed1 || signed2) ? CmpInst::ICMP_SGT : CmpInst::ICMP_UGT;
		}
	} else if (op == ">=") {
		if (isFPOp) {
			return CmpInst::FCMP_OGE;
		} else {
			return (signed1 || signed2) ? CmpInst::ICMP_SGE : CmpInst::ICMP_UGE;
		}
	} else if (op == "<") {
		if (isFPOp) {
			return CmpInst::FCMP_OLT;
		} else {
			return (signed1 || signed2) ? CmpInst::ICMP_SLT : CmpInst::ICMP_ULT;
		}
	} else if (op == "<=") {
		if (isFPOp) {
			return CmpInst::FCMP_OLE;
		} else {
			return (signed1 || signed2) ? CmpInst::ICMP_SLE : CmpInst::ICMP_ULE;
		}
	} else if (op == "==") {
		if (isFPOp) {
			return CmpInst::FCMP_OEQ;
		} else {
			return CmpInst::ICMP_EQ;
		}
	} else if (op == "!=") {
		if (isFPOp) {
			return CmpInst::FCMP_ONE;
		} else {
			return CmpInst::ICMP_NE;
		}
	}

	throw CompilerMessage(op, "Unhandled compare operation " + op);
}


Value* BinOpExpr::Codegen() {
  if (m_LHS == nullptr || m_RHS == nullptr) {
      std::runtime_error("LHS or RHS are missing!");
  }

	// Generate the LHS side of the expression, expecting it to return a value if it needs to.
	Value* LHS = m_LHS->Codegen();
	Value* RHS = m_RHS->Codegen();

	if (m_op == "=" && LHS == nullptr) {
		throw CompilerMessage(*m_LHS, m_LHS->string() + " never created in this scope.");
	}

	// Validate should never return false, since it throws an exception any time it encounters
	// something that is invalid, but let's make sure anyway.
	if (Validate(LHS, RHS) == false) {
		throw std::runtime_error("The expression is not valid!");
	}

	if (m_RHS->returnsPtr()) RHS = GE::builder()->CreateLoad(RHS);

	// Load the LHS if it's a pointer and isn't used as an assign. Load the RHS if it's a pointer.
	if (IsAssignOp(m_op) == false && m_LHS->returnsPtr()) LHS = GE::builder()->CreateLoad(LHS);

	// If we're assigning, we want to cast RHS to LHS (forced).
	// Otherwise, cast them to fit.
	if (IsAssignOp(m_op)) {
		m_RHS->cast(&RHS, m_LHS->getType(), true);
	} else {
		if (IsCompareOp(m_op) == false && LHS->getType()->isPointerTy() && RHS->getType()->isIntegerTy()) {
			// If we're doing arithmetic with pointers/integers, make LHS an int
			LHS = GE::builder()->CreatePtrToInt(LHS, LHS->getType()->getPointerElementType());
			CastingEngine::CastToFit(&LHS, m_LHS->getType()->getPointerElementType(), &RHS, m_RHS->getType());
		} else {
			CastingEngine::CastToFit(&LHS, m_LHS->getType(), &RHS, m_RHS->getType());
		}

	}

	if (m_op == "=") {
		GE::builder()->CreateStore(RHS, LHS);
		m_value = GE::builder()->CreateLoad(LHS);
		return m_value;
	} else if (m_op == "+=") {
		Value* loadedLHS = GE::builder()->CreateLoad(LHS);
		m_value = GE::builder()->CreateBinOp(GetBinOpFunction(loadedLHS, m_LHS->isSigned(), "+", RHS, m_RHS->isSigned()), loadedLHS, RHS);
		GE::builder()->CreateStore(m_value, LHS);
		return m_value;
	} else if (m_op == "-=") {
		Value* loadedLHS = GE::builder()->CreateLoad(LHS);
		m_value = GE::builder()->CreateBinOp(GetBinOpFunction(loadedLHS, m_LHS->isSigned(), "-", RHS, m_RHS->isSigned()), loadedLHS, RHS);
		GE::builder()->CreateStore(m_value, LHS);
		return m_value;
	} else if (m_op == "*=") {
		Value* loadedLHS = GE::builder()->CreateLoad(LHS);
		m_value = GE::builder()->CreateBinOp(GetBinOpFunction(loadedLHS, m_LHS->isSigned(), "*", RHS, m_RHS->isSigned()), loadedLHS, RHS);
		GE::builder()->CreateStore(m_value, LHS);
		return m_value;
	} else if (m_op == "/=") {
		Value* loadedLHS = GE::builder()->CreateLoad(LHS);
		m_value = GE::builder()->CreateBinOp(GetBinOpFunction(loadedLHS, m_LHS->isSigned(), "/", RHS, m_RHS->isSigned()), loadedLHS, RHS);
		GE::builder()->CreateStore(m_value, LHS);
		return m_value;
	}


	if (IsCustomOp(m_op)) {
		// For && and ||, we have two blocks: check and continue.
		// Check is always used to modify the resulting value.
		// The value starts initially with being LHS.
		// If check is called, the value changes to the value of RHS.
		// Both LHS and RHS must be casted to booleans.
		// The nature of when we go to check is determined by && and ||.
		if (m_op == "&&" || m_op == "and" || m_op == "||" || m_op == "or") {
			bool castedL = m_LHS->cast(&LHS, IntTy::getUnsigned(1), true);
			bool castedR = m_RHS->cast(&RHS, IntTy::getUnsigned(1), true);

			if (castedL == false || castedR == false) {
 				throw CompilerMessage(*this, "both LHS and RHS must be castable to a boolean!");
			}

			Value *booleanVal = GE::builder()->CreateAlloca(IntTy::getUnsigned(1)->getLLVMType());
			GE::builder()->CreateStore(LHS, booleanVal);

			// Get the function we're in to create the blocks
			auto curFunction   = (FunctionStmt*)GE::runner()->symtab()->findStructure("FunctionStmt");
			auto llvmFunction  = (Function *)curFunction->getValue();

			// Create our blocks
			auto checkBlock    = BasicBlock::Create(GE::runner()->context(), "check", llvmFunction, curFunction->getBlockEnd());
			auto continueBlock = BasicBlock::Create(GE::runner()->context(), "continue", llvmFunction, curFunction->getBlockEnd());

			if (m_op == "&&" || m_op == "and") {
				// If LHS is true, we go to check. If it's false, we go to continue.
				// The reason for this is short circuiting; L && R will only ever evaluate
				// R if L is true, since false && x will never evaluate to true, independent of x.
				GE::builder()->CreateCondBr(LHS, checkBlock, continueBlock);
				GE::builder()->SetInsertPoint(checkBlock);
				GE::builder()->CreateStore(RHS, booleanVal);
			} else if (m_op	== "||" || m_op	== "or") {
				// IF LHS is true, we go to continue. If it's false, we go to check.
				// The reason for this is short circuiting; L || R will only ever evaluate R
				// if L is false; since true || x will always evaluate to true, independent of x.
				GE::builder()->CreateCondBr(LHS, continueBlock, checkBlock);
				GE::builder()->SetInsertPoint(checkBlock);
				GE::builder()->CreateStore(RHS, booleanVal);
			}

			GE::builder()->CreateBr(continueBlock);
			GE::builder()->SetInsertPoint(continueBlock);
			m_value = GE::builder()->CreateLoad(booleanVal);
			return m_value;
		}
	}

	if (IsCompareOp(m_op) == false) {
		m_value = GE::builder()->CreateBinOp(GetBinOpFunction(LHS, m_LHS->isSigned(), m_op, RHS, m_RHS->isSigned()), LHS, RHS);

		// If we're doing arithmetic with pointers and integers, cast m_value back to a pointer
		if (m_LHS->getType()->isPointerTy() && m_RHS->getType()->isIntegerTy()) {
			CastingEngine::CastValue(&m_value, m_RHS->getType(), m_LHS->getType(), true);
		}

		return m_value;
	} else {
		bool isFPOp = LHS->getType()->isFloatingPointTy() && RHS->getType()->isFloatingPointTy();
		CmpInst::Predicate pred = GetBinOpPredComp(LHS, m_LHS->isSigned(), m_op, RHS, m_RHS->isSigned());

		if (isFPOp) {
			m_value = GE::builder()->CreateFCmp(pred, LHS, RHS);
		} else {
			m_value = GE::builder()->CreateICmp(pred, LHS, RHS);
		}

		return m_value;
	}
}

std::string BinOpExpr::string() {
	std::stringstream ss;
	ss << "(" << m_LHS->string() << " " << m_op << " " << m_RHS->string() << ")";
	return ss.str();
}

void BinOpExpr::resolve() {
	ASTNode::resolve();

	if (m_resolved) return;
	m_resolved = true;

	if (m_op == "=" && isVarExpr(m_LHS)) {
		VarExpr* vExpr = getVarExpr(m_LHS);

		if (vExpr->getType()->isVoidTy() || vExpr->getType()->isVarTy()) {
			vExpr->setType(m_RHS->getType());
			vExpr->resolve();
		}
	}

	//
	// Determine type
	//
	if (IsAssignOp(m_op)) {
		OrangeTy* t = m_LHS->getType();
		m_type = t->isVoidTy() ? m_RHS->getType() : t;
	} else {
		OrangeTy *lType = m_LHS->getType();
		OrangeTy *rType = m_RHS->getType();

		if (lType->isVoidTy()) {
			throw CompilerMessage(*m_LHS, m_LHS->string() + " does not exist!");
		} else if (rType->isVoidTy()) {
			throw CompilerMessage(*m_RHS, m_RHS->string() + " does not exist!");
		}

		if (IsCompareOp(m_op) || IsCustomOp(m_op)) {
			m_type = IntTy::getUnsigned(1);
		} else {
			m_type = CastingEngine::GetHighestPrecedence(lType, rType);
		}
	}
}

bool BinOpExpr::isSigned() {
	if (IsCompareOp(m_op)) return false;
	if (IsCustomOp(m_op)) return false;

	return m_LHS->isSigned() || m_RHS->isSigned();
}

bool BinOpExpr::isConstant() {
	return m_LHS->isConstant() && m_RHS->isConstant();
}

BinOpExpr::BinOpExpr(Expression* LHS, StrElement op, Expression* RHS) : m_LHS(LHS), m_RHS(RHS), m_op(op) {
	addChild("m_LHS", m_LHS);
	addChild("m_RHS", m_RHS);
}
