#pragma once
#ifndef WYRD_VFILE_INTERFACE_H
#define WYRD_VFILE_INTERFACE_H

struct VFile_Interface_t;

typedef void (*VFile_CloseFunc)(struct VFile_Interface_t *);
typedef void (*VFile_FlushFunc)(struct VFile_Interface_t *);
typedef size_t (*VFile_ReadFunc)(struct VFile_Interface_t *, void *buffer, size_t byteCount);
typedef size_t (*VFile_WriteFunc)(struct VFile_Interface_t *, void const *buffer, size_t byteCount);
typedef bool (*VFile_SeekFunc)(struct VFile_Interface_t *, int64_t offset, enum VFile_SeekDir origin);
typedef int64_t (*VFile_TellFunc)(struct VFile_Interface_t *);
typedef size_t (*VFile_SizeFunc)(struct VFile_Interface_t *);

static const uint32_t InterfaceMagic = 0xDEA0DEA0;

// this is the shared header to the various vfile specific
// all vfile derived should have this as the first part of there
// own structure
typedef struct VFile_Interface_t {

  uint32_t magic;

  VFile_CloseFunc closeFunc;
  VFile_FlushFunc flushFunc;
  VFile_ReadFunc readFunc;
  VFile_WriteFunc writeFunc;
  VFile_SeekFunc seekFunc;
  VFile_TellFunc tellFunc;
  VFile_SizeFunc sizeFunc;

} VFile_Interface_t;

#endif //WYRD_INTERFACE_H
