#pragma once
#ifndef WYRD_VFILE_OSFILE_H
#define WYRD_VFILE_OSFILE_H
#include "core/core.h"
#include "os/file.h"

typedef struct VFile_OsFile_t {
  Os_FileHandle fileHandle;
} VFile_OsFile_t;

#endif //WYRD_VFILE_OSFILE_H
