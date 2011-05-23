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

#ifndef CAPSH_FILE_H
#define CAPSH_FILE_H

#include "capsicum.h"
#include "exception.h"


namespace capsh
{
	class FileException : public Exception
	{
		public:
		FileException(const std::string& message) throw() : Exception(message) {}
		~FileException() throw() {}
	};
	
	class NoSuchFileException : public FileException
	{
		public:
		NoSuchFileException(const std::string& filename) throw()
			: FileException("No such file '" + filename + "'")
		{
		}
		~NoSuchFileException() throw() {}
	};

	class FilePermissionException : public FileException
	{
		public:
		FilePermissionException(const std::string& file, const std::string& mode)
				throw()
			: FileException("Cannot access file '" + file + "' with mode " + mode)
		{
		}
		~FilePermissionException() throw() {}
	};


	class File
	{
		public:
		static cap_rights_t DEFAULT_RIGHTS;

		/// Open a file relative to an open directory descriptor.
		static File openat(int base, const std::string& name,
				cap_rights_t rights = DEFAULT_RIGHTS)
			throw(CError, FileException);

		/// Open a file using a global name (if allowed).
		static File open(const std::string& name,
				cap_rights_t rights = DEFAULT_RIGHTS)
			throw(CapabilityModeException, CError, FileException);

		File() throw() : name("uninitialized"), fd(-1) {}

		bool isValid() const { return (fd != -1); }
		const std::string& getName() const { return name; }
		int getDescriptor() const { return fd; }
	
		private:
		static int openFD(int base, const std::string& name, cap_rights_t rights)
			throw(CError);
	
		File(const std::string& name, int fd) : name(name), fd(fd) {}
	
		std::string name;
		int fd;
	};
}

#endif
