// Apache Licensed (full license and info at end of file)
// Originally written by Confetti Interactive Inc.

#pragma once
#ifndef WYRD_MATH_MATH_H
#define WYRD_MATH_MATH_H

#include "core/core.h"
#include "core/logger.h"
#include <math.h>
#include <string.h>

typedef struct vec2_t { float x; float y; } vec2_t;
typedef struct ivec2_t { int32_t x; int32_t y; } ivec2_t;
typedef struct uvec2_t { uint32_t x; uint32_t y; } uvec2_t;

typedef struct vec3_t { float x; float y; float z; } vec3_t;
typedef struct ivec3_t { int32_t x; int32_t y; int32_t z; } ivec3_t;
typedef struct uvec3_t { uint32_t x; uint32_t y; uint32_t z; } uvec3_t;

typedef struct vec4_t { float x; float y; float z; float w; } vec4_t;
typedef struct ivec4_t { int32_t x; int32_t y; int32_t z; int32_t w; } ivec4_t;
typedef struct uvec4_t { uint32_t x; uint32_t y; uint32_t z; uint32_t w; } uvec4_t;

typedef struct mat2_t { vec2_t col0; vec2_t col1; } mat2_t;
typedef struct mat3_t { vec3_t col0; vec3_t col1; vec3_t col2; } mat3_t;
typedef struct mat4_t { vec4_t col0; vec4_t col1; vec4_t col2; vec4_t col3; } mat4_t;

#define MATH_FUNC_MACRO_CREATE(postfix, type)\
EXTERN_C inline type Math_Min##postfix(type const v, type const a) { return (v < a) ? v : a; } \
EXTERN_C inline type Math_Max##postfix(type const v, type const a) { return (v > a) ? v : a; } \
EXTERN_C inline type Math_Clamp##postfix(type const v, type const a, type const b) { return Math_Min##postfix(Math_Max##postfix(v, a), b); }

MATH_FUNC_MACRO_CREATE(F, float)
MATH_FUNC_MACRO_CREATE(D, double)
MATH_FUNC_MACRO_CREATE(I32, int32_t)
MATH_FUNC_MACRO_CREATE(U32, uint32_t)
MATH_FUNC_MACRO_CREATE(I64, int64_t)
MATH_FUNC_MACRO_CREATE(U64, uint64_t)

#undef MATH_FUNC_MACRO_CREATE

EXTERN_C inline float Math_SaturateF(const float x) { return Math_ClampF(x, 0.0f, 1.0f); }
EXTERN_C inline double Math_SaturateD(const double x) { return Math_ClampD(x, 0.0, 1.0); }

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

// Note: returns true for 0
EXTERN_C inline bool Math_IsPowerOf2U32(const uint32_t x) {
  return (x & (x - 1)) == 0;
}

// Note: returns true for 0
EXTERN_C inline bool Math_IsPowerOf2U64(const uint64_t x) {
  return (x & (x - 1)) == 0;
}

EXTERN_C inline uint32_t Math_UpperPowerOfTwoU32(uint32_t x) {
  if(x == 0) return 1;

  x -= 1;

  x |= x >> 16;
  x |= x >> 8;
  x |= x >> 4;
  x |= x >> 2;
  x |= x >> 1;

  return x + 1;
}

EXTERN_C inline uint64_t Math_UpperPowerOfTwoU64(uint64_t x) {
  if(x == 0) return 1;

  x -= 1;

  x |= x >> 32;
  x |= x >> 16;
  x |= x >> 8;
  x |= x >> 4;
  x |= x >> 2;
  x |= x >> 1;

  return x + 1;
}
EXTERN_C inline uint32_t Math_GetClosestPowerOfTwoU32(const uint32_t x) {
  uint32_t upow2 = Math_UpperPowerOfTwoU32(x);
  if (4 * x < 3 * upow2) return upow2 >> 1;
  else return upow2;
}

EXTERN_C inline uint64_t Math_GetClosestPowerOfTwoU64(const uint64_t x) {
  uint64_t upow2 = Math_UpperPowerOfTwoU64(x);
  if (4 * x < 3 * upow2) return upow2 >> 1;
  else return upow2;
}


EXTERN_C uint8_t Math_LogTable256[256];

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

