/*
** Copyright 2014-2015 Robert Fratto. See the LICENSE.txt file at the top-level 
** directory of this distribution.
**
** Licensed under the MIT license <http://opensource.org/licenses/MIT>. This file 
** may not be copied, modified, or distributed except according to those terms.
*/ 

#include <orange/SymTable.h>

static int lastID = 0;

SymTable* SymTable::parent() const {
	return m_parent;
}

SymTable* SymTable::container() const {
	return m_container; 
}

bool SymTable::create(std::string name, ASTNode* node) {
	// If it's already created, return false.
	if (m_objs.find(name) != m_objs.end()) return false; 

	// We require null to not be false 
	if (node == nullptr) return false; 

	// We also require the name to not be blank
	if (name == "") return false; 

	m_objs[name] = node; 
	return true; 
}

ASTNode* SymTable::find(std::string name) {
	auto it = m_objs.find(name); 

	// return null if it doesn't exist 
	if (it == m_objs.end()) {
		return m_parent ? m_parent->find(name) : nullptr;
	}  


	return it->second; 
}

ASTNode* SymTable::findStructure(std::string className) {
	SymTable* ptr = this; 

	while (ptr != nullptr) {
		// If the ptr has a structure and the class matches our param, we found it
		if (ptr->m_structure && ptr->m_structure->getClass() == className) {
			return ptr->m_structure; 
		}

		// Continue to traverse up the tree
		ptr = ptr->m_parent; 
	}

	// We didn't find it; return nullptr.
	return nullptr; 
}

void SymTable::setStructure(ASTNode* structure) {
	m_structure = structure; 
}

SymTable* SymTable::clone() {
	SymTable* clonedSymtab = new SymTable(m_parent);
	clonedSymtab->m_structure = m_structure; 
	clonedSymtab->m_objs = m_objs;
	return clonedSymtab;
}

int SymTable::ID() const { 
	return m_ID; 
}

bool SymTable::root() const {
	return m_root;
}


SymTable::SymTable(bool isRoot) {
	m_ID = lastID++;
	m_root = isRoot; 
	m_parent = nullptr;
}

SymTable::SymTable(SymTable* parent) {
	m_ID = lastID++;
	m_parent = m_container = parent;
}

SymTable::SymTable(SymTable* parent, SymTable* container) {
	m_ID = lastID++;
	m_parent = parent; 
	m_container = container; 
}

