// Full license at end of file
// Summary: Apache - original from The-Forge modified by DeanoC

#pragma once
#ifndef WYRD_OS_FILE_H
#define WYRD_OS_FILE_H

#include "core/core.h"

typedef void* File_Handle;

typedef enum File_Mode
{
	FM_Read = 1,
	FM_Write = FM_Read << 1,
	FM_Append = FM_Write << 1,
	FM_Binary = FM_Append << 1,
	FM_ReadWrite = FM_Read | FM_Write,
	FM_ReadAppend = FM_Read | FM_Append,
	FM_WriteBinary = FM_Write | FM_Binary,
	FM_ReadBinary = FM_Read | FM_Binary
} File_Mode;

typedef enum File_SeekDir
{
	FSD_BEGIN = 0,
	FSD_CUR,
	FSD_END,
} File_SeekDir;

/// Low level file system interface providing basic file I/O operations
/// Implementations platform dependent
EXTERN_C File_Handle File_Open(char const* filename, const File_Mode mode);
EXTERN_C bool File_Close(File_Handle handle);
EXTERN_C void File_Flush(File_Handle handle);
EXTERN_C size_t File_Read(File_Handle handle, void* buffer, size_t byteCount);
EXTERN_C size_t File_Write(File_Handle handle, void const* buffer, size_t byteCount);
EXTERN_C bool File_Seek(File_Handle handle, int64_t offset, File_SeekDir origin);
EXTERN_C int64_t File_Tell(File_Handle handle);
EXTERN_C size_t File_Size(File_Handle handle);

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
