/*
 * Copyright (c) 2018 Confetti Interactive Inc.
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

#pragma once

#include "core/core.h"
#include "os/thread.hpp"
#include "os/file.hpp"
#include "tinystl/vector.h"
#include "tinystl/string.h"

/// Logging subsystem.
class LogManager {
 public:
  LogManager();
  ~LogManager();

  void Open(const tinystl::string& fileName);
  void Close();

  void SetLevel();
  void SetTimeStamp(bool enable);
  void SetQuiet(bool quiet);

  bool GetTimeStamp() const { return mRecordTimestamp; }
  tinystl::string GetLastMessage() const { return mLastMessage; }
  bool IsQuiet() const { return mQuietMode; }

 private:
  void msg(char const *level, char const *file, int line, const char *function, char const *msg);
  static void errorMsg(char const *file, int line, const char *function, char const *msg);
  static void warningMsg(char const *file, int line, const char *function, char const *msg);
  static void infoMsg(char const *file, int line, const char *function, char const *msg);
  static void debugMsg(char const *file, int line, const char *function, char const *msg);
  static void failedAssert(char const *file, int line, char const *msg);

  /// Mutex for threaded operation.
  Mutex logMutex;
  Os::File logFile;
  tinystl::string mLastMessage;
  bool mRecordTimestamp;
  bool mInWrite;
  bool mQuietMode;
  static Core_Logger log;
  static Core_Logger oldLog;
};
