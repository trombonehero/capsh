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

#ifndef CAPSH_H
#define CAPSH_H

#include <string>
#include <vector>

#include "exception.h"

// The main libtecla class.
struct GetLine;

namespace capsh
{
	/// Something went wrong in the execution of a command, but the shell is still ok.
	class CommandError : public Exception
	{
		public:
		CommandError(const std::string& message) throw();
		~CommandError() throw();
	};

	/// Something about the command execution means that we need to kill the whole shell.
	class FatalError : public Exception
	{
		public:
		FatalError(const std::string& message = "") throw();
		~FatalError() throw();
	};
	
	class Command
	{
		public:
		virtual void execute() throw(CommandError, FatalError) = 0;
	};
}

#endif // CAPSH_H
