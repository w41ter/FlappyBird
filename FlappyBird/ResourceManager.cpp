#include "main.h"

#include "ResourceManager.h"

namespace games
{
	ResourceLibrary<resource, wstring> &wstringResource()
	{
		static ResourceLibrary<resource, wstring> source;

		return source;
	}
}
