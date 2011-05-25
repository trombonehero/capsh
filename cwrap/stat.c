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

#include <dlfcn.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>

#include <sys/capability.h>
#include <sys/stat.h>

#include <libcapsicum.h>

#include "capmode.h"


//! Signature of the libc stat(2) and lstat(2) functions.
typedef int (*stat_signature)(const char*, struct stat*);

//! Signature of the fstatat(2) function.
typedef int (*fstatat_signature)(int, const char*, struct stat*, int);


int
stat(const char *path, struct stat *sb)
{
	static stat_signature unwrapped = 0;
	if (unwrapped == 0)
		unwrapped = (stat_signature) dlsym(RTLD_NEXT, "stat");

	if (!in_capability_mode())
		return unwrapped(path, sb);

	return fstatat(AT_FDCWD, path, sb, 0);
}


int
lstat(const char *path, struct stat *sb)
{
	static stat_signature unwrapped = 0;
	if (unwrapped == 0)
		unwrapped = (stat_signature) dlsym(RTLD_NEXT, "lstat");

	if (!in_capability_mode())
		return unwrapped(path, sb);

	return fstatat(AT_FDCWD, path, sb, AT_SYMLINK_NOFOLLOW);
}


int
fstatat(int fd, const char *path, struct stat *sb, int flag)
{
	static fstatat_signature unwrapped = 0;
	if (unwrapped == 0)
		unwrapped = (fstatat_signature) dlsym(RTLD_NEXT, "fstatat");

	struct lc_fdlist *fds = lc_fdlist_global();
	if (fds == 0)
		return unwrapped(fd, path, sb, flag);

	// If operating relative to a directory, or not in capability mode, just go
	// ahead and perform the system call.
	if ((fd != AT_FDCWD) || !in_capability_mode())
		return unwrapped(fd, path, sb, flag);

	const char *relative_name;
	fd = lc_fdlist_find(
		fds, "org.freebsd.capsicum.capsh", "files", path, &relative_name);

	if (fd < 0) {
		// We didn't find the file in the cache.
		errno = ENOENT;
		return (-1);
	}

	if (strnlen(relative_name, 1) != 0)
		return unwrapped(fd, relative_name, sb, flag);

	return fstat(fd, sb);
}

