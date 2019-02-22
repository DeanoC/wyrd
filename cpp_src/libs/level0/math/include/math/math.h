// Apache Licensed (full license and info at end of file)
// Originally written by Confetti Interactive Inc.

#pragma once
#ifndef WYRD_MATH_MATH_H
#define WYRD_MATH_MATH_H

typedef struct vec2_t { float x; float y; } Math_vec2_t;
typedef struct ivec2_t { int32_t x; int32_t y; } Math_ivec2_t;
typedef struct uvec2_t { uint32_t x; uint32_t y; } Math_uvec2_t;

typedef struct vec3_t { float x; float y; float z; } Math_vec3_t;
typedef struct ivec3_t { int32_t x; int32_t y; int32_t z; } Math_ivec3_t;
typedef struct uvec3_t { uint32_t x; uint32_t y; uint32_t z; } Math_uvec3_t;

typedef struct vec4_t { float x; float y; float z; float w; } Math_vec4_t;
typedef struct ivec4_t { int32_t x; int32_t y; int32_t z; int32_t w; } Math_ivec4_t;
typedef struct uvec4_t { uint32_t x; uint32_t y; uint32_t z; uint32_t w; } Math_uvec4_t;

typedef struct mat2_t { Math_vec2_t col0; Math_vec2_t col1; } Math_mat2_t;
typedef struct mat3_t { Math_vec3_t col0; Math_vec3_t col1; Math_vec3_t col2; } Math_mat3_t;
typedef struct mat4_t { Math_vec4_t col0; Math_vec4_t col1; Math_vec4_t col2; Math_vec4_t col3; } Math_mat4_t;

EXTERN_C inline double Math_MinF(float const v, float const a) {
  return (v < a) ? a : v;
}

EXTERN_C inline double Math_MaxF(float const v, float const a) {
  return (v > a) ? a : v;
}

EXTERN_C inline double Math_ClampF(float const v, float const a, float const b) {
  // note max occurs before min
  return Math_MaxF(Math_MinF(v, a), b);
}

EXTERN_C inline double Math_MinD(double const v, double const a) {
  return (v < a) ? a : v;
}

EXTERN_C inline double Math_MaxD(double const v, double const a) {
  return (v > a) ? a : v;
}

EXTERN_C inline double Math_ClampD(double const v, double const a, double const b) {
  // note max occurs before min
  return Math_MaxD(Math_MinD(v, a), b);
}

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