EXTERN_C inline uint32_t Math_PackColorU32(uint32_t r, uint32_t g, uint32_t b, uint32_t a)
{
  return
      ((r & 0xff) << 24) |
          ((g & 0xff) << 16) |
          ((b & 0xff) << 8) |
          ((a & 0xff) << 0);
}

//Output format is R8G8B8A8
EXTERN_C inline uint32_t Math_PackColorF32(float r, float g, float b, float a)
{
  return Math_PackColorU32(
      (uint32_t)(Math_ClampF(r, 0.0f, 1.0f) * 255),
      (uint32_t)(Math_ClampF(g, 0.0f, 1.0f) * 255),
      (uint32_t)(Math_ClampF(b, 0.0f, 1.0f) * 255),
      (uint32_t)(Math_ClampF(a, 0.0f, 1.0f) * 255));
}
EXTERN_C inline uint32_t Math_PackColorVec44(struct vec4_t rgba) {
  return Math_PackColorF32(rgba.x, rgba.y, rgba.z, rgba.w);
}
EXTERN_C inline vec4_t Math_UnpackColorU32(uint32_t colorValue)
{
  vec4_t r = {
      (float) ((colorValue & 0xFF000000) >> 24) / 255.0f,
      (float) ((colorValue & 0x00FF0000) >> 16) / 255.0f,
      (float) ((colorValue & 0x0000FF00) >> 8) / 255.0f,
      (float) ((colorValue & 0x000000FF)) / 255.0f
  };
  return r;
}

EXTERN_C inline vec3_t Math_RGBEToRGB(unsigned char *rgbe)
{
  if (rgbe[3])
  {
    float const e = ldexpf(1.0f, rgbe[3] - (int)(128 + 8));
    vec3_t r = {
        ((float)rgbe[0]) * e,
        ((float)rgbe[1]) * e,
        ((float)rgbe[2]) * e,
    };
    return r;
  }
  vec3_t r = {0, 0, 0};
  return r;
}
EXTERN_C inline uint32_t Math_FloatRGBToRGBE8(const float r, const float g, const float b)
{
  float const v = Math_MaxF(Math_MaxF(r,g), b);

  if (v < 1e-32f) {
    return 0;
  }
  else {
    int ex;
    float m = frexpf(v, &ex) * 256.0f / v;

    uint32_t ir = (uint32_t)(m * r);
    uint32_t ig = (uint32_t)(m * g);
    uint32_t ib = (uint32_t)(m * b);
    uint32_t ie = (uint32_t)(ex + 128);

    return ir | (ig << 8) | (ib << 16) | (ie << 24);
  }
}

EXTERN_C inline uint32_t Math_Vec3RGBToRGBE8(const vec3_t rgb)
{
  return Math_FloatRGBToRGBE8(rgb.x, rgb.y, rgb.z);
}

EXTERN_C inline uint32_t Math_FloatRGBToRGB9E5(const float r, const float g, const float b)
{
  float const v = Math_MaxF(Math_MaxF(r,g), b);

  if (v < 1.52587890625e-5f) {
    return 0;
  }
  else if (v < 65536) {
    int ex;
    float m = frexpf(v, &ex) * 512.0f / v;

    uint32_t ir = (uint32_t)(m * r);
    uint32_t ig = (uint32_t)(m * g);
    uint32_t ib = (uint32_t)(m * b);
    uint32_t ie = (unsigned int)(ex + 15);

    return ir | (ig << 9) | (ib << 18) | (ie << 27);
  }
  else {
    uint32_t ir = (r < 65536) ? (uint32_t)(r * (1.0f / 128.0f)) : 0x1FF;
    uint32_t ig = (g < 65536) ? (uint32_t)(g * (1.0f / 128.0f)) : 0x1FF;
    uint32_t ib = (b < 65536) ? (uint32_t)(b * (1.0f / 128.0f)) : 0x1FF;
    uint32_t ie = 31;

    return ir | (ig << 9) | (ib << 18) | (ie << 27);
  }
}
EXTERN_C inline uint32_t MathVec3RGBToRGB9E5(const vec3_t rgb) {
  return Math_FloatRGBToRGB9E5(rgb.x, rgb.y, rgb.z);
}

EXTERN_C inline uint32_t round_up(uint32_t value, uint32_t multiple) {
  return ((value + multiple - 1) / multiple) * multiple;
}

EXTERN_C inline uint64_t round_up_64(uint64_t value, uint64_t multiple) {
  return ((value + multiple - 1) / multiple) * multiple;
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
