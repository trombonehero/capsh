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

#include <poll.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/wait.h>

#include <libcapsicum.h>

#include "exec.h"
#include "file.h"

using namespace capsh;

using std::string;
using std::vector;

extern char **environ;


//! The rights required by the run-time linker to link a library.
static cap_rights_t LIB_RIGHTS =
	CAP_FSTAT | CAP_FSTATFS | CAP_MMAP | CAP_READ | CAP_SEEK | CAP_MAPEXEC;

//! The rights required by the run-time linker in order to execute a binary.
static cap_rights_t BIN_RIGHTS = LIB_RIGHTS | CAP_FEXECVE;


// TODO: get the following from libcapsicum/rtld
//! What libcapsicum and the runtime linker agree to call RTLD.
static const char *RTLD_SUBSYSTEM = "org.freebsd.rtld-elf-cap";

//! The class name of libraries we want the runtime linker to preload.
static const char *RTLD_PRELOAD_CLASS = "PreloadLibrary";


Exec::Exec(const CommandLine& commandline, const Path& path) throw(CError)
	: path(path), commandline(commandline)
{
	child.fd = -1;
	child.events = POLLERR | POLLHUP | POLLNVAL;

	// TODO: actually understand library paths
	libcwrap = File::open("cwrap/libcwrap.so", LIB_RIGHTS);
}


void Exec::execute() throw(CommandError, FatalError)
{
	if (commandline.size() == 0) return;

	vector<string> args;
	vector<File> files;


	File command;
	try
	{
		command = path.findFile(commandline[0], BIN_RIGHTS);

		args.push_back(command.getName());
		files.push_back(command);
	}
	catch (FileNotInPathException e) { throw CommandError(e.getMessage()); }


	// Open files which have been explicitly named at the command line.
	for (unsigned int i = 1; i < commandline.size(); i++)
	{
		try
		{
			File file = File::open(commandline[i]);

			args.push_back(file.getName());
			files.push_back(file);
		}
		catch (NoSuchFileException& e)
		{
			args.push_back(commandline[i]);
		}
		catch (CError& e) { throw CommandError(e.getMessage()); }
		catch (FileException& e) { throw CommandError(e.getMessage()); }
		catch (...)
		{
			char buffer[120];
			sprintf(buffer, "%s:%d - unknown exception", __FILE__, __LINE__);
			throw CommandError(buffer);
		}
	}


	// Run it!
	try { runInSandbox(command, args, files); }
	catch (CError& e) { throw FatalError(e.getMessage()); }
}


void Exec::runInSandbox(
		File binary, const vector<string>& args, const vector<File>& files)
	throw(CError)
{
	struct lc_fdlist *fds = lc_fdlist_new();
	for (vector<File>::const_iterator i = files.begin(); i != files.end(); i++)
		if (0 != lc_fdlist_add(fds, "org.freebsd.capsicum.capsh", "files",
					i->getName().c_str(), i->getDescriptor()))
			throw CError("lc_fdlist_add('" + i->getName() + "')");

	// Preload the libcwrap wrapper to intercept global-namespace calls like
	// open() and stat().
	if (0 != lc_fdlist_add(fds, RTLD_SUBSYSTEM, RTLD_PRELOAD_CLASS,
				libcwrap.getName().c_str(), libcwrap.getDescriptor()))
		throw CError("Error in lc_fdlist_addcap()");

	char **argv = new char*[args.size() + 1];
	for (unsigned int i = 0; i < args.size(); i ++)
		argv[i] = const_cast<char*>(args[i].c_str());
	argv[args.size()] = NULL;

	int flags = LCH_PERMIT_STDOUT | LCH_PERMIT_STDERR;
	struct lc_sandbox *sandbox;
	if (0 != lch_startfd(
			binary.getDescriptor(), binary.getName().c_str(),
			argv, flags, fds, &sandbox))
		throw CError("lch_startfd()");

	if (0 != lch_getprocdesc(sandbox, &child.fd))
		throw CError("lch_getprocdesc()");

	while (true)
	{
		int ready = poll(&child, 1, 1000);
		if (ready == -1) throw CError("poll(sandboxed child)");
		else if (ready == 0) continue;

		if (child.revents & POLLHUP) break;
	}

	delete [] argv;
}

