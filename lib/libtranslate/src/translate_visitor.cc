//
// Copyright 2014-2016 Robert Fratto. See the LICENSE.txt file at the top-level
// directory of this distribution.
//
// Licensed under the MIT license <http://opensource.org/licenses/MIT>. This file
// may not be copied, modified, or distributed except according to those terms.
//

#include <stdexcept>
#include <llvm/IR/IRBuilder.h>
#include <libast/typecheck.h>

#include "translate_visitor.h"
#include "type_converter.h"

using namespace orange::translate;


llvm::Value* TranslateVisitor::GetValue(Node* node) {
	auto it = mValues.find(node->id);
	if (it == mValues.end()) return nullptr;
	return it->second;
}

void TranslateVisitor::SetValue(Node* node, llvm::Value* value) {
	mValues[node->id] = value;
}

void TranslateVisitor::VisitLoopStmt(LoopStmt* node) {
	throw std::runtime_error("Don't know how to handle LoopStmt");
}

void TranslateVisitor::VisitForeachStmt(ForeachStmt* node) {
	throw std::runtime_error("Don't know how to handle ForeachStmt");
}

void TranslateVisitor::VisitBreakStmt(BreakStmt* node) {
	throw std::runtime_error("Don't know how to handle BreakStmt");
}

void TranslateVisitor::VisitContinueStmt(ContinueStmt* node) {
	throw std::runtime_error("Don't know how to handle ContinueStmt");
}

void TranslateVisitor::VisitYieldStmt(YieldStmt* node) {
	throw std::runtime_error("Don't know how to handle YieldStmt");
}

void TranslateVisitor::VisitReturnStmt(ReturnStmt* node) {
	if (mCurrentBlock == nullptr) return;

	llvm::Value* retValue = nullptr;

	if (node->value != nullptr) {
		mWalker.WalkExpr(this, node->value);
		retValue = GetValue(node->value);
	}

	auto val = mBuilder->CreateRet(retValue);
	SetValue(node, val);
}

void TranslateVisitor::VisitAggregateStmt(AggregateStmt* node) {
	throw std::runtime_error("Don't know how to handle AggregateStmt");
}

void TranslateVisitor::VisitExternFuncStmt(ExternFuncStmt* node) {
	throw std::runtime_error("Don't know how to handle ExternFuncStmt");
}

void TranslateVisitor::VisitEnumValue(EnumValue* node) {
	throw std::runtime_error("Don't know how to handle EnumValue");
}

void TranslateVisitor::VisitEnumStmt(EnumStmt* node) {
	throw std::runtime_error("Don't know how to handle EnumStmt");
}

void TranslateVisitor::VisitClassStmt(ClassStmt* node) {
	throw std::runtime_error("Don't know how to handle ClassStmt");
}

void TranslateVisitor::VisitInterfaceStmt(InterfaceStmt* node) {
	throw std::runtime_error("Don't know how to handle InterfaceStmt");
}

void TranslateVisitor::VisitExtendStmt(ExtendStmt* node) {
	throw std::runtime_error("Don't know how to handle ExtendStmt");
}

void TranslateVisitor::VisitGetterStmt(GetterStmt* node) {
	throw std::runtime_error("Don't know how to handle GetterStmt");
}

void TranslateVisitor::VisitSetterStmt(SetterStmt* node) {
	throw std::runtime_error("Don't know how to handle SetterStmt");
}

void TranslateVisitor::VisitPropertyStmt(PropertyStmt* node) {
	throw std::runtime_error("Don't know how to handle PropertyStmt");
}

void TranslateVisitor::VisitThrowStmt(ThrowStmt* node) {
	throw std::runtime_error("Don't know how to handle ThrowStmt");
}

void TranslateVisitor::VisitDeleteStmt(DeleteStmt* node) {
	throw std::runtime_error("Don't know how to handle DeleteStmt");
}

void TranslateVisitor::VisitExprStmt(ExprStmt* node) {
	throw std::runtime_error("Don't know how to handle ExprStmt");
}

void TranslateVisitor::VisitVarDeclExpr(VarDeclExpr* node) {
	throw std::runtime_error("Don't know how to handle VarDeclExpr");
}

void TranslateVisitor::VisitIntValue(IntValue* node) {
	auto orangeTy = mCurrentContext->GetNodeType(node);
	auto llvmTy = GetLLVMType(orangeTy);
	auto llvmVal = llvm::APInt(GetIntegerBitWidth(orangeTy), node->value, IsSignedType(orangeTy));
	auto val = llvm::Constant::getIntegerValue(llvmTy, llvmVal);
	SetValue(node, val);
}

void TranslateVisitor::VisitUIntValue(UIntValue* node) {
	throw std::runtime_error("Don't know how to handle UIntValue");
}

void TranslateVisitor::VisitFloatValue(FloatValue* node) {
	throw std::runtime_error("Don't know how to handle FloatValue");
}

void TranslateVisitor::VisitDoubleValue(DoubleValue* node) {
	throw std::runtime_error("Don't know how to handle DoubleValue");
}

void TranslateVisitor::VisitStringValue(StringValue* node) {
	throw std::runtime_error("Don't know how to handle StringValue");
}

void TranslateVisitor::VisitCharValue(CharValue* node) {
	throw std::runtime_error("Don't know how to handle CharValue");
}

void TranslateVisitor::VisitThisID(ThisID* node) {
	throw std::runtime_error("Don't know how to handle ThisID");
}

