#include "core/core.h"
#include "core/logger.h"
#include "os/file.hpp"
#include "os/filesystem.hpp"
#include "logmanager/logmanager.hpp"

#include <cstdio> // sprintf

//#include "../Interfaces/IMemoryManager.h"
Core_Logger LogManager::log = {
    &LogManager::errorMsg,
    &LogManager::warningMsg,
    &LogManager::infoMsg,
    &LogManager::debugMsg,
    &LogManager::failedAssert
};

Core_Logger LogManager::oldLog;

tinystl::string GetTimeStamp() {
  time_t sysTime;
  time(&sysTime);
  tinystl::string dateTime = ctime(&sysTime);
  dateTime.replace('\n', ' ');
  return dateTime;
}

static LogManager *pLogInstance = nullptr;

LogManager::LogManager() :
    mRecordTimestamp(false),
    mInWrite(false),
    mQuietMode(false) {
  pLogInstance = this;

  logFile = Os::File::Open(Os::FileSystem::GetCurrentDir() + "log.log", Os_FM_Write);

  Os::Thread::SetMainThread();
  oldLog = Core_Log;
  Core_Log = log;
}

LogManager::~LogManager() {
  Close();
  pLogInstance = nullptr;
  Core_Log = oldLog;
}

void LogManager::Open(const tinystl::string& fileName) {
  if (fileName.size() == 0) {
    return;
  }

  if (logFile.IsOpen()) {
    if (fileName == tinystl::string(logFile.Name())) {
      return;
    } else {
      Close();
    }
  }

  logFile = Os::File::Open(fileName, Os_FM_Write);
  if (logFile.IsOpen()) {
    LOGINFOF("Opened log file %s", fileName.c_str());
  } else {
    LOGERRORF("Failed to create log file %s", fileName.c_str());
  }
}

void LogManager::Close() {
  if (logFile.IsOpen()) {
    logFile.Close();
  }
}

void LogManager::SetTimeStamp(bool enable) {
  mRecordTimestamp = enable;
}

void LogManager::SetQuiet(bool quiet) {
  mQuietMode = quiet;
}

void LogManager::msg(char const *level, char const *file, int line, const char *function, char const *msg) {
  if (mInWrite) { return; }

  char buffer[2048];
  if (file != nullptr) {
    sprintf(buffer, "%s: %s(%i) - %s: %s\n", level, file, line, function, msg);
  } else {
    sprintf(buffer, "%s: %s\n", level, msg);
  }

  tinystl::string formattedMessage(buffer);

  if (!Os::Thread::IsMainThread()) {
    logMutex.Acquire();
  }

  mLastMessage = msg;

  if (mRecordTimestamp) {
    formattedMessage = tinystl::string("[ ") + ::GetTimeStamp() + "] " + formattedMessage;
  }

  mInWrite = true;

  OutputDebugString(formattedMessage.c_str());

  if (logFile.IsOpen()) {
    logFile.Write(formattedMessage.c_str(), formattedMessage.size());
    logFile.Flush();
  }

  // TODO: Send Log Write Event

  mInWrite = false;
  if (!Os::Thread::IsMainThread()) {
    logMutex.Release();
  }
}

void LogManager::errorMsg(char const *file, int line, const char *function, char const *msg) {
  if (!pLogInstance) { return; }

  pLogInstance->msg("ERR", file, line, function, msg);
}

void LogManager::warningMsg(char const *file, int line, const char *function, char const *msg) {
  if (!pLogInstance) { return; }

  pLogInstance->msg("WARN", file, line, function, msg);
}

void LogManager::infoMsg(char const *file, int line, const char *function, char const *msg) {
  if (!pLogInstance) { return; }

  pLogInstance->msg("INFO", nullptr, 0, nullptr, msg);
}

void LogManager::debugMsg(char const *file, int line, const char *function, char const *msg) {
  if (!pLogInstance) { return; }

  pLogInstance->msg("DBG", file, line, function, msg);
}

void LogManager::failedAssert(char const *file, int line, char const *msg) {
  if (!pLogInstance) { return; }

  pLogInstance->msg("ASSERT", file, line, "", msg);
}

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

