/*
** Copyright 2014-2016 Robert Fratto. See the LICENSE.txt file at the top-level
** directory of this distribution.
**
** Licensed under the MIT license <http://opensource.org/licenses/MIT>. This file
** may not be copied, modified, or distributed except according to those terms.
*/

#pragma once

#include "Expression.h"
#include "Named.h"
#include "Accessible.h"

namespace Orange { class Type; }

class Parameter : public Expression, public Named, public Accessible {
public:
	virtual llvm::Value* getPointer() const override;
	
	virtual bool hasPointer() const override;
	
	virtual llvm::Value* getValue() const override;
	
	virtual ASTNode* copy() const override;
	
	virtual std::vector<ObjectBase**> getMemberNodes() override;
	
	virtual std::vector<std::vector<ObjectBase *>*> getMemberLists() override;

	virtual bool isAccessible() const override;
	
	virtual Expression* access(OString name, const ASTNode* hint)
		const override;
	
	virtual void setType(const Orange::Type* type) override;
	
	Parameter(const Orange::Type* type, OString name);
	Parameter(const Parameter& other);
};