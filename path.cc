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

#include <string>

#include <fcntl.h>
#include <stdlib.h>

#include "path.h"

using namespace capsh;

using std::string;
using std::vector;


Path Path::create()
{
	string env(getenv("PATH"));
	vector<int> dirs;

	size_t i = 0, j;
	do
	{
		j = env.find(":", i + 1);
		string name = env.substr(i, j - i);
		i = j + 1;

		int dir = open(name.c_str(), O_RDONLY);
		if (dir != -1) dirs.push_back(dir);
	}
	while (j != string::npos);

	return Path(dirs);
}

int Path::findFile(const string& name, int flags) const
{
	for (vector<int>::const_iterator i = path.begin(); i != path.end(); i++)
	{
		int fd = openat(*i, name.c_str(), flags);
		if (fd != -1) return fd;
	}

	return -1;
}

Path::Path(const vector<int>& path) : path(path) {}
