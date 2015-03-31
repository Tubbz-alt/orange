#ifndef __BIN_OP_EXPR_H__
#define __BIN_OP_EXPR_H__

#include "AST.h"

class BinOpExpr : public Expression {
public:
	virtual std::string getClass() { return "BinOpExpr"; }

	Value* Codegen();

	Type *getType();

	Expression *LHS;
	std::string op;
	Expression *RHS;

	virtual bool isSigned();

	virtual void resolve();

	virtual bool isConstant() { return LHS->isConstant() && RHS->isConstant(); }

	virtual ASTNode* clone() { 
		BinOpExpr *ret = new BinOpExpr((Expression *)LHS->clone(), op, (Expression *)RHS->clone());
		return ret;
	}

	virtual std::string string() {
		std::stringstream ss;
		ss << "( " << LHS->string() << " " << op << " " << RHS->string() << " )";
		return ss.str();
	}

	BinOpExpr(Expression *LHS, std::string op, Expression *RHS);

	~BinOpExpr();
};

#endif