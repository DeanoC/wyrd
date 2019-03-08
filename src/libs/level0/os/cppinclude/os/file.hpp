// Full license at end of file
// Summary: Apache - original from The-Forge modified by DeanoC

#pragma once
#ifndef WYRD_OS_FILE_HPP
#define WYRD_OS_FILE_HPP

#include "core/core.h"
#include "os/file.h"
#include "tinystl/string.h"

namespace Os {

// C++ wrapper for File_*
struct File {

  static File Open(tinystl::string const& filename, const Os_FileMode mode) {
    return {Os_FileOpen(filename.c_str(), mode), filename};
  }

  bool Close() { return Os_FileClose(handle); }
  bool IsOpen() const { return handle != nullptr; }

  tinystl::string_view Name() const { return tinystl::string_view(name.c_str(), name.size()); }

  void Flush() { Os_FileFlush(handle); }
  size_t Read(void *buffer, size_t byteCount) { return Os_FileRead(handle, buffer, byteCount); }
  size_t Write(void const *buffer, size_t byteCount) { return Os_FileWrite(handle, buffer, byteCount); };
  bool Seek(int64_t offset, Os_FileSeekDir origin) { return Os_FileSeek(handle, offset, origin); }
  int64_t Tell() const { return Os_FileTell(handle); };

  Os_FileHandle handle = nullptr;
  tinystl::string name;
};

} // end Os namespace

#endif //WYRD_OS_FILE_HPP
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
