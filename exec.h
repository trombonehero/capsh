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

#ifndef CAPSH_EXEC_H
#define CAPSH_EXEC_H

#include <vector>

#include <poll.h>

#include "capsh.h"
#include "cmdproc.h"
#include "path.h"

namespace capsh
{
	class Exec : public Command
	{
		public:
		Exec(const CommandLine& commandline, const Path& path) throw(CError);
		~Exec() throw() {}

		void execute() throw(CommandError, FatalError);
	
		private:
		void runInSandbox(
				File binary,
				const std::vector<std::string>& args,
				const std::vector<File>& files)
			throw(CError);

		//! The path to search for binary executables.
		const Path& path;

		/**
		 * Library which intercepts certain global-namespace-using libc calls,
		 * such as open().
		 *
		 * If these calls weren't wrapped, they would simply fail with ECAPMODE.
		 * Intercepting them allows us to do something useful, like pre-cache
		 * files which the user explicitly mentiones on the command line. This
		 * way, lots of unmodified software can be made to "speak" capabilities
		 * "under the hood."
		 */
		File libcwrap;

		//! The command line that the user wants to execute.
		std::vector<std::string> commandline;

		/**
		 * The currently-running subprocess.
		 *
		 * capsh does not support multiple concurrent or backgrounded jobs (yet).
		 */
		struct pollfd child;
	};
}

#endif
