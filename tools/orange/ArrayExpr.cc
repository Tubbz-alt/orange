/*
** Copyright 2014-2015 Robert Fratto. See the LICENSE.txt file at the top-level 
** directory of this distribution.
**
** Licensed under the MIT license <http://opensource.org/licenses/MIT>. This file 
** may not be copied, modified, or distributed except according to those terms.
*/ 

#include <orange/ArrayExpr.h>
#include <orange/generator.h>

std::string ArrayExpr::string() {
	std::stringstream ss; 

	ss << "[ "; 

	for (unsigned int i = 0; i < m_elements.size(); i++) {
		ss << m_elements[i]->string();

		if (i + 1 < m_elements.size()) {
			ss << ", ";
		}
	}

	ss << " ]";

	return ss.str();
}

Value* ArrayExpr::Codegen() {
	// 1. allocate space for our expr 
	// 2. check if we're a constant array 
	//		a. if we are a constant array:
	//			i. use vector<Constant *> 
	//			ii. push back each code generated expr into the vactor  
	//			iii. create array from ConstantArray::get  
	//			iv. store array into our allocated space 
	//		b. if we're not a constant array: 
	//			i. go through each of our elements 
	//			ii. GEP (getelementpointer) to element i (space, 0, i) in the allocated space 
	//			iii. store our code generated expr into the GEP 
	// 3. return our allocated space 

	OrangeTy* arrType = getType();
	Value* space = arrType->allocate();

	if (isConstant()) {
		std::vector<Constant *> consts; 

		for (auto element : m_elements) {
			Value* arr_ele = element->Codegen();

			// Cast it to the type of the array 
			if (element->getType()->isArrayTy() == false)
				CastingEngine::CastValueToType(&arr_ele, arrType->getArrayElementType(), element->isSigned(), true);

			consts.push_back((Constant *)arr_ele);
		}

		Value* storeArr = ConstantArray::get((ArrayType *)arrType->getLLVMType(), consts);
		GE::builder()->CreateStore(storeArr, space);
	} else {
		for (unsigned int i = 0; i < m_elements.size(); i++) {
			Value* arr_ele = m_elements[i]->Codegen();
			
			if (arr_ele == nullptr) {
				throw std::runtime_error("ArrayExpr::Codegen(): arr_ele did not generate a value!");
			}

			// This is only necessary for the non-constant array since constant exprs 
			// will never require to be loaded 
			if (m_elements[i]->returnsPtr()) {
				arr_ele = GE::builder()->CreateLoad(arr_ele);
			}

			// Cast it to the type of the array 
			if (m_elements[i]->getType()->isArrayTy() == false)
				CastingEngine::CastValueToType(&arr_ele, arrType->getArrayElementType(), m_elements[i]->isSigned(), true);
			
			Value *gep = GE::builder()->CreateConstInBoundsGEP2_64(space, 0, i);

			GE::builder()->CreateStore(arr_ele, gep);
		}
	}

	m_value = space; 
	return m_value;
}

ASTNode* ArrayExpr::clone() {
	ArgList cloned_elements; 

	for (auto element : m_elements) {
		cloned_elements.push_back((Expression *)element->clone());
	}

	return new ArrayExpr(cloned_elements);
}

OrangeTy* ArrayExpr::getType() {
	// If we don't have any elements, we're an int*. 
	if (m_elements.size() == 0) {
		return IntTy::getSigned(64)->getPointerTo();
	}
	
	// First, we need to find the highest precedence type from all of the elements.
	// Recursively traverse the elements in our array, collecting all of the types,
	// looking for the highest one. 
	OrangeTy* highestType = nullptr;

	for (auto element : m_elements) {
		OrangeTy* ty = element->getType();

		while (ty->isArrayTy()) {
			ty = ty->getArrayElementType();
		}

		if (highestType == nullptr) {
			highestType = ty; 
		} else {
			highestType = CastingEngine::GetFittingType(highestType, ty);
		}
	}

	// We have to work backwards to create our array.
	std::vector<int> elementStack; 
	elementStack.push_back(m_elements.size());

	OrangeTy* pty = m_elements[0]->getType(); 
	while (pty->isArrayTy()) {
		elementStack.push_back(pty->getArrayElements());
		pty = pty->getArrayElementType();
	}	

	// Now that we know the stack of elements, we can create our type.
	OrangeTy* ret = highestType;

	for (auto i = elementStack.rbegin(); i < elementStack.rend(); i++) {
		ret = ArrayTy::get(ret, *i);
	}

	return ret; 
}

bool ArrayExpr::isConstant() {
	for (auto element : m_elements) {
		if (element->isConstant() == false || element->getType()->isArrayTy()) 
			return false; 
	}

	// Only return true if nothing else returned false.
	return true; 
}


ArrayExpr::ArrayExpr(ArgList elements) {
	m_elements = elements;

	for (unsigned int i = 0; i < m_elements.size(); i++) {
		auto element = m_elements[i];
			
		std::stringstream ss; 
		ss << "element" << i; 

		addChild(ss.str(), element);
	}
}
