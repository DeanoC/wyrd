#include "core/core.h"
#include "core/logger.h"
#include "math/math.h"

EXTERN_C uint8_t Math_LogTable256[] =
    {0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
     4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
     5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
     7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
     7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
     7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
     7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
     7, 7, 7, 7};

EXTERN_C uint32_t Math_SRGBTable[256] = {
    0x00000000, 0x399f22b4, 0x3a1f22b4, 0x3a6eb40e, 0x3a9f22b4, 0x3ac6eb61, 0x3aeeb40e, 0x3b0b3e5d,
    0x3b1f22b4, 0x3b33070b, 0x3b46eb61, 0x3b5b518d, 0x3b70f18d, 0x3b83e1c6, 0x3b8fe616, 0x3b9c87fd,
    0x3ba9c9b7, 0x3bb7ad6f, 0x3bc63549, 0x3bd56361, 0x3be539c1, 0x3bf5ba70, 0x3c0373b5, 0x3c0c6152,
    0x3c15a703, 0x3c1f45be, 0x3c293e6b, 0x3c3391f7, 0x3c3e4149, 0x3c494d43, 0x3c54b6c7, 0x3c607eb1,
    0x3c6ca5df, 0x3c792d22, 0x3c830aa8, 0x3c89af9f, 0x3c9085db, 0x3c978dc5, 0x3c9ec7c2, 0x3ca63433,
    0x3cadd37d, 0x3cb5a601, 0x3cbdac20, 0x3cc5e639, 0x3cce54ab, 0x3cd6f7d5, 0x3cdfd010, 0x3ce8ddb9,
    0x3cf2212c, 0x3cfb9ac1, 0x3d02a569, 0x3d0798dc, 0x3d0ca7e6, 0x3d11d2af, 0x3d171963, 0x3d1c7c2e,
    0x3d21fb3c, 0x3d2796b2, 0x3d2d4ebb, 0x3d332380, 0x3d39152b, 0x3d3f23e3, 0x3d454fd1, 0x3d4b991c,
    0x3d51ffef, 0x3d58846a, 0x3d5f26b7, 0x3d65e6fe, 0x3d6cc564, 0x3d73c20f, 0x3d7add29, 0x3d810b67,
    0x3d84b795, 0x3d887330, 0x3d8c3e4a, 0x3d9018f6, 0x3d940345, 0x3d97fd4a, 0x3d9c0716, 0x3da020bb,
    0x3da44a4b, 0x3da883d7, 0x3daccd70, 0x3db12728, 0x3db59112, 0x3dba0b3b, 0x3dbe95b5, 0x3dc33092,
    0x3dc7dbe2, 0x3dcc97b6, 0x3dd1641f, 0x3dd6412c, 0x3ddb2eef, 0x3de02d77, 0x3de53cd5, 0x3dea5d19,
    0x3def8e52, 0x3df4d091, 0x3dfa23e8, 0x3dff8861, 0x3e027f07, 0x3e054280, 0x3e080ea3, 0x3e0ae378,
    0x3e0dc105, 0x3e10a754, 0x3e13966b, 0x3e168e52, 0x3e198f10, 0x3e1c98ad, 0x3e1fab30, 0x3e22c6a3,
    0x3e25eb09, 0x3e29186c, 0x3e2c4ed0, 0x3e2f8e41, 0x3e32d6c4, 0x3e362861, 0x3e39831e, 0x3e3ce703,
    0x3e405416, 0x3e43ca5f, 0x3e4749e4, 0x3e4ad2ae, 0x3e4e64c2, 0x3e520027, 0x3e55a4e6, 0x3e595303,
    0x3e5d0a8b, 0x3e60cb7c, 0x3e6495e0, 0x3e6869bf, 0x3e6c4720, 0x3e702e0c, 0x3e741e84, 0x3e781890,
    0x3e7c1c38, 0x3e8014c2, 0x3e82203c, 0x3e84308d, 0x3e8645ba, 0x3e885fc5, 0x3e8a7eb2, 0x3e8ca283,
    0x3e8ecb3d, 0x3e90f8e1, 0x3e932b74, 0x3e9562f8, 0x3e979f71, 0x3e99e0e2, 0x3e9c274e, 0x3e9e72b7,
    0x3ea0c322, 0x3ea31892, 0x3ea57308, 0x3ea7d289, 0x3eaa3718, 0x3eaca0b7, 0x3eaf0f69, 0x3eb18333,
    0x3eb3fc18, 0x3eb67a18, 0x3eb8fd37, 0x3ebb8579, 0x3ebe12e1, 0x3ec0a571, 0x3ec33d2d, 0x3ec5da17,
    0x3ec87c33, 0x3ecb2383, 0x3ecdd00b, 0x3ed081cd, 0x3ed338cc, 0x3ed5f50b, 0x3ed8b68d, 0x3edb7d54,
    0x3ede4965, 0x3ee11ac1, 0x3ee3f16b, 0x3ee6cd67, 0x3ee9aeb6, 0x3eec955d, 0x3eef815d, 0x3ef272ba,
    0x3ef56976, 0x3ef86594, 0x3efb6717, 0x3efe6e02, 0x3f00bd2d, 0x3f02460e, 0x3f03d1a7, 0x3f055ff9,
    0x3f06f106, 0x3f0884cf, 0x3f0a1b56, 0x3f0bb49b, 0x3f0d50a0, 0x3f0eef67, 0x3f1090f1, 0x3f12353e,
    0x3f13dc51, 0x3f15862b, 0x3f1732cd, 0x3f18e239, 0x3f1a946f, 0x3f1c4971, 0x3f1e0141, 0x3f1fbbdf,
    0x3f21794e, 0x3f23398e, 0x3f24fca0, 0x3f26c286, 0x3f288b41, 0x3f2a56d3, 0x3f2c253d, 0x3f2df680,
    0x3f2fca9e, 0x3f31a197, 0x3f337b6c, 0x3f355820, 0x3f3737b3, 0x3f391a26, 0x3f3aff7c, 0x3f3ce7b5,
    0x3f3ed2d2, 0x3f40c0d4, 0x3f42b1be, 0x3f44a590, 0x3f469c4b, 0x3f4895f1, 0x3f4a9282, 0x3f4c9201,
    0x3f4e946e, 0x3f5099cb, 0x3f52a218, 0x3f54ad57, 0x3f56bb8a, 0x3f58ccb0, 0x3f5ae0cd, 0x3f5cf7e0,
    0x3f5f11ec, 0x3f612eee, 0x3f634eef, 0x3f6571e9, 0x3f6797e3, 0x3f69c0d6, 0x3f6beccd, 0x3f6e1bbf,
    0x3f704db8, 0x3f7282af, 0x3f74baae, 0x3f76f5ae, 0x3f7933b9, 0x3f7b74c6, 0x3f7db8e0, 0x3f800000
};

