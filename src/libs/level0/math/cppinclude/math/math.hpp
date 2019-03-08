// Apache Licensed (full license and info at end of file)
// Originally written by Confetti Interactive Inc.
#pragma once

#ifndef WYRD_MATH_MATH_HPP
#define WYRD_MATH_MATH_HPP

#include <cmath>
#include <cstdlib>
#include <limits>
#include <algorithm>

// ModifiedSonyMath ReadMe:
// - All you need to do is include the public header file vectormath.hpp. It will expose the relevant parts of
//   the library for you and try to select the SSE implementation if supported.
#include "../../src/vectormath.hpp"

namespace Math {

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

template<typename T, typename = typename std::enable_if<std::is_floating_point<T>{}>::type>
static constexpr auto pi() -> T { return T(3.14159265358979323846264338327950L); }

template<typename T, typename = typename std::enable_if<std::is_floating_point<T>{}>::type>
static constexpr auto pi_over_2() -> T { return pi<T>() / T(2); }
template<typename T, typename = typename std::enable_if<std::is_floating_point<T>{}>::type>
static constexpr auto two_pi() -> T { return T(2) * pi<T>(); }

template<typename T>
static constexpr auto square(T const val_) -> T { return val_ * val_; }

template<typename T, typename = typename std::enable_if<std::is_floating_point<T>{}>::type>
static constexpr auto degreesToRadians(T const val_) -> T {
  return val_ * (pi<T>() / T(180));
}

template<typename T, typename = typename std::enable_if<std::is_floating_point<T>{}>::type>
static constexpr auto radiansToDegrees(T const val_) -> T {
  return (T(180) * val_) / pi<T>();
}

template<typename T, typename = typename std::enable_if<std::is_floating_point<T>{}>::type>
constexpr auto ApproxEqual(T const a_, T const b_, T const eps_ = T(1e-5)) -> bool {
  return std::fabs(a_ - b_) < eps_;
}

template<typename T, typename = typename std::enable_if<std::is_floating_point<T>{}>::type>
constexpr T Reciprocal(T a) { return T(1.0) / a; }

template<typename T>
constexpr int Sign(T val) { return (T(0) < val) - (val < T(0)); }

//! Length^2 of a 1D Vector for orthogonality
template<typename T, typename = typename std::enable_if<std::is_floating_point<T>{}>::type>
constexpr T LengthSquared(T a) { return a * a; }

//! Length of a 1D Vector for orthogonality
template<typename T, typename = typename std::enable_if<std::is_floating_point<T>{}>::type>
constexpr T Length(T a) { return a; }

template<typename T, typename = typename std::enable_if<std::is_floating_point<T>{}>::type>
constexpr T ReciprocalSqrt(T a) { return T(1.0) / std::sqrt(a); }

template<typename genType>
constexpr genType Clamp(genType x, genType minVal, genType maxVal) {
  static_assert(std::numeric_limits<genType>::is_iec559 || std::numeric_limits<genType>::is_integer,
                "'clamp' only accept floating-point or integer inputs");
  return std::min(std::max(x, minVal), maxVal);
}

} // end Math namespace

#endif //WYRD_MATH_MATH_HPP
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
