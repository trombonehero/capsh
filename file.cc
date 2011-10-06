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

#include <sstream>

#include <err.h>
#include <fcntl.h>

#include <libcapsicum.h>

#include <sys/stat.h>

#include "capsicum.h"
#include "file.h"

using namespace capsh;

using std::ostringstream;
using std::string;


cap_rights_t File::DEFAULT_RIGHTS = CAP_FSTAT | CAP_READ | CAP_SEEK;

File File::open(const string& name, cap_rights_t rights)
	throw(CapabilityModeException, CError, FileException)
{
	assertNotInCapabilityMode("open('" + name + "')");
	return File::openat(AT_FDCWD, name, rights);
}


File File::openat(int base, const string& name, cap_rights_t rights)
	throw(CError, FileException)
{
	if (base == AT_FDCWD)
		assertNotInCapabilityMode("openat(AT_FDCWD, '" + name + "'");

	// First, check to see if we've been given the correct filename.
	struct stat s;
	if (::fstatat(base, name.c_str(), &s, 0) == 0)
		return File(name, File::openFD(base, name, rights));

	// Not found; perhaps it's a tagged filename (e.g. 'file:rwx').
	size_t lastColon = name.rfind(":");
	if (lastColon == string::npos) throw NoSuchFileException(name);

	string tag = name.substr(lastColon + 1);
	string untagged = name.substr(0, lastColon);

	if (tag.length() > 3) throw NoSuchFileException(name);
	if (stat(untagged.c_str(), &s) != 0) throw NoSuchFileException(name);

	// The file exists! can we open it with the requested mode?
	rights |= CAP_SEEK;
	if (tag.find("r") != string::npos) rights |= CAP_READ;
	if (tag.find("w") != string::npos) rights |= CAP_WRITE;
	if (tag.find("x") != string::npos) rights |= CAP_FEXECVE;

	int fd = File::openFD(base, untagged, rights);
	if (fd == -1) throw FilePermissionException(untagged, tag);

	return File(untagged, fd);
}

int File::openFD(int base, const string& name, cap_rights_t rights)
	throw(CError)
{
	int flags = (rights & CAP_FEXECVE) ? O_EXEC : 0;
	if ((rights & CAP_READ) and (rights & CAP_WRITE)) flags |= O_RDWR;
	else if(rights & CAP_READ) flags |= O_RDONLY;
	else if(rights & CAP_WRITE) flags |= O_WRONLY;

	int fd = ::openat(base, name.c_str(), flags);
	if (fd < 0)
	{
		if (errno == ENOENT) return fd;
		else
		{
			ostringstream err;
			err << "openat(" << base << ", '" << name << "', ";

			if (flags & O_EXEC) err << "O_EXEC ";
			if (flags & O_RDWR) err << "O_RDWR ";
			else if (flags & O_WRONLY) err << "O_WRONLY ";
			else err << "O_RDONLY ";
			err << ")";
			throw CError(err.str());
		}
	}

	if (lc_limitfd(fd, rights) != 0)
	{
		ostringstream err;
		err << "lc_limitfd(";

		cap_rights_t fd_rights;
		if (cap_getrights(fd, &fd_rights) < 0) err << "FD:";
		else err << "cap:[" << rightsString(fd_rights) << "]:";

		err << fd << ", [" << rightsString(rights) << "])";

		cap_rights_t baserights;
		if (cap_getrights(base, &baserights) != -1)
			err << " [base rights: " << rightsString(baserights) << "]";

		throw CError(err.str());
	}

	return fd;
}

