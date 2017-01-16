/*
** Copyright 2014-2017 Robert Fratto. See the LICENSE.txt file at the top-level
** directory of this distribution.
**
** Licensed under the MIT license <http://opensource.org/licenses/MIT>. This file
** may not be copied, modified, or distributed except according to those terms.
*/

#pragma once

#include <string>
#include <vector>

int invokeProgramWithOptions(const char *program, std::vector<const char *> options, bool close = false);
const char *programPath(std::string programName);

void invokeLinkerWithOptions(std::vector<const char *> options);
const char *linkerPath();
