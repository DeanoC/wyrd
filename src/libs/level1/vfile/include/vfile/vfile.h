#pragma once
#ifndef WYRD_VFILE_VFILE_H
#define WYRD_VFILE_VFILE_H

#include "core/core.h"
#include "os/file.h"

typedef struct {} *VFile_Handle;

enum VFile_SeekDir {
  VFile_SD_Begin = 0,
  VFile_SD_Current,
  VFile_SD_End,
};

enum {
  VFile_Type_Invalid = 0,
  VFile_Type_OsFile = 1,
  VFile_Type_Memory = 2
};

EXTERN_C VFile_Handle VFile_FromFile(char const *filename, enum Os_FileMode mode);
EXTERN_C VFile_Handle VFile_FromMemory(void *memory, size_t size, bool takeOwnership);
EXTERN_C VFile_Handle VFile_ToBuffer(size_t initialSize);

EXTERN_C void VFile_Close(VFile_Handle handle);
EXTERN_C void VFile_Flush(VFile_Handle handle);
EXTERN_C size_t VFile_Read(VFile_Handle handle, void *buffer, size_t byteCount);
EXTERN_C size_t VFile_Write(VFile_Handle handle, void const *buffer, size_t byteCount);
EXTERN_C bool VFile_Seek(VFile_Handle handle, int64_t offset, enum VFile_SeekDir origin);
EXTERN_C int64_t VFile_Tell(VFile_Handle handle);
EXTERN_C size_t VFile_Size(VFile_Handle handle);
EXTERN_C char const *VFile_GetName(VFile_Handle handle);
EXTERN_C bool VFile_IsEOF(VFile_Handle handle);
EXTERN_C uint32_t VFile_GetType(VFile_Handle handle);
EXTERN_C void* VFile_GetTypeSpecificData(VFile_Handle handle);

#endif //WYRD_VFILE_VFILE_H
