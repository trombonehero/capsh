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
#include <sstream>

#include <libtecla.h>

#include "builtins.h"
#include "cmdproc.h"
#include "exec.h"
#include "StringSwitch.h"

using namespace capsh;

using std::cout;
using std::endl;
using std::istringstream;
using std::string;


CommandProcessor::CommandProcessor() throw(FatalError)
	: tecla(new_GetLine(1024, 2048)), path(Path::create())
{
	if (tecla == NULL)
		throw FatalError("NULL tecla instance; error in libtecla?");
}

CommandProcessor::~CommandProcessor() throw()
{
	if (tecla != NULL)
		del_GetLine(tecla);
}

/// Process a single command line.
void CommandProcessor::process(const CommandLine& line) throw(CommandError, FatalError)
{
	if (line.size() == 0) return;

	Command* command = llvm::StringSwitch<Command*>(line[0])
		// builtins
		.Cases("", "noop", new NoOp())
		.Case("exit", new Exit())
		.Case("help", new Help())
		.Case("moo", new Moo())

		// not a builtin: find in the PATH and execute
		.Default(new Exec(line, path))
		;

	command->execute();
	delete command;
}

/// Prompt the user for commands and execute them.
void CommandProcessor::run(const string& prompt) throw(FatalError)
{
	while(true)
	{
		char *line = gl_get_line(tecla, prompt.c_str(), NULL, -1);
		if (line == NULL) continue;

		istringstream input(line);
		CommandLine commandline;
		while (input)
		{
			string token;
			input >> token;

			if (token.length() > 0)
				commandline.push_back(token);
		}

		try { process(commandline); }
		catch (CommandError& e) { cout << e.what() << endl; }
	}
}
