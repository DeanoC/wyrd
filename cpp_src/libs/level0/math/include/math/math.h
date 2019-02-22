// Apache Licensed (full license and info at end of file)
// Originally written by Confetti Interactive Inc.

#pragma once
#ifndef WYRD_MATH_MATH_H
#define WYRD_MATH_MATH_H

#include "core/core.h"
#include "core/logger.h"
#include <math.h>

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

EXTERN_C inline float Math_MinF(float const v, float const a) {
  return (v < a) ? a : v;
}

EXTERN_C inline float Math_MaxF(float const v, float const a) {
  return (v > a) ? a : v;
}

EXTERN_C inline float Math_ClampF(float const v, float const a, float const b) {
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

EXTERN_C inline double Math_PiD() { return (double) (3.14159265358979323846264338327950L); }
EXTERN_C inline double Math_PiOver2D() { return Math_PiD() / 2.0; }
EXTERN_C inline double Math_TwoPiD() { return 2.0 * Math_PiD(); }
EXTERN_C inline double Math_SqrD(double const val) { return val * val; }
EXTERN_C inline double Math_DegreesToRadiansD(double const val) {
  return val * (Math_PiD() / 180.0);
}
EXTERN_C inline double Math_RadiansToDegreesD(double const val) {
  return (180.0 * val) / Math_PiD();
}
EXTERN_C inline double Math_ReciprocalD(double const a) { return 1.0 / a; }
EXTERN_C inline int Math_SignD(double val) { return (0.0 < val) - (val < 0.0); }

//! Length^2 of a 1D Vector for orthogonality
EXTERN_C inline double Math_LengthSquaredD(double const a) { return a * a; }

//! Length of a 1D Vector for orthogonality
EXTERN_C inline double Math_Length(double const a) { return a; }

EXTERN_C inline double Math_ReciprocalSqrtD(double const a) { return 1.0 / sqrt(a); }

EXTERN_C uint8_t Math_LogTable256[];


/// \brief	return Log2 of v.
/// return log2 of an int. this is equivalent to finding the highest bit that has been set
/// or the number to shift 1 left by to get the nearest lower power of 2
/// \param	v	The number to get the log2 of.
/// \return	log2(v).
EXTERN_C inline unsigned int Math_Log2(unsigned int v) {
  unsigned int r = 0;     // r will be lg(v)
  unsigned int t = 0, tt = 0; // temporaries

  if ((tt = v >> 16) != 0) {
    r = ((t = tt >> 8) != 0) ? 24 + ((unsigned int) Math_LogTable256[t]) : 16 + ((unsigned int) Math_LogTable256[tt]);
  } else {
    r = ((t = v >> 8) != 0) ? 8 + ((unsigned int) Math_LogTable256[t]) : ((unsigned int) Math_LogTable256[v]);
  }
  return r;
}

// From Chunk Walbourns code from DirectXTexConvert.cpp
// e5b9g9r9 are positive only shared exponent float formats
EXTERN_C uint32_t Math_Floats2E5b9g9r9(float const in_[3]);
EXTERN_C void Math_E5b9g9r92Floats(uint32_t const in_, float *r, float *g, float *b);

// the half to float and vice versa is from Rygorous publid domain code
EXTERN_C uint16_t Math_Float2Half(float f_);
EXTERN_C float Math_Half2Float(uint16_t h_);

// from D3DX_DXGIFormatConvert.inl
EXTERN_C inline float Math_Float2SRGB(float val) {
  if (val < 0.0031308f) {
    val *= 12.92f;
  } else {
    val = 1.055f * powf(val, 1.0f / 2.4f) - 0.055f;
  }

  return val;
}

EXTERN_C inline float Math_SRGB2FloatInexact(float val) {
  if (val < 0.04045f) {
    val /= 12.92f;
  } else {
    val = powf((val + 0.055f) / 1.055f, 2.4f);
  }
  return val;
}

EXTERN_C uint32_t Math_SRGBTable[256];

EXTERN_C inline float Math_SRGB2Float(uint32_t val) {
  ASSERT(val <= 255);

  float f;
  memcpy(&f, &Math_SRGBTable[val], sizeof(float));
  return f;
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
