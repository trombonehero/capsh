/*
 * Copyright 2011 Jonathan Anderson
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <iostream>

#include <signal.h>

#include "cmdproc.h"

using namespace capsh;

using std::cerr;
using std::endl;


int main(int argc, char *argv[])
{
	sigignore(SIGINT);
	CommandProcessor cp;

	try
	{
		if (argc <= 1) cp.run();
		else
		{
			char **commands = argv + 1;
			char **end = argv + argc;
	
			CommandLine commandLine(commands, end);
			cp.process(commandLine);
		}
	}
	catch (FatalError& e) { cerr << e.what() << endl; }
	catch (Exception& e) { cerr << e.what() << endl; }
	catch (...) { cerr << "uncaught exception" << endl; }

	return 0;
}


CommandError::CommandError(const std::string& message) throw() : Exception(message) {}
CommandError::~CommandError() throw() {}

FatalError::FatalError(const std::string& message) throw() : Exception(message) {}
FatalError::~FatalError() throw() {}
