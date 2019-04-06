#include "core/core.h"
#include "image/block.h"
#include "image/format_cracker.h"

EXTERN_C void Image_BlockDecodeColor(
    uint8_t *dest,
    int blockWidth, int blockHeight,
    int pixelPitch, int rowPitch, enum Image_Format format, int red, int blue, uint8_t const *src) {
  uint8_t colors[4][3];

  uint16_t c0 = *(uint16_t *) src;
  uint16_t c1 = *(uint16_t *) (src + 2);

  colors[0][0] = ((c0 >> 11) & 0x1F) << 3;
  colors[0][1] = ((c0 >> 5) & 0x3F) << 2;
  colors[0][2] = (c0 & 0x1F) << 3;

  colors[1][0] = ((c1 >> 11) & 0x1F) << 3;
  colors[1][1] = ((c1 >> 5) & 0x3F) << 2;
  colors[1][2] = (c1 & 0x1F) << 3;

  if (c0 > c1 ||
      ((format == Image_Format_BC3_SRGB_BLOCK) ||
          (format == Image_Format_BC3_UNORM_BLOCK))) {
    for (int i = 0; i < 3; i++) {
      colors[2][i] = (2 * colors[0][i] + colors[1][i] + 1) / 3;
      colors[3][i] = (colors[0][i] + 2 * colors[1][i] + 1) / 3;
    }
  } else {
    for (int i = 0; i < 3; i++) {
      colors[2][i] = (colors[0][i] + colors[1][i] + 1) >> 1;
      colors[3][i] = 0;
    }
  }

  src += 4;
  for (int y = 0; y < blockHeight; y++) {
    uint8_t *dst = dest + rowPitch * y;
    unsigned int indexes = src[y];
    for (int x = 0; x < blockWidth; x++) {
      unsigned int index = indexes & 0x3;
      dst[red] = colors[index][0];
      dst[1] = colors[index][1];
      dst[blue] = colors[index][2];
      indexes >>= 2;

      dst += pixelPitch;
    }
  }
}

EXTERN_C void Image_BlockDecodeExplicitAlpha(uint8_t *dest,
    int blockWidth, int blockHeight,
    int pixelPitch, int rowPitch,
    uint8_t const *src) {
  for (int y = 0; y < blockHeight; y++) {
    uint8_t *dst = dest + rowPitch * y;
    unsigned int alpha = ((uint16_t *) src)[y];
    for (int x = 0; x < blockWidth; x++) {
      *dst = (alpha & 0xF) * 17;
      alpha >>= 4;
      dst += pixelPitch;
    }
  }
}

EXTERN_C void Image_BlockDecodeInterpolateAlpha(uint8_t *dest,
    int blockWidth, int blockHeight,
    int pixelPitch, int rowPitch,
    uint8_t const *src) {
  uint8_t a0 = src[0];
  uint8_t a1 = src[1];
  uint64_t alpha = (*(uint64_t *) src) >> 16;

  for (int y = 0; y < blockHeight; y++) {
    uint8_t *dst = dest + rowPitch * y;
    for (int x = 0; x < blockWidth; x++) {
      int k = ((unsigned int) alpha) & 0x7;
      if (k == 0) {
        *dst = a0;
      } else if (k == 1) {
        *dst = a1;
      } else if (a0 > a1) {
        *dst = (uint8_t) (((8 - k) * a0 + (k - 1) * a1) / 7);
      } else if (k >= 6) {
        *dst = (k == 6) ? 0 : 255;
      } else {
        *dst = (uint8_t) (((6 - k) * a0 + (k - 1) * a1) / 5);
      }
      alpha >>= 3;

      dst += pixelPitch;
    }
    if (blockWidth < 4) {
      alpha >>= (3 * (4 - blockWidth));
    }
  }
}

EXTERN_C void Image_BlockDecodeBC1(uint8_t *dest, uint8_t const *src) {
  Image_BlockDecodeColor(dest, 4, 4, 3, 3 * 4, Image_Format_BC1_RGB_UNORM_BLOCK, 0, 2, src);
}

EXTERN_C void Image_BlockDecodeBC2(uint8_t *dest, uint8_t const *src) {
  Image_BlockDecodeColor(dest, 4, 4, 4, 4 * 4, Image_Format_BC2_UNORM_BLOCK, 0, 2, src);
  Image_BlockDecodeExplicitAlpha(dest + 3, 4, 4, 4, 4 * 4, src + 8);
}

