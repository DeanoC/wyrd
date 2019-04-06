// Borrowed from Freetype-GL and seperated into its own file.
// Original license at bottom of file
#ifndef WYRD_IMAGE_HQ_RESCALAR_HPP
#define WYRD_IMAGE_HQ_RESCALAR_HPP
#include <cmath>
#include <cstring>
#include <algorithm>

namespace Image {

template<typename real = float>
real MitchellNetravali(const real x, const real _b = real(1) / real(3), const real _c = real(1) / real(3));

template<typename real = float>
real hq_interpolate(real x,
                    real y0,
                    real y1,
                    real y2,
                    real y3,
                    const real _b = real(1) / real(3),
                    const real _c = real(1) / real(3));

template<typename real = float>
void hq_resample(const unsigned int _channelCount,
                 const real *_srcData,
                 const unsigned int _srcWidth,
                 const unsigned int _srcHeight,
                 real *_dstData,
                 const unsigned int _dstWidth,
                 const unsigned int _dstHeight,
                 const real _b = real(1) / real(3),
                 const real _c = real(1) / real(3),
                 const real _clampLow = real(0),
                 const real _clampHigh = real(1));

// ------------------------------------------------------ MitchellNetravali ---
// Mitchell Netravali reconstruction filter
template<typename real>
real MitchellNetravali(real x, const real B, const real C) {
  // const float B = 1 / 3.0f, C = 1 / 3.0f; // Recommended
  // const float B =   1.0, C =   0.0; // Cubic B-spline (smoother results)
  // const float B =   0.0, C = 1/2.0; // Catmull-Rom spline (sharper results)
  x = std::abs(x);
  if (x < 1) {
    return ((12 - 9 * B - 6 * C) * x * x * x
        + (-18 + 12 * B + 6 * C) * x * x
        + (6 - 2 * B)) / 6;
  } else if (x < 2) {
    return ((-B - 6 * C) * x * x * x
        + (6 * B + 30 * C) * x * x
        + (-12 * B - 48 * C) * x
        + (8 * B + 24 * C)) / 6;
  } else {
    return 0;
  }
}

// ------------------------------------------------------------ interpolate ---
template<typename real>
real hq_interpolate(real x, real y0, real y1, real y2, real y3, real _b, real _c) {

  real c0 = MitchellNetravali<real>(x - 2, _b, _c);
  real c1 = MitchellNetravali<real>(x - 1, _b, _c);
  real c2 = MitchellNetravali<real>(x + 0, _b, _c);
  real c3 = MitchellNetravali<real>(x + 1, _b, _c);
  real r = c0 * y0 + c1 * y1 + c2 * y2 + c3 * y3;
  return r;
}

// ------------------------------------------------------------------ scale ---
template<typename real>
void hq_resample(const unsigned int _channelCount,
                 const real *_srcData, const unsigned int _srcWidth, const unsigned int _srcHeight,
                 real *_dstData, const unsigned int _dstWidth, const unsigned int _dstHeight,
                 const real _b, const real _c, const real _clampLow, const real _clampHigh) {
  using namespace std;

  if ((_srcWidth == _dstWidth) && (_srcHeight == _dstHeight)) {
    memcpy(_dstData, _srcData, _srcWidth * _srcHeight * sizeof(real) * _channelCount);
    return;
  }
  const real xscale = _srcWidth / (real) _dstWidth;
  const real yscale = _srcHeight / (real) _dstHeight;

  for (unsigned int j = 0; j < _dstHeight; ++j) {
    for (unsigned int i = 0; i < _dstWidth; ++i) {
      // genereate indices
      const real rSrcI = ((real) i) * xscale;
      const real rSrcJ = ((real) j) * yscale;
      const int srcI = (int) floor(rSrcI);
      const int srcJ = (int) floor(rSrcJ);
      int i0 = min(max(0, srcI - 2), (int) _srcWidth - 1);
      int i1 = min(max(0, srcI - 1), (int) _srcWidth - 1);
      int i2 = min(max(0, srcI + 0), (int) _srcWidth - 1);
      int i3 = min(max(0, srcI + 1), (int) _srcWidth - 1);
      int j0 = min(max(0, srcJ - 2), (int) _srcHeight - 1);
      int j1 = min(max(0, srcJ - 1), (int) _srcHeight - 1);
      int j2 = min(max(0, srcJ + 0), (int) _srcHeight - 1);
      int j3 = min(max(0, srcJ + 1), (int) _srcHeight - 1);

      for (unsigned int c = 0; c < _channelCount; ++c) {
        real t0 = hq_interpolate<real>((real) i / (real) _dstWidth,
                                       _srcData[(((j0 * _srcWidth) + i0) * _channelCount) + c],
                                       _srcData[(((j0 * _srcWidth) + i1) * _channelCount) + c],
                                       _srcData[(((j0 * _srcWidth) + i2) * _channelCount) + c],
                                       _srcData[(((j0 * _srcWidth) + i3) * _channelCount) + c],
                                       _b, _c);
        real t1 = hq_interpolate<real>((real) i / (real) _dstWidth,
                                       _srcData[(((j1 * _srcWidth) + i0) * _channelCount) + c],
                                       _srcData[(((j1 * _srcWidth) + i1) * _channelCount) + c],
                                       _srcData[(((j1 * _srcWidth) + i2) * _channelCount) + c],
                                       _srcData[(((j1 * _srcWidth) + i3) * _channelCount) + c],
                                       _b, _c);
        real t2 = hq_interpolate<real>((real) i / (real) _dstWidth,
                                       _srcData[(((j2 * _srcWidth) + i0) * _channelCount) + c],
                                       _srcData[(((j2 * _srcWidth) + i1) * _channelCount) + c],
                                       _srcData[(((j2 * _srcWidth) + i2) * _channelCount) + c],
                                       _srcData[(((j2 * _srcWidth) + i3) * _channelCount) + c],
                                       _b, _c);
        real t3 = hq_interpolate<real>((real) i / (real) _dstWidth,
                                       _srcData[(((j3 * _srcWidth) + i0) * _channelCount) + c],
                                       _srcData[(((j3 * _srcWidth) + i1) * _channelCount) + c],
                                       _srcData[(((j3 * _srcWidth) + i2) * _channelCount) + c],
                                       _srcData[(((j3 * _srcWidth) + i3) * _channelCount) + c],
                                       _b, _c);
        // this will pass unless _clampLow is a NAN
        if (_clampLow == _clampLow) {
          t0 = std::max(t0, _clampLow);
          t1 = std::max(t1, _clampLow);
          t2 = std::max(t2, _clampLow);
          t3 = std::max(t3, _clampLow);
        }
        // this will pass unless _clampHigh is a NAN
        if (_clampHigh == _clampHigh) {
          t0 = std::min(t0, _clampHigh);
          t1 = std::min(t1, _clampHigh);
          t2 = std::min(t2, _clampHigh);
          t3 = std::min(t3, _clampHigh);
        }

        real y = hq_interpolate<real>((real) j / (real) _dstHeight,
                                      t0, t1, t2, t3, _b, _c);
        if (_clampLow == _clampLow) {
          y = std::max(y, _clampLow);
        }
        if (_clampHigh == _clampHigh) {
          y = std::min(y, _clampHigh);
        }
        _dstData[(((j * _dstWidth) + i) * _channelCount) + c] = y;
      }
    }
  }
}
}

#endif //WYRD_IMAGE_HQ_RESCALAR_HPP
/* =========================================================================
* Freetype GL - A C OpenGL Freetype engine
* Platform:    Any
* WWW:         http://code.google.com/p/freetype-gl/
* -------------------------------------------------------------------------
* Copyright 2011 Nicolas P. Rougier. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*  1. Redistributions of source code must retain the above copyright notice,
*     this list of conditions and the following disclaimer.
*
*  2. Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in the
*     documentation and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY NICOLAS P. ROUGIER ''AS IS'' AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
* EVENT SHALL NICOLAS P. ROUGIER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
* THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* The views and conclusions contained in the software and documentation are
* those of the authors and should not be interpreted as representing official
* policies, either expressed or implied, of Nicolas P. Rougier.
* ========================================================================= */
