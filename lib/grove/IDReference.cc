/*
** Copyright 2014-2015 Robert Fratto. See the LICENSE.txt file at the top-level
** directory of this distribution.
**
** Licensed under the MIT license <http://opensource.org/licenses/MIT>. This file
** may not be copied, modified, or distributed except according to those terms.
*/

#include <grove/IDReference.h>
#include <grove/Valued.h>
#include <grove/Named.h>
#include <grove/Typed.h>

#include <util/assertions.h>

llvm::Value* IDReference::getPointer() const
{
	return findNode()->getPointer();
}

bool IDReference::hasPointer() const
{
	return findNode()->hasPointer();
}

Valued* IDReference::findNode() const
{
	return findNamed(getName())->as<Valued *>();
}

std::string IDReference::getName() const
{
	return m_name;
}

ASTNode* IDReference::copy() const
{
	return new IDReference(m_name);
}

void IDReference::findDependencies()
{
	auto ref = findNamed(getName());
	assertExists(ref, "No variable with this name exists.");
	
	addDependency(ref->as<ASTNode *>());
}

void IDReference::resolve()
{
	auto ref = findNamed(getName());
	assertExists(ref, "No variable with this name exists.");

	auto typed = findNamed(getName())->as<Typed *>();

	auto ty = typed->getType();
	assertExists(ty, "Could not assign type.");

	setType(ty);
}

llvm::Value* IDReference::getValue() const
{
	return findNode()->getValue();
}

IDReference::IDReference(std::string name)
{
	if (name == "")
	{
		throw std::invalid_argument("name cannot be null.");
	}

	m_name = name;
}
