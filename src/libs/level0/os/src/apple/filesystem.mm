#import <Foundation/Foundation.h>
#include <AppKit/NSOpenPanel.h>

#include "core/core.h"
#include "core/logger.h"
#include "os/filesystem.hpp"
//#include "../Interfaces/IMemoryManager.h"

#define RESOURCE_DIR "Shaders/Metal"

EXTERN_C bool Os_IsAbsolutePath(char const *fileFullPath) {
  return (([NSString stringWithUTF8String:fileFullPath].absolutePath == YES) ? true : false);
}

EXTERN_C bool Os_FileCopy(char const *src, char const *dst) {
  NSError *error = nil;
  if (NO == [[NSFileManager defaultManager] copyItemAtPath:[NSString stringWithUTF8String:src]
                                                    toPath:[NSString stringWithUTF8String:dst]
                                                     error:&error]) {
    LOGINFOF("Failed to copy file with error : %s", [[error localizedDescription] UTF8String]);
    return false;
  }

  return true;
}

EXTERN_C bool Os_GetExePath(char *dirOut, int maxSize) {
  const char *exePath =
      [[[[NSBundle mainBundle] bundlePath] stringByStandardizingPath] cStringUsingEncoding:NSUTF8StringEncoding];
  if (exePath == NULL) { return false; }
  if (strlen(exePath) >= maxSize) { return false; }

  strncpy(dirOut, exePath, maxSize);
  return true;
}

bool Os_GetUserDocumentsDir(char *dirOut, int maxSize) {
  const char *rawUserPath = [[[[NSFileManager defaultManager] homeDirectoryForCurrentUser] absoluteString] UTF8String];
  if (rawUserPath == NULL) { return false; }

  const char *path;
  path = strstr(rawUserPath, "/Users/");
  if (strlen(path) >= maxSize) { return false; }
  strcpy(dirOut, path);
  return true;
}

EXTERN_C bool Os_GetAppPrefsDir(char const *org, char const *app, char *dirOut, int maxSize) {
  const char *rawUserPath = [[[[NSFileManager defaultManager] homeDirectoryForCurrentUser] absoluteString] UTF8String];
  if (rawUserPath == NULL) { return false; }
  const char *path;
  path = strstr(rawUserPath, "/Users/");

  tinystl::string out = tinystl::string(path) +
      tinystl::string("Library/") +
      tinystl::string(org) +
      tinystl::string("/") +
      tinystl::string(app);

  if (out.size() >= maxSize) { return false; }

  strcpy(dirOut, out.c_str());
  return true;
}