EXTERN_C void Image_BlockDecodeBC3(uint8_t *dest, uint8_t const *src) {
  Image_BlockDecodeColor(dest, 4, 4, 4, 4 * 4, Image_Format_BC3_UNORM_BLOCK, 0, 2, src);
  Image_BlockDecodeInterpolateAlpha(dest + 3, 4, 4, 4, 4 * 4, src + 8);
}

EXTERN_C void Image_BlockDecodeBC4(uint8_t *dest, uint8_t const *src) {
  Image_BlockDecodeInterpolateAlpha(dest, 4, 4, 1, 1 * 4, src);
}

EXTERN_C void Image_BlockDecodeBC5(uint8_t *dest, uint8_t const *src) {
  Image_BlockDecodeInterpolateAlpha(dest, 4, 4, 2, 2 * 4, src + 8);
  Image_BlockDecodeInterpolateAlpha(dest + 1, 4, 4, 2, 2 * 4, src);
}

EXTERN_C void Image_BlockDecodeCompressedData(uint8_t *dest,
                                               uint8_t const *src,
                                               const int width,
                                               const int height,
                                               const enum Image_Format format) {
  ASSERT(Image_Format_IsCompressed(format));

  int blockWidth = (width < 4) ? width : 4;
  int blockHeight = (height < 4) ? height : 4;

  int nChannels = Image_Format_ChannelCount(format);

  for (int y = 0; y < height; y += 4) {
    for (int x = 0; x < width; x += 4) {
      unsigned char *dst = dest + (y * width + x) * nChannels;
      switch (format) {
        case Image_Format_BC1_RGB_SRGB_BLOCK:
        case Image_Format_BC1_RGB_UNORM_BLOCK:
        case Image_Format_BC1_RGBA_SRGB_BLOCK:
        case Image_Format_BC1_RGBA_UNORM_BLOCK: {
          Image_BlockDecodeColor(dst, blockWidth, blockHeight, nChannels, width * nChannels, format, 0, 2, src);
          src += 8;
          break;
        }
        case Image_Format_BC2_SRGB_BLOCK:
        case Image_Format_BC2_UNORM_BLOCK: {
          Image_BlockDecodeColor(dst, blockWidth, blockHeight, nChannels, width * nChannels, format, 0, 2, src);
          Image_BlockDecodeExplicitAlpha(dst + 3, blockWidth, blockHeight, nChannels, width * nChannels, src + 8);
          src += 16;
          break;
        }
        case Image_Format_BC3_SRGB_BLOCK:
        case Image_Format_BC3_UNORM_BLOCK: {
          Image_BlockDecodeColor(dst, blockWidth, blockHeight, nChannels, width * nChannels, format, 0, 2, src);
          Image_BlockDecodeInterpolateAlpha(dst + 3, blockWidth, blockHeight, nChannels, width * nChannels, src + 8);
          src += 16;
          break;
        }
        case Image_Format_BC4_SNORM_BLOCK:
        case Image_Format_BC4_UNORM_BLOCK: {
          Image_BlockDecodeInterpolateAlpha(dst, blockWidth, blockHeight, 1, width, src);
          src += 8;
          break;
        }
        case Image_Format_BC5_SNORM_BLOCK:
        case Image_Format_BC5_UNORM_BLOCK: {
          Image_BlockDecodeInterpolateAlpha(dst, blockWidth, blockHeight, 2, width * 2, src + 8);
          Image_BlockDecodeInterpolateAlpha(dst + 1, blockWidth, blockHeight, 2, width * 2, src);
          src += 16;
          break;
        }
        default:ASSERT(false);
          return;
      }
    }
  }
}
EXTERN_C bool Image_BlockDecodeIsSupported(Image_Format format) {
  switch (format) {
    case Image_Format_BC1_RGB_SRGB_BLOCK:
    case Image_Format_BC1_RGB_UNORM_BLOCK:
    case Image_Format_BC1_RGBA_SRGB_BLOCK:
    case Image_Format_BC1_RGBA_UNORM_BLOCK:
    case Image_Format_BC2_SRGB_BLOCK:
    case Image_Format_BC2_UNORM_BLOCK:
    case Image_Format_BC3_SRGB_BLOCK:
    case Image_Format_BC3_UNORM_BLOCK:
    case Image_Format_BC4_SNORM_BLOCK:
    case Image_Format_BC4_UNORM_BLOCK:
    case Image_Format_BC5_SNORM_BLOCK:
    case Image_Format_BC5_UNORM_BLOCK:
      return true;
    default: return false;
  }

}


