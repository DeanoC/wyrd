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

#include "core/core.h"
#include "core/logger.h"
#import "appdelegate.h"

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification*)aNotification
{
  // Insert code here to initialize your application
  LOGINFO("applicationDidFinishLaunching");
}

- (void)applicationWillTerminate:(NSNotification*)aNotification
{
  // Insert code here to tear down your application
  LOGINFO("applicationWillTerminate");
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender
{
  return YES;
}
- (void)applicationWillHide:(NSNotification *)notification
{
  LOGINFO("applicationWillHide");

}

@end
