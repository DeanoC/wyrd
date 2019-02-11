// Apache Licensed (full license and info at end of file)
// Originally written by Confetti Interactive Inc.

#pragma once
#ifndef WYRD_MATH_MATH_H
#define WYRD_MATH_MATH_H

#ifndef __cplusplus

#include <stdint.h>

typedef struct vec2_t { float mX; float mY; } vec2;
typedef struct ivec2_t { int32_t mX; int32_t mY; } ivec2;
typedef struct uvec2_t { uint32_t mX; uint32_t mY; } uvec2;

typedef struct vec3_t { float mX; float mY; float mZ; } vec3;
typedef struct ivec3_t { int32_t mX; int32_t mY; int32_t mZ; } ivec3;
typedef struct uvec3_t { uint32_t mX; uint32_t mY; uint32_t mZ; } uvec3;

typedef struct vec4_t { float mX; float mY; float mZ; float mW; } vec4;
typedef struct ivec4_t { int32_t mX; int32_t mY; int32_t mZ; int32_t mW; } ivec4;
typedef struct uvec4_t { uint32_t mX; uint32_t mY; uint32_t mZ; uint32_t mW; } uvec4;

typedef struct mat2_t { vec2 mCol0; vec2 mCol1; } mat2;
typedef struct mat3_t { vec3 mCol0; vec3 mCol1; vec3 mCol2; } mat3;
typedef struct mat4_t { vec4 mCol0; vec4 mCol1; vec4 mCol2; vec4 mCol3; } mat4;

#else
// ModifiedSonyMath ReadMe:
// - All you need to do is include the public header file vectormath.hpp. It will expose the relevant parts of
//   the library for you and try to select the SSE implementation if supported.
#include "../../src/vectormath.hpp"
typedef Vector2 vec2;
typedef Vector3 vec3;
typedef Vector4 vec4;

typedef IVector2 ivec2;
typedef IVector3 ivec3;
typedef IVector4 ivec4;

typedef UVector2 uvec2;
typedef UVector3 uvec3;
typedef UVector4 uvec4;

typedef Matrix2 mat2;
typedef Matrix3 mat3;
typedef Matrix4 mat4;

#endif

#endif //WYRD_MATH_MATH_H

/*
 * Copyright (c) 2018-2019 Confetti Interactive Inc.
 * Modified by Deano Calver

 * This file is part of The-Forge
 * (see https://github.com/ConfettiFX/The-Forge).
 * and wyrd
 * (see https://github.com/DeanoC/wyrd).
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
