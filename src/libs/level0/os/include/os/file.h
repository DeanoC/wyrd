// Full license at end of file
// Summary: Apache - original from The-Forge modified by DeanoC

#pragma once
#ifndef WYRD_OS_FILE_H
#define WYRD_OS_FILE_H

#include "core/core.h"

typedef void* Os_FileHandle;

enum Os_FileMode {
  Os_FM_Read = 1,
  Os_FM_Write = Os_FM_Read << 1,
  Os_FM_Append = Os_FM_Write << 1,
  Os_FM_Binary = Os_FM_Append << 1,
  Os_FM_ReadWrite = Os_FM_Read | Os_FM_Write,
  Os_FM_ReadAppend = Os_FM_Read | Os_FM_Append,
  Os_FM_WriteBinary = Os_FM_Write | Os_FM_Binary,
  Os_FM_ReadBinary = Os_FM_Read | Os_FM_Binary
};

enum Os_FileSeekDir {
  Os_FSD_Begin = 0,
  Os_FSD_Current,
  Os_FSD_End,
};

/// Low level file system interface providing basic file I/O operations
/// Implementations platform dependent
EXTERN_C Os_FileHandle Os_FileOpen(char const *filename, enum Os_FileMode mode);
EXTERN_C bool Os_FileClose(Os_FileHandle handle);
EXTERN_C bool Os_FileIsOpen(Os_FileHandle handle);


EXTERN_C void Os_FileFlush(Os_FileHandle handle);
EXTERN_C size_t Os_FileRead(Os_FileHandle handle, void *buffer, size_t byteCount);
EXTERN_C size_t Os_FileWrite(Os_FileHandle handle, void const *buffer, size_t byteCount);
EXTERN_C bool Os_FileSeek(Os_FileHandle handle, int64_t offset, enum Os_FileSeekDir origin);
EXTERN_C int64_t Os_FileTell(Os_FileHandle handle);
EXTERN_C size_t Os_FileSize(Os_FileHandle handle);
EXTERN_C bool Os_FileIsEOF(Os_FileHandle handle);

#endif //WYRD_OS_FILE_H

/*
 * Copyright (c) 2018-2019 Confetti Interactive Inc.
 *
 * This file is part of The-Forge
 * (see https://github.com/ConfettiFX/The-Forge).
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
*/
