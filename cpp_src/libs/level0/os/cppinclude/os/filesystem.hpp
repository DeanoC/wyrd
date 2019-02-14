#pragma once
#ifndef WYRD_OS_FILESYSTEM_HPP
#define WYRD_OS_FILESYSTEM_HPP

#include "os/filesystem.h"
#include "tinystl/string.h"
namespace FileSystem
{
	void SplitPath(
		tinystl::string const& fullPath, tinystl::string* pathName, tinystl::string* fileName, tinystl::string* extension,
		bool lowercaseExtension);

	tinystl::string GetInternalPath(tinystl::string const& path)
	{
		char tmp[2048];
		if(FS_GetInternalPath(path.c_str(), tmp, 2048))
			return tinystl::string(tmp);
		else return tinystl::string();
	}

	tinystl::string GetNativePath(tinystl::string const& path)
	{
		char tmp[2048];
		if(FS_GetNativePath(path.c_str(), tmp, 2048))
			return tinystl::string(tmp);
		else return tinystl::string();
	}

	tinystl::string GetCurrentDir()
	{
		char cwd[2048];
		if(FS_GetCurrentDir(cwd, 2048)) return tinystl::string(cwd);
		else return tinystl::string();
	}

	tinystl::string GetExtension()
	{

	}
}

#endif //WYRD_FILESYSTEM_HPP