namespace FileSystem {
/*
void get_files_with_extension(const char *dir, const char *ext, tinystl::vector<tinystl::string>& filesOut) {
  tinystl::string path = FileSystem::GetPlatformPath(dir);
  DIR *pDir = opendir(dir);
  if (!pDir) {
    LOGWARNINGF("Could not open directory: %s", dir);
    return;
  }

  // recursively search the directory for files with given extension
  dirent *entry = NULL;
  while ((entry = readdir(pDir)) != NULL) {
    if (FileSystem::GetExtension(entry->d_name) == ext) {
      filesOut.push_back(path + entry->d_name);
    }
  }

  closedir(pDir);
}

const char *pszRoots[] = {
    RESOURCE_DIR "/Binary/",    // FSR_BinShaders
    RESOURCE_DIR "/",           // FSR_SrcShaders
    "Textures/",                // FSR_Textures
    "Meshes/",                  // FSR_Meshes
    "Fonts/",                   // FSR_Builtin_Fonts
    "GPUCfg/",                  // FSR_GpuConfig
    "Animation/",               // FSR_Animation
    "",                         // FSR_OtherFiles
};

void get_sub_directories(const char *dir, tinystl::vector<tinystl::string>& subDirectoriesOut) {
  tinystl::string path = FileSystem::GetPlatformPath(dir);
  DIR *pDir = opendir(dir);
  if (!pDir) {
    LOGWARNINGF("Could not open directory: %s", dir);
    return;
  }

  // recursively search the directory for files with given extension
  dirent *entry = NULL;
  while ((entry = readdir(pDir)) != NULL) {
    if (entry->d_type & DT_DIR) {
      if (entry->d_name[0] != '.') {
        tinystl::string subDirectory = path + entry->d_name;
        subDirectoriesOut.push_back(subDirectory);
      }
    }
  }

  closedir(pDir);
}

static void FormatFileExtensionsFilter(
    tinystl::string const& fileDesc, tinystl::vector<tinystl::string> const& extFiltersIn,
    tinystl::string& extFiltersOut) {
  extFiltersOut = "";
  for (size_t i = 0; i < extFiltersIn.size(); ++i) {
    extFiltersOut += extFiltersIn[i];
    if (i != extFiltersIn.size() - 1)
      extFiltersOut += ";";
  }
}

void open_file_dialog(
    const char *title, const char *dir, FileDialogCallbackFn callback, void *userData, const char *fileDesc,
    const tinystl::vector<tinystl::string>& fileExtensions) {
  tinystl::string extFilter;
  FormatFileExtensionsFilter(fileDesc, fileExtensions, extFilter);

  // Create array of filtered extentions
  NSString *extString = [NSString stringWithCString:extFilter.c_str() encoding:[NSString defaultCStringEncoding]];
  NSArray *extList = [extString componentsSeparatedByString:@";"];

  NSString *objcString = [NSString stringWithCString:title encoding:[NSString defaultCStringEncoding]];
  NSString *objcURL = [NSString stringWithCString:dir encoding:[NSString defaultCStringEncoding]];
  NSURL *nsURL = [NSURL URLWithString:objcURL];

  // Create the File Open Dialog class.
  NSOpenPanel *openDlg = [NSOpenPanel openPanel];

  // Enable the selection of files in the dialog.
  [openDlg setCanChooseFiles:YES];

  // Multiple files not allowed
  [openDlg setAllowsMultipleSelection:NO];

  // Can't select a directory
  [openDlg setCanChooseDirectories:NO];
  [openDlg setMessage:objcString];
  [openDlg setDirectoryURL:nsURL];

  // Extention filtering
  [openDlg setAllowedFileTypes:extList];

  [openDlg beginSheetModalForWindow:[[NSApplication sharedApplication].windows objectAtIndex:0]
                  completionHandler:^(NSInteger result) {
                    if (result == NSModalResponseOK) {
                      NSArray *urls = [openDlg URLs];
                      NSString *url = [urls objectAtIndex:0];
                      callback(url.fileSystemRepresentation, userData);
                    }
                  }];
}

void save_file_dialog(
    const char *title, const char *dir, FileDialogCallbackFn callback, void *userData, const char *fileDesc,
    const tinystl::vector<tinystl::string>& fileExtensions) {
  tinystl::string extFilter;
  FormatFileExtensionsFilter(fileDesc, fileExtensions, extFilter);

  // Create array of filtered extentions
  NSString *extString = [NSString stringWithCString:extFilter.c_str() encoding:[NSString defaultCStringEncoding]];
  NSArray *extList = [extString componentsSeparatedByString:@";"];

  NSString *objcString = [NSString stringWithCString:title encoding:[NSString defaultCStringEncoding]];
  NSString *objcURL = [NSString stringWithCString:dir encoding:[NSString defaultCStringEncoding]];
  NSURL *nsURL = [NSURL URLWithString:objcURL];

  // Create the File Open Dialog class.
  NSSavePanel *saveDlg = [NSSavePanel savePanel];

  // Can't select a directory
  [saveDlg setMessage:objcString];
  [saveDlg setDirectoryURL:nsURL];

  // Extention filtering
  [saveDlg setAllowedFileTypes:extList];

  [saveDlg beginSheetModalForWindow:[[NSApplication sharedApplication].windows objectAtIndex:0]
                  completionHandler:^(NSInteger result) {
                    if (result == NSModalResponseOK) {
                      NSURL *url = [saveDlg URL];
                      callback(url.fileSystemRepresentation, userData);
                    }
                  }];
}*/
} // end namespace FileSystem

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

