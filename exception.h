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

#ifndef CAPSH_EXCEPTION_H
#define CAPSH_EXCEPTION_H

#include <exception>
#include <ostream>
#include <string>

#include <errno.h>


namespace capsh
{
	/// Base exception class.
	class Exception : public std::exception
	{
		public:
		Exception(const std::string& message) throw()
			: message(message)
		{
		}
		~Exception() throw() {}

		std::string getMessage() const { return message; }
		const char *what() const throw() { return message.c_str(); }

		private:
		std::string message;
	};

	/// A c-style error has occurred.
	class CError : public Exception
	{
		public:
		CError(const std::string& context, int errnum = errno) throw();
		~CError() throw() {}
	};

	std::ostream& operator << (std::ostream& os, const Exception& e);
}

#endif // CAPSH_EXECEPTION_H
