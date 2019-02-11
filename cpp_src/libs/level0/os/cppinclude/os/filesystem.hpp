#pragma once
#ifndef WYRD_OS_FILESYSTEM_HPP
#define WYRD_OS_FILESYSTEM_HPP

#include "os/filesystem.h"
#include "tinystl/string.h"
namespace FileSystem
{
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

}

#endif //WYRD_FILESYSTEM_HPP
