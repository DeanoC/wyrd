#pragma once
#ifndef WYRD_VFILE_MEMFILE_H
#define WYRD_VFILE_MEMFILE_H

#include "core/core.h"

typedef struct VFile_MemFile_t {
  void *memory;
  size_t size;
  bool takeOwnership;
  size_t offset;
} VFile_MemFile_t;

#endif //WYRD_VFILE_MEMFILE_H
