#ifndef __AST_H__
#define __AST_H__

#include <iostream>
#include <vector>
#include <sstream>
#include <stack>
#include <map>

#include "llvm/Analysis/Passes.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/PassManager.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Transforms/Scalar.h"

#ifdef DEBUG
#define DEBUG_MSG(str) std::cout << "DEBUG: " << str << std::endl;
#else 
#define DEBUG_MSG(str) ;
#endif 

using namespace llvm;

Type *getType(std::string typeStr);

class ArgExpr;
class Expression;

typedef std::vector<Expression *> ExprList;

class Statement;
class Block;
class BaseVal;

class ArgList : public std::vector<ArgExpr *>  {
public:
	bool isVarArg = false;

	ArgList *clone();
};

// Base
class Statement {
public:
	virtual std::string getClass() { return "Statement"; }
	virtual Value* Codegen() { return nullptr; }
	virtual Type *getType() { return Type::getVoidTy(getGlobalContext()); }
	virtual std::string string() { return ""; }
	virtual bool returnsPtr() { return false; }

	bool resolved = false;

	virtual Statement* clone() { return new Statement(); }
	
	// any function that implements resolve can only 
	// be resolved once 
	virtual void resolve() { 
		if (resolved)
			return; 
		resolved = true; 
	};

	virtual ~Statement() { };
};


class Expression : public Statement { 
public:
	virtual std::string getClass() { return "Expression"; }
	virtual Type *getType() { return Type::getVoidTy(getGlobalContext()); }
	virtual bool isSigned() { return false; }
	virtual bool isConstant() { return true; } 

	virtual Statement* clone() { return new Expression(); }
};

class AnyType {
private:
	std::vector<uint64_t> arrays;

	bool mArrayType = false;
	int mArraySize = 0;
	AnyType() { }
public:
	int arrays_size() const { return arrays.size(); }

	std::string type; 
	bool isSigned();
	int numPointers = 0; 
	bool arrayType() const { return mArrayType; }  
	int arraySize() const { return mArraySize; }
	int absoluteNumPtrs();

	std::string string(bool no_brackets = false);

	Type *getType();

	static AnyType *Create(Type *t);
	AnyType* clone();

	AnyType(std::string *type, int numPointers, std::vector<BaseVal *> *arrays);
};


class StrVal : public Expression {
public:
	virtual std::string getClass() { return "StrVal"; }
	std::string value; 

	virtual std::string string() {  
		std::stringstream ss;
		ss << "\"" << value << "\"";
		return ss.str();
	}

	virtual Type *getType() { return Type::getInt8PtrTy(getGlobalContext()); }

	Value* Codegen();

	virtual Statement* clone() { 
		StrVal *ret = new StrVal("\"\"");
		ret->value = value; 
		return ret; 
	}

	StrVal(std::string v);
};

class BaseVal : public Expression { 

};

class ValFactory {
public:
	std::string value;
	std::string size;

	BaseVal *produce();
};

Value *replace(Value *del, Value *v);


#endif
