/*
** Copyright 2014-2015 Robert Fratto. See the LICENSE.txt file at the top-level
** directory of this distribution.
**
** Licensed under the MIT license <http://opensource.org/licenses/MIT>. This file
** may not be copied, modified, or distributed except according to those terms.
*/

#pragma once 

#include "CodeBase.h"

class OString : public CodeBase
{
private:
	std::string m_str;
public:
	operator std::string() const;
	
	// Comparisons
	bool operator==(OString& other);
	bool operator==(std::string& other);
	bool operator==(const char* other);
	
	// Methods
	std::string str() const;
	
	// Constructors
	OString(OString& str);
	OString(std::string& str);
	OString(const char* str);
};