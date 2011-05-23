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

#ifndef CAPSH_PATH_H
#define CAPSH_PATH_H

#include <string>
#include <vector>

#include <sys/capability.h>

#include <fcntl.h>

#include "exception.h"
#include "file.h"

namespace capsh
{
	class Path;

	/// A file could not be found in a path.
	class FileNotInPathException : public Exception
	{
		public:
		FileNotInPathException(const std::string& filename, const Path& path)
			throw();

		~FileNotInPathException() throw() {}
	};

	/// Represents the binary search path.
	class Path
	{
		public:
		static Path create();
		File findFile(const std::string& name, cap_rights_t rights) const
			throw (CError, FileNotInPathException);

		/// A human-readable version of the path.
		std::string str() const;

		private:
		Path(const std::vector<File>& path) throw();
	
		std::vector<File> path;
	};
}

#endif
