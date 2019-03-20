#pragma once
#ifndef WYRD_IMAGE_BLOCK_H
#define WYRD_IMAGE_BLOCK_H
#include "core/core.h"
#include "image/format.h"

EXTERN_C void Image_BlockDecodeColor(uint8_t *dest,
                                     int blockWidth,
                                     int blockHeight,
                                     int pixelPitch,
                                     int rowPitch,
                                     enum Image_Format format,
                                     int red,
                                     int blue,
                                     uint8_t const *src);
EXTERN_C void Image_BlockDecodeExplicitAlpha(uint8_t *dest,
                                             int blockWidth,
                                             int blockHeight,
                                             int pixelPitch,
                                             int rowPitch,
                                             uint8_t const *src);

EXTERN_C void Image_BlockDecodeInterpolateAlpha(uint8_t *dest,
                                                int blockWidth,
                                                int blockHeight,
                                                int pixelPitch,
                                                int rowPitch,
                                                uint8_t const *src);

// these decode a 8/16 byte block in src to 4x4 pixels
// BC1 = 8 bytes in 48 bytes out (TODO alpha)
// BC2/BC3 = 16 bytes in 64 bytes out
// BC4 = 8 bytes in 16 bytes out
// BC5 = 16 bytes in 32 bytes out
EXTERN_C void Image_BlockDecodeBC1(uint8_t *dest, uint8_t const *src);
EXTERN_C void Image_BlockDecodeBC2(uint8_t *dest, uint8_t const *src);
EXTERN_C void Image_BlockDecodeBC3(uint8_t *dest, uint8_t const *src);
EXTERN_C void Image_BlockDecodeBC4(uint8_t *dest, uint8_t const *src);
EXTERN_C void Image_BlockDecodeBC5(uint8_t *dest, uint8_t const *src);

EXTERN_C void Image_BlockDecodeCompressedData(uint8_t *dest,
                                               uint8_t const *src,
                                               int width,
                                               int height,
                                               Image_Format format);

EXTERN_C bool Image_BlockDecodeIsSupported(Image_Format format);
#endif //WYRD_IMAGE_BLOCK_H
