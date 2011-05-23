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

#ifndef CAPSH_CAPSICUM_H
#define CAPSH_CAPSICUM_H

#include <string>

#include <sys/capability.h>

#include "exception.h"

namespace capsh
{
	/// Something may not be done in capablity mode.
	class CapabilityModeException : public Exception
	{
		public:
		CapabilityModeException(const std::string& action) throw()
			: Exception("Cannot " + action + " in capability mode")
		{
		}
//		~CapabilityModeException throw() {}
	};

	bool inCapabilityMode() throw(CError);
	void assertNotInCapabilityMode(const std::string& action)
		throw(CError, CapabilityModeException);

	std::string rightsString(cap_rights_t rights);
}

#endif // CAPSH_H