// From Chunk Walbourns code from DirectXTexConvert.cpp
// e5b9g9r9 are positive only shared exponent float formats
EXTERN_C uint32_t Math_Floats2E5b9g9r9(float const in_[3]) {

  float const maxf9 = (float) (0x1ffu << 7u);
  float const minf9 = float(1.f / (1u << 16u));

  float x = (in_[0] >= 0.f) ? ((in_[0] > maxf9) ? maxf9 : in_[0]) : 0.f;
  float y = (in_[1] >= 0.f) ? ((in_[1] > maxf9) ? maxf9 : in_[1]) : 0.f;
  float z = (in_[2] >= 0.f) ? ((in_[2] > maxf9) ? maxf9 : in_[2]) : 0.f;

  const float max_xy = (x > y) ? x : y;
  const float max_xyz = (max_xy > z) ? max_xy : z;
  const float maxColor = (max_xyz > minf9) ? max_xyz : minf9;

  union {
    float f;
    int32_t i;
  } fi;
  union {
    struct {
      uint32_t xm : 9;
      uint32_t ym : 9;
      uint32_t zm : 9;
      uint32_t e : 5;
    };
    uint32_t v;
  } ei;

  fi.f = maxColor;
  fi.i += 0x00004000; // round up leaving 9 bits in fraction (including assumed 1)

  // Fix applied from DirectXMath 3.10
  uint32_t exp = (uint32_t) fi.i >> 23u;
  ei.e = exp - 0x6f;

  fi.i = 0x83000000 - (exp << 23u);

  ei.xm = static_cast<uint32_t>(lroundf(x * fi.f));
  ei.ym = static_cast<uint32_t>(lroundf(y * fi.f));
  ei.zm = static_cast<uint32_t>(lroundf(z * fi.f));
  return ei.v;
}

