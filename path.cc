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
#include <sstream>

#include <fcntl.h>
#include <stdlib.h>

#include "file.h"
#include "path.h"

using namespace capsh;

using std::ostringstream;
using std::string;
using std::vector;


Path Path::create()
{
	string env(getenv("PATH"));
	vector<File> dirs;

	size_t i = 0, j;
	do
	{
		j = env.find(":", i + 1);
		string name = env.substr(i, j - i);
		i = j + 1;

		try { dirs.push_back(File::open(name)); }
		catch (NoSuchFileException e) { /* ignore invalid dirs */ }
	}
	while (j != string::npos);

	return Path(dirs);
}

File Path::findFile(const string& name) const
	throw (CError, FileNotInPathException)
{
	for (vector<File>::const_iterator i = path.begin(); i != path.end(); i++)
	{
		try
		{
			File f = File::openat(i->getDescriptor(), name);
			if (f.isValid()) return f;
		}
		catch (NoSuchFileException e) { /* ignore unhelpful directories */ }
	}

	throw FileNotInPathException(name, *this);
}

string Path::str() const
{
	ostringstream oss;

	oss << "[ ";
	for (vector<File>::const_iterator i = path.begin(); i != path.end(); i++)
		oss << "'" << i->getName() << "' ";

	oss << "]";

	return oss.str();
}

Path::Path(const vector<File>& path) throw() : path(path) {}


FileNotInPathException::FileNotInPathException(
		const string& filename, const Path& path) throw()
	: Exception("No file '" + filename + "' in path " + path.str())
{
}

