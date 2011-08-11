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
#include <stdarg.h>
#include <string.h>

#include <sys/capability.h>

#include <libcapsicum.h>

#include "capmode.h"


//! Signature of the libc open(2) function.
typedef int (*open_signature)(const char*, int, ...);


//! FDList-aware wrapper around libc's open().
int open(const char *path, int flags, ...)
{
	static open_signature unwrapped = 0;
	if (unwrapped == 0)
		unwrapped = (open_signature) dlsym(RTLD_NEXT, "open");

	va_list ap;
	va_start(ap, flags);

	struct lc_fdlist *fds = lc_fdlist_global();

	// If there is no global FD list, fall back to libc.
	if (fds == 0)
		return unwrapped(path, flags, ap);

	// Try to find the file.
	const char *relative_name;
	int fd = lc_fdlist_find(fds, "org.freebsd.capsicum.capsh", "files",
		path, &relative_name);

	if (fd >= 0) {
		// Perhaps we found a directory descriptor and
		// relative filename?
		if (strnlen(relative_name, 1) != 0)
			fd = openat(fd, relative_name, flags, ap);

		return fd;
	}

	// If we failed to find the file, and we're not in capability mode,
	// fall through to libc's open().
	if (!in_capability_mode())
		return unwrapped(path, flags, ap);

	// We're in capability mode, so we can't fall back to libc.
	// We have a list of files, but the one we're looking for isn't in it.
	// Return error, with the errno previously set by lc_fdlist_find().
	return (-1);
}