EXTERN_C void Math_E5b9g9r92Floats(uint32_t const in_, float *r, float *g, float *b) {
  ASSERT(r);
  ASSERT(g);
  ASSERT(b);

  union {
    struct {
      uint32_t xm : 9;
      uint32_t ym : 9;
      uint32_t zm : 9;
      uint32_t e : 5;
    };
    uint32_t v;
  } ei;
  ei.v = in_;

  float exp = 0.0f;
  if (ei.e == 0) {
    // subnormal
    // for now flush to zero
    *r = 0.0f;
    *g = 0.0f;
    *b = 0.0f;
    return;

    /* TODO finish this
    // not sure if each channel can be denormalised seperately...
    int32_t e = 1;
    uint32_t mr = ei.xm;
    uint32_t mg = ei.ym;
    uint32_t mb = ei.zm;
    do {
        e--;
        mr <<= 1;
    } while((mr & 0x400) == 0);
    mr &= 0x400;
    */
  } else if (ei.e == 0x1F) {
    // infinity or nan
    if (ei.xm == 0 || ei.ym == 0 || ei.zm == 0) {
      *r = *g = *b = std::numeric_limits<float>::quiet_NaN();
      return;
    }
    *r = *g = *b = std::numeric_limits<float>::infinity();
    return;
  } else {
    exp = float(1u << ei.e);
  }

  *r = (1.0f + (float(ei.xm) / float(1 << 9))) * exp;
  *g = (1.0f + (float(ei.ym) / float(1 << 9))) * exp;
  *b = (1.0f + (float(ei.zm) / float(1 << 9))) * exp;
  return;
}


// often we want half conversion. so here are Rygorous's one. These may be
// hw accelerated in future on HW that has a CPU instruction
// the half to float and vice versa is from Rygorous publid domain code
// the float to half is his fast round to even
// he has SSE/SIMD versions if required
// Same, but rounding ties to nearest even instead of towards +inf
// Ryg's float_to_half_fast3_rtne

EXTERN_C uint16_t Math_Float2Half(float f_) {
  union FP32 {
    uint32_t u;
    float f;
    struct {
      uint32_t Mantissa : 23;
      uint32_t Exponent : 8;
      uint32_t Sign : 1;
    };
  };

  union FP16 {
    uint16_t u;
    struct {
      uint32_t Mantissa : 10;
      uint32_t Exponent : 5;
      uint32_t Sign : 1;
    };
  };

  FP32 f32infty = {255 << 23};
  FP32 f16max = {(127 + 16) << 23};
  FP32 denorm_magic = {((127 - 15) + (23 - 10) + 1) << 23};
  uint32_t sign_mask = 0x80000000u;
  FP16 o = {0};
  FP32 f;
  f.f = f_;

  uint32_t sign = f.u & sign_mask;
  f.u ^= sign;

  // NOTE all the integer compares in this function can be safely
  // compiled into signed compares since all operands are below
  // 0x80000000. Important if you want fast straight SSE2 code
  // (since there's no unsigned PCMPGTD).

  if (f.u >= f16max.u) { // result is Inf or NaN (all exponent bits set)
    o.u = (f.u > f32infty.u) ? 0x7e00 : 0x7c00; // NaN->qNaN and Inf->Inf
  } else // (De)normalized number or zero
  {
    if (f.u < (113 << 23)) // resulting FP16 is subnormal or zero
    {
      // use a magic value to align our 10 mantissa bits at the bottom of
      // the float. as long as FP addition is round-to-nearest-even this
      // just works.
      f.f += denorm_magic.f;

      // and one integer subtract of the bias later, we have our final float!
      o.u = f.u - denorm_magic.u;
    } else {
      uint32_t mant_odd = (f.u >> 13) & 1; // resulting mantissa is odd

      // update exponent, rounding bias part 1
      f.u += 0xc8000fff;
      // rounding bias part 2
      f.u += mant_odd;
      // take the bits!
      o.u = f.u >> 13;
    }
  }

  o.u |= sign >> 16;
  return o.u;
}

EXTERN_C float Math_Half2Float(uint16_t h_) {
  union FP32 {
    uint32_t u;
    float f;
    struct {
      uint32_t Mantissa : 23;
      uint32_t Exponent : 8;
      uint32_t Sign : 1;
    };
  };

  union FP16 {
    uint16_t u;
    struct {
      uint32_t Mantissa : 10;
      uint32_t Exponent : 5;
      uint32_t Sign : 1;
    };
  };

  static const FP32 magic = {113 << 23};
  static const uint32_t shifted_exp = 0x7c00 << 13; // exponent mask after shift
  FP16 h;
  h.u = h_;
  FP32 o;

  o.u = (h.u & 0x7fff) << 13;     // exponent/mantissa bits
  uint32_t exp = shifted_exp & o.u;   // just the exponent
  o.u += (127 - 15) << 23;        // exponent adjust

  // handle exponent special cases
  if (exp == shifted_exp) { // Inf/NaN?
    o.u += (128 - 16) << 23;    // extra exp adjust
  } else if (exp == 0) // Zero/Denormal?
  {
    o.u += 1 << 23;             // extra exp adjust
    o.f -= magic.f;             // renormalize
  }

  o.u |= (h.u & 0x8000) << 16;    // sign bit
  return o.f;
}
