/*
** Copyright 2014-2015 Robert Fratto. See the LICENSE.txt file at the top-level
** directory of this distribution.
**
** Licensed under the MIT license <http://opensource.org/licenses/MIT>. This file
** may not be copied, modified, or distributed except according to those terms.
*/

#include <grove/Expression.h>
#include <grove/types/Type.h>

#include <llvm/IR/Instruction.h>
#include <llvm/IR/IRBuilder.h>

llvm::Value* Expression::getValue() const
{
	return m_value;
}

llvm::Value* Expression::getPointer() const
{
	return nullptr;
}

bool Expression::hasPointer() const
{
	return false;
}

llvm::Value* Expression::castTo(Type *ty)
{
	auto op = (llvm::Instruction::CastOps)getType()->castOperation(ty);
	
	if (op == NO_CAST)
	{
		return getValue();
	}
	
	auto casted = IRBuilder()->CreateCast(op, getValue(), ty->getLLVMType());
	return casted;
}

void Expression::setValue(llvm::Value *value)
{
	m_value = value;
}
