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

#include <cstdlib>
#include <string>
#include <vector>

#include <stdlib.h>
#include <unistd.h>

#include <sys/wait.h>

#include "exec.h"
#include "file.h"

using namespace capsh;

using std::string;
using std::vector;

extern char **environ;


void Exec::execute() throw(CommandError, FatalError)
{
	if (commandline.size() == 0) return;
	const string& command = commandline[0];

	int file = path.findFile(command);
	if (file == -1) throw CommandError("Unknown command: '" + command + "'");

	// Open files where we can.
	char** argv = new char*[commandline.size() + 1];
	for (unsigned int i = 0; i < commandline.size(); i++)
	{
		try
		{
			File file = File::open(commandline[i]);
			// save file.getDescriptor() into an FD list
			commandline[i] = file.getName();
		}
		catch(NoSuchFileException& e) {}
		catch(FileException& e) { throw CommandError(e.getMessage()); }
		catch(...)
		{
			char buffer[120];
			sprintf(buffer, "%s:%d - unknown exception", __FILE__, __LINE__);
			throw CommandError(buffer);
		}

		argv[i] = const_cast<char*>(commandline[i].c_str());
	}
	argv[commandline.size()] = NULL;

	pid_t pid = fork();
	if (pid == 0) fexecve(file, argv, environ);
	else
	{
		int status;
		do { wait(&status); } while(!WIFEXITED(status));
	}

	delete [] argv;
}
