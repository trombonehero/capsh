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

#include "capsicum.h"

using namespace capsh;

using std::ostringstream;
using std::string;


bool capsh::inCapabilityMode() throw(CError)
{
	unsigned int mode;

	if (cap_getmode(&mode) < 0) throw CError("cap_getmode()");
	return (mode != 0);
}

void capsh::assertNotInCapabilityMode(const string& action)
	throw(CError, CapabilityModeException)
{
	if (inCapabilityMode())
		throw CapabilityModeException(action);
}

string capsh::rightsString(cap_rights_t rights)
{
	ostringstream oss;

	if (rights & CAP_READ)				oss << "CAP_READ ";
	if (rights & CAP_WRITE)				oss << "CAP_WRITE ";
	if (rights & CAP_SEEK)				oss << "CAP_SEEK ";
	if (rights & CAP_GETPEERNAME)		oss << "CAP_GETPEERNAME ";
	if (rights & CAP_GETSOCKNAME)		oss << "CAP_GETSOCKNAME ";
	if (rights & CAP_FCHFLAGS)			oss << "CAP_FCHFLAGS ";
	if (rights & CAP_IOCTL)				oss << "CAP_IOCTL ";
	if (rights & CAP_FSTAT)				oss << "CAP_FSTAT ";
	if (rights & CAP_MMAP)				oss << "CAP_MMAP ";
	if (rights & CAP_FCNTL)				oss << "CAP_FCNTL ";
	if (rights & CAP_EVENT)				oss << "CAP_EVENT ";
	if (rights & CAP_FSYNC)				oss << "CAP_FSYNC ";
	if (rights & CAP_FCHOWN)			oss << "CAP_FCHOWN ";
	if (rights & CAP_FCHMOD)			oss << "CAP_FCHMOD ";
	if (rights & CAP_FTRUNCATE)		oss << "CAP_FTRUNCATE ";
	if (rights & CAP_FLOCK)				oss << "CAP_FLOCK ";
	if (rights & CAP_FSTATFS)			oss << "CAP_FSTATFS ";
	if (rights & CAP_REVOKE)			oss << "CAP_REVOKE ";
	if (rights & CAP_FEXECVE)			oss << "CAP_FEXECVE ";
	if (rights & CAP_FPATHCONF)		oss << "CAP_FPATHCONF ";
	if (rights & CAP_FUTIMES)			oss << "CAP_FUTIMES ";
	if (rights & CAP_ACL_GET)			oss << "CAP_ACL_GET ";
	if (rights & CAP_ACL_SET)			oss << "CAP_ACL_SET ";
	if (rights & CAP_ACL_DELETE)		oss << "CAP_ACL_DELETE ";
	if (rights & CAP_ACL_CHECK)		oss << "CAP_ACL_CHECK ";
	if (rights & CAP_EXTATTR_GET)		oss << "CAP_EXTATTR_GET ";
	if (rights & CAP_EXTATTR_SET)		oss << "CAP_EXTATTR_SET ";
	if (rights & CAP_EXTATTR_DELETE)	oss << "CAP_EXTATTR_DELETE ";
	if (rights & CAP_EXTATTR_LIST)	oss << "CAP_EXTATTR_LIST ";
	if (rights & CAP_MAC_GET)			oss << "CAP_MAC_GET ";
	if (rights & CAP_MAC_SET)			oss << "CAP_MAC_SET ";
	if (rights & CAP_ACCEPT)			oss << "CAP_ACCEPT ";
	if (rights & CAP_CONNECT)			oss << "CAP_CONNECT ";
	if (rights & CAP_BIND)				oss << "CAP_BIND ";
	if (rights & CAP_GETSOCKOPT)		oss << "CAP_GETSOCKOPT ";
	if (rights & CAP_SETSOCKOPT)		oss << "CAP_SETSOCKOPT ";
	if (rights & CAP_LISTEN)			oss << "CAP_LISTEN ";
	if (rights & CAP_SHUTDOWN)			oss << "CAP_SHUTDOWN ";
	if (rights & CAP_PEELOFF)			oss << "CAP_PEELOFF ";
	if (rights & CAP_LOOKUP)			oss << "CAP_LOOKUP ";
	if (rights & CAP_SEM_POST)			oss << "CAP_SEM_POST ";
	if (rights & CAP_SEM_WAIT)			oss << "CAP_SEM_WAIT ";
	if (rights & CAP_SEM_GETVALUE)	oss << "CAP_SEM_GETVALUE ";
	if (rights & CAP_KEVENT)			oss << "CAP_KEVENT ";
	if (rights & CAP_PDGETPID)			oss << "CAP_PDGETPID ";
	if (rights & CAP_PDWAIT)			oss << "CAP_PDWAIT ";
	if (rights & CAP_PDKILL)			oss << "CAP_PDKILL ";
	if (rights & CAP_MAPEXEC)			oss << "CAP_MAPEXEC ";
	if (rights & CAP_TTYHOOK)			oss << "CAP_TTYHOOK ";
	if (rights & CAP_FCHDIR)			oss << "CAP_FCHDIR ";
	if (rights & CAP_FSCK)				oss << "CAP_FSCK ";
	if (rights & CAP_ATBASE)			oss << "CAP_ATBASE ";
	if (rights & CAP_ABSOLUTEPATH)	oss << "CAP_ABSOLUTEPATH ";
	if (rights & CAP_CREATE)			oss << "CAP_CREATE ";
	if (rights & CAP_DELETE)			oss << "CAP_DELETE ";
	if (rights & CAP_MKDIR)				oss << "CAP_MKDIR ";
	if (rights & CAP_RMDIR)				oss << "CAP_RMDIR ";
	if (rights & CAP_MKFIFO)			oss << "CAP_MKFIFO ";

	return oss.str();
}

