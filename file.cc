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

#include <err.h>
#include <fcntl.h>

#include <sys/stat.h>

#include "file.h"

using namespace capsh;

using std::string;


File File::open(const string& name) throw(CError, FileException)
{
	return openat(AT_FDCWD, name);
}


File File::openat(int base, const string& name) throw(CError, FileException)
{
	// First, check to see if we've been given the correct filename.
	struct stat s;
	if (fstatat(base, name.c_str(), &s, 0) == 0)
		return File(name, ::openat(base, name.c_str(), O_RDONLY));

	// Perhaps it's a tagged filename (e.g. 'file:rwx')...
	size_t lastColon = name.rfind(":");
	if (lastColon == string::npos) throw NoSuchFileException(name);

	string tag = name.substr(lastColon + 1);
	string untagged = name.substr(0, lastColon);

	if (tag.length() > 3) throw NoSuchFileException(name);
	if (stat(untagged.c_str(), &s) != 0) throw NoSuchFileException(name);

	// The file exists! can we open it with the requested mode?
	bool read = (tag.find("r") != string::npos);
	bool write = (tag.find("w") != string::npos);
	bool exec = (tag.find("x") != string::npos);

	int fd = openat(base, untagged, read, write, exec);
	if (fd == -1) throw FilePermissionException(untagged, tag);
	else return File(untagged, fd);
}


int File::openat(int base, const string& name, bool read, bool write, bool exec)
{
	int flags = exec ? O_EXEC : 0;
	if (read && write) flags |= O_RDWR;
	else if(read) flags |= O_RDONLY;
	else if(write) flags |= O_WRONLY;

	// TODO: wrap everything in capabilities

	return ::openat(base, name.c_str(), flags);
}

