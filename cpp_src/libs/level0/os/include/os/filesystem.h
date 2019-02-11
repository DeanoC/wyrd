// Full license at end of file
// Summary: Apache - original from The-Forge modified by DeanoC

#pragma once
#ifndef WYRD_OS_FILESYSTEM_H
#define WYRD_OS_FILESYSTEM_H

#include "core/core.h"
#include "os/file.h"

EXTERN_C bool FS_CopyFile(char const* src, char const* dst, bool bFailIfExists);
EXTERN_C bool FS_Delete(char const* fileName);
EXTERN_C bool FS_DirExists(char const* pathName);
EXTERN_C bool FS_CreateDir(char const* pathName);
EXTERN_C int  FS_SystemRun(char const* fileName, int argc, char const* argv[]);
EXTERN_C size_t	FS_GetLastModifiedTime(char const* fileName);
EXTERN_C bool FS_Exists(char const* fileName);
EXTERN_C uint64_t FS_GetFileSize(File_Handle handle);

EXTERN_C bool FS_GetCurrentDir(char* dirOut, int maxSize);
EXTERN_C bool FS_GetExePath(char* dirOut, int maxSize);
EXTERN_C bool FS_GetProgramFileName(char* dirOut, int maxSize);
EXTERN_C bool FS_GetUserDocumentsDir(char* dirOut, int maxSize);
EXTERN_C bool FS_GetAppPrefsDir(char const* org, char const* app, char* dirOut, int maxSize);
EXTERN_C bool FS_SetCurrentDir(char const* dir, int maxSize);
EXTERN_C bool FS_GetPath(char const* fullPath, char const* dir, int maxSize);
EXTERN_C bool FS_GetFileName(char const* fullPath, char const* dir, int maxSize);
EXTERN_C bool FS_GetExtension(char const* fullPath, char* dirOut, int maxSize);
EXTERN_C bool FS_ReplaceExtension(char const* fullPath, char const* newExtension, char* dirOut, int maxSize);
EXTERN_C bool FS_GetParentPath(char const* pathName, char* dirOut, int maxSize);
EXTERN_C bool FS_GetNativePath(char const* pathName, char* dirOut, int maxSize);

#ifdef __cplusplus
#include "os/filesystem.hpp"
#endif

#endif //WYRD_OS_FILESYSTEM_H

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
