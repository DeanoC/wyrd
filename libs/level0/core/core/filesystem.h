#pragma once
#ifndef WYRD_FILESYSTEM_H
#define WYRD_FILESYSTEM_H

#include "core.h"
#if PLATFORM_OS == OSX
#include "macos/experimental/filesystem"
#error MACOS still doesn't support filesystem sadly, use cppfs
#else
#include <experimental/filesystem>
#endif

namespace std
{
	namespace filesystem = std::experimental::filesystem;
}

#endif //WYRD_FILESYSTEM_H