void TranslateVisitor::VisitNamedIDExpr(NamedIDExpr* node) {
	throw std::runtime_error("Don't know how to handle NamedIDExpr");
}

void TranslateVisitor::VisitTempIDExpr(TempIDExpr* node) {
	throw std::runtime_error("Don't know how to handle TempIDExpr");
}

void TranslateVisitor::VisitDtorIDExpr(DtorIDExpr* node) {
	throw std::runtime_error("Don't know how to handle DtorIDExpr");
}

void TranslateVisitor::VisitAccessIDExpr(AccessIDExpr* node) {
	throw std::runtime_error("Don't know how to handle AccessIDExpr");
}

void TranslateVisitor::VisitLongBlockExpr(LongBlockExpr* node) {
	for (auto stmt : node->statements) mWalker.WalkNode(this, stmt);
}

void TranslateVisitor::VisitShortBlockExpr(ShortBlockExpr* node) {
	throw std::runtime_error("Don't know how to handle ShortBlockExpr");
}

void TranslateVisitor::VisitBinOpExpr(BinOpExpr* node) {
	throw std::runtime_error("Don't know how to handle BinOpExpr");
}

void TranslateVisitor::VisitUnaryExpr(UnaryExpr* node) {
	throw std::runtime_error("Don't know how to handle UnaryExpr");
}

void TranslateVisitor::VisitTupleExpr(TupleExpr* node) {
	throw std::runtime_error("Don't know how to handle TupleExpr");
}

void TranslateVisitor::VisitArrayExpr(ArrayExpr* node) {
	throw std::runtime_error("Don't know how to handle ArrayExpr");
}

void TranslateVisitor::VisitArrayRangeExpr(ArrayRangeExpr* node) {
	throw std::runtime_error("Don't know how to handle ArrayRangeExpr");
}

void TranslateVisitor::VisitArrayAccessExpr(ArrayAccessExpr* node) {
	throw std::runtime_error("Don't know how to handle ArrayAccessExpr");
}

void TranslateVisitor::VisitMemberAccessExpr(MemberAccessExpr* node) {
	throw std::runtime_error("Don't know how to handle MemberAccessExpr");
}

void TranslateVisitor::VisitNamedExpr(NamedExpr* node) {
	throw std::runtime_error("Don't know how to handle NamedExpr");
}

void TranslateVisitor::VisitConditionalBlock(ConditionalBlock* node) {
	throw std::runtime_error("Don't know how to handle ConditionalBlock");
}

void TranslateVisitor::VisitIfExpr(IfExpr* node) {
	throw std::runtime_error("Don't know how to handle IfExpr");
}

void TranslateVisitor::VisitTernaryExpr(TernaryExpr* node) {
	throw std::runtime_error("Don't know how to handle TernaryExpr");
}

void TranslateVisitor::VisitSwitchPattern(SwitchPattern* node) {
	throw std::runtime_error("Don't know how to handle SwitchPattern");
}

void TranslateVisitor::VisitSwitchExpr(SwitchExpr* node) {
	throw std::runtime_error("Don't know how to handle SwitchExpr");
}

void TranslateVisitor::VisitClassConstraint(ClassConstraint* node) {
	throw std::runtime_error("Don't know how to handle ClassConstraint");
}

void TranslateVisitor::VisitDefaultCtorConstraint(DefaultCtorConstraint* node) {
	throw std::runtime_error("Don't know how to handle DefaultCtorConstraint");
}

void TranslateVisitor::VisitBaseConstraint(BaseConstraint* node) {
	throw std::runtime_error("Don't know how to handle BaseConstraint");
}

void TranslateVisitor::VisitDataConstraint(DataConstraint* node) {
	throw std::runtime_error("Don't know how to handle DataConstraint");
}

void TranslateVisitor::VisitTypeConstraint(TypeConstraint* node) {
	throw std::runtime_error("Don't know how to handle TypeConstraint");
}

void TranslateVisitor::VisitGenerics(Generics* node) {
	throw std::runtime_error("Don't know how to handle Generics");
}

void TranslateVisitor::VisitFunctionExpr(FunctionExpr* node) {
	throw std::runtime_error("Don't know how to handle FunctionExpr");
}

void TranslateVisitor::VisitCatchBlock(CatchBlock* node) {
	throw std::runtime_error("Don't know how to handle CatchBlock");
}

void TranslateVisitor::VisitTryExpr(TryExpr* node) {
	throw std::runtime_error("Don't know how to handle TryExpr");
}

void TranslateVisitor::VisitCastExpr(CastExpr* node) {
	throw std::runtime_error("Don't know how to handle CastExpr");
}

void TranslateVisitor::VisitFunctionCallExpr(FunctionCallExpr* node) {
	throw std::runtime_error("Don't know how to handle FunctionCallExpr");
}

void TranslateVisitor::VisitNewExpr(NewExpr* node) {
	throw std::runtime_error("Don't know how to handle NewExpr");
}

void TranslateVisitor::SetCurrentBlock(llvm::BasicBlock* currentBlock) {
	mCurrentBlock = currentBlock;
	mBuilder->SetInsertPoint(mCurrentBlock);
}

void TranslateVisitor::SetCurrentContext(orange::analysis::NodeTypeContext* ctx) {
	mCurrentContext = ctx;
}

TranslateVisitor::TranslateVisitor(Walker& walker, std::shared_ptr<llvm::Module> mod) :
	mWalker(walker), mModule(mod) {
	mBuilder = std::make_shared<LLVMIRBuilder>(llvm::getGlobalContext());
}



