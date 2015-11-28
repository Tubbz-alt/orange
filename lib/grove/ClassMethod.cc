/*
** Copyright 2014-2015 Robert Fratto. See the LICENSE.txt file at the top-level
** directory of this distribution.
**
** Licensed under the MIT license <http://opensource.org/licenses/MIT>. This file
** may not be copied, modified, or distributed except according to those terms.
*/

#include <grove/ClassMethod.h>

ClassMethod::ClassMethod(OString name, std::vector<Parameter *> params)
: Function(name, params)
{
	// Do nothing.
}