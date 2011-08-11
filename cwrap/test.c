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

#include <sys/mman.h>

#include <err.h>
#include <fcntl.h>
#include <stdio.h>

#include <libcapsicum.h>


int main(int argc, char *argv[])
{
	const char *filename = "/etc/passwd";

	// Open the descriptor normally (this should fall through).
	int fd = open(filename, O_RDONLY);
	if (fd < 0)
		err(-1, "open('%s')", filename);

	// Store it in a descriptor list.
	struct lc_fdlist *fds = lc_fdlist_new();
	if (0 != lc_fdlist_addcap(fds,
			"org.freebsd.capsicum.capsh", "file", filename,
			fd, CAP_READ | CAP_SEEK))
		err(-1, "lc_fdlist_addcap()");

	// A place to store the FD list.
	//
	// TODO: this should move to after the cap_enter() call once shm_open()
	//       is allowed in capability mode with SHM_ANON.
	int shm = shm_open(SHM_ANON, O_RDWR | O_CREAT | O_TRUNC, 0600);
	if (shm < 0) err(-1, "shm_open(SHM_ANON)");


	// Enter capability mode - further calls to open() should fail.
	cap_enter();

	// This open() call should fail; we are in capability mode.
	if (open(filename, O_RDONLY) != -1)
		errx(-1, "open('%s') succeeded in capability mode", filename);

	// Save the descriptor list as the default, global one.
	if (0 != lc_fdlist_set_global(fds, shm))
		err(-1, "lc_fdlist_set_global()");

	// Now open() should work, because the descriptor has been cached.
	int copy = open(filename, O_RDONLY);
	if (copy < 0)
		err(-1, "open(cached descriptor for '%s')", filename);

	// Everything works!
	printf("OK\n");

	return 0;
}

