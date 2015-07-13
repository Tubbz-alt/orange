/*
** Copyright 2014-2015 Robert Fratto. See the LICENSE.txt file at the top-level 
** directory of this distribution.
**
** Licensed under the MIT license <http://opensource.org/licenses/MIT>. This file 
** may not be copied, modified, or distributed except according to those terms.
*/ 

#ifndef __ORANGE_VALUES_H__
#define __ORANGE_VALUES_H__
#include "AST.h"
#include "OrangeTypes.h"

class BaseVal : public Expression { 
public:
	virtual bool isConstant() { return true; }

	virtual BaseVal* increment() { return nullptr; }

	virtual std::string valueStr() { return ""; }
};

class StrElement : public CodeElement {
private:
	std::string m_str; 
public:
	bool operator==(std::string RHS) {
		return m_str == RHS; 
	} 

	bool operator==(StrElement& RHS) {
		return m_str == RHS.m_str;
	}

	friend StrElement operator+(const char *s, StrElement& element);
	friend StrElement operator+(StrElement element, const char *s);
	friend std::ostream& operator<< (std::ostream& stream, const StrElement& element);

	operator std::string() const { return m_str; }

	StrElement(std::string str) : m_str(str) {}
	StrElement(const char *str) : m_str(str) {}
};

class StrVal : public Expression {
private:
	StrElement m_value; 
public:
	virtual std::string getClass() { return "StrVal"; }

	virtual std::string string() {  
		std::stringstream ss;
		ss << "\"" << m_value << "\"";
		return ss.str();
	}

	Value* Codegen();

	virtual ASTNode* clone() { 
		StrVal *ret = new StrVal("\"\"");
		ret->m_value = m_value; 
		return ret; 
	}

	StrVal(StrElement value);
};


class ValFactory {
public:
	std::string value;
	std::string size;

	BaseVal *produce();
};

class UIntVal : public BaseVal {
public:
	virtual std::string getClass() { return "UIntVal"; }

	Value* Codegen();

	uint64_t value;
	uint8_t size;

	virtual bool isSigned() { return false; }

	virtual std::string string() {
		std::stringstream ss;
		ss << "(uint" << (uint64_t)size << ")" << value;
		return ss.str();
	}

	virtual ASTNode* clone() { 
		UIntVal *ret = new UIntVal(value, size); 
		return ret;
	}

	virtual BaseVal* increment() {
		return new UIntVal(value + 1, size);
	}

	virtual std::string valueStr() { 
		std::stringstream ss; 
		ss << value; 
		return ss.str();
	}

	UIntVal() { 
		m_type = IntTy::getUnsigned(size); 
	}

	UIntVal(uint64_t val, uint8_t size) : value(val), size(size) {
		m_type = IntTy::getUnsigned(size); 
	} 
};

class IntVal : public BaseVal {
public:
	virtual std::string getClass() { return "IntVal"; }

	Value* Codegen();

	int64_t value;
	uint8_t size;

	virtual bool isSigned() { return true; }

	virtual std::string string() {
		std::stringstream ss;
		ss << "(int" << (uint64_t)size << ")" << value;
		return ss.str();
	}

	virtual ASTNode* clone() { 
		IntVal* ret = new IntVal(value, size);
		return ret;
	}

	virtual BaseVal* increment() {
		return new IntVal(value + 1, size);
	}

	virtual std::string valueStr() { 
		std::stringstream ss; 
		ss << value; 
		return ss.str();
	}

	IntVal() { 
		m_type = IntTy::getSigned(size); 
	}

	IntVal(int64_t val, uint8_t size) : value(val), size(size) {
		m_type = IntTy::getSigned(size); 
	}
};

class FloatVal : public BaseVal {
public:
	virtual std::string getClass() { return "FloatVal"; }

	Value* Codegen();

	float value;

	virtual std::string string() {
		std::stringstream ss;
		ss << "(float)" << value;
		return ss.str();
	}

	virtual ASTNode* clone() { 
		FloatVal* ret = new FloatVal(value);
		return ret;
	}

	virtual BaseVal* increment() {
		return new FloatVal(value + 1);
	}

	virtual std::string valueStr() { 
		std::stringstream ss; 
		ss << value; 
		return ss.str();
	}

	FloatVal() {
		m_type = FloatTy::get();
	}

	FloatVal(float val) : value(val) {
		m_type = FloatTy::get();
	}
};

class DoubleVal : public BaseVal {
public:
	virtual std::string getClass() { return "DoubleVal"; }

	Value* Codegen();

	double value;

	virtual std::string string() {
		std::stringstream ss;
		ss << "(double)" << value;
		return ss.str();
	}

	virtual ASTNode* clone() { 
		DoubleVal* ret = new DoubleVal(value);
		return ret;
	}

	virtual BaseVal* increment() {
		return new DoubleVal(value + 1);
	}

	virtual std::string valueStr() { 
		std::stringstream ss; 
		ss << value; 
		return ss.str();
	}

	DoubleVal() {
		m_type = DoubleTy::get();
	}

	DoubleVal(double val) : value(val) {
		m_type = DoubleTy::get();
	} 
};

#endif 