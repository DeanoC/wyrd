#include "core/core.h"
#include "image/format.h"
#include "image/format_cracker.h"
#include "image/image.h"

// we include fetch after swizzle so hopefully the compiler will inline it...
EXTERN_C inline enum Image_Channel_t Image_Channel_Swizzle(enum Image_Format_t format, enum Image_Channel_t channel) {
  Image_Swizzle_t swizzler = Image_Format_Swizzle(format);
  return (enum Image_Channel_t) swizzler[channel];
}
#include "image_fetch.hpp"

EXTERN_C Image_Header_t *Image_Create2D(uint32_t width, uint32_t height, enum Image_Format_t format) {
  Image_Header_t *image = Image_Create2DNoClear(width, height, format);
  if (image) {
    memset(image + 1, 0, image->dataSize);
  }

  return image;
}

EXTERN_C Image_Header_t *Image_Create2DNoClear(uint32_t width, uint32_t height, enum Image_Format_t format) {
  // block compression can't be less than 4x4
  if ((width < 4 || height < 4) && Image_Format_IsCompressed(format)) {
    return NULL;
  }

  uint64_t const dataSize = (width * height * Image_Format_BitWidth(format)) / 8;

  Image_Header_t *image = (Image_Header_t *) malloc(sizeof(Image_Header_t) + dataSize);
  image->dataSize = dataSize;
  image->width = width;
  image->height = height;
  image->depth = 1;
  image->slices = 1;
  image->format = format;

  return image;

}

EXTERN_C void Image_Destroy(Image_Header_t *image) {
  free(image);
}

EXTERN_C size_t Image_CalculateIndex(Image_Header_t const *image, uint32_t x, uint32_t y, uint32_t z, uint32_t slice) {
  ASSERT(image);

  ASSERT(x < image->width);
  ASSERT(y < image->height);
  ASSERT(z < image->depth);
  ASSERT(slice < image->slices);

  size_t const size1D = image->width;
  size_t const size2D = image->width * image->height;
  size_t const size3D = image->width * image->height * image->depth;
  size_t const index = (slice * size3D) + (z * size2D) + (y * size1D) + x;

  return index;
}

EXTERN_C double Image_GetChannelAt(Image_Header_t const *image, enum Image_Channel_t channel, size_t index) {
  ASSERT(image);

  using namespace Image;
  // seperate out the block compressed format first
  if (Image_Format_IsCompressed(image->format)) {
    return Image::CompressedChannelAt(image, channel, index);
  }

  // split into bit width grouped formats
  ASSERT(Image_Format_BitWidth(image->format) >= 8);

  uint8_t *pixelPtr = ((uint8_t *) Image_RawDataPtr(image)) +
      index * (Image_Format_BitWidth(image->format) / 8);

  switch (Image_Format_BitWidth(image->format)) {
    case 256:return BitWidth256ChannelAt(channel, image->format, pixelPtr);
    case 192:return BitWidth192ChannelAt(channel, image->format, pixelPtr);
    case 128:return BitWidth128ChannelAt(channel, image->format, pixelPtr);
    case 96:return BitWidth96ChannelAt(channel, image->format, pixelPtr);
    case 64:return BitWidth64ChannelAt(channel, image->format, pixelPtr);
    case 48:return BitWidth48ChannelAt(channel, image->format, pixelPtr);
    case 32:return BitWidth32ChannelAt(channel, image->format, pixelPtr);
    case 24:return BitWidth24ChannelAt(channel, image->format, pixelPtr);
    case 16:return BitWidth16ChannelAt(channel, image->format, pixelPtr);
    case 8:return BitWidth8ChannelAt(channel, image->format, pixelPtr);
    default:LOGERROR("Bitwidth of format not supported");
      return 0.0;
  }

}

EXTERN_C void Image_SetChannelAt(Image_Header_t const *image,
                                 enum Image_Channel_t channel,
                                 size_t index,
                                 double value) {

}

EXTERN_C void Image_GetPixelAt(Image_Header_t const *image, Image_Pixel_t *pixel, size_t index) {
  ASSERT(image);
  ASSERT(pixel);

  // intentional fallthrough on this switch statement
  switch (Image_Format_ChannelCount(image->format)) {
    case 4:pixel->a = Image_GetChannelAt(image, Image_Alpha, index);
    case 3:pixel->b = Image_GetChannelAt(image, Image_Blue, index);
    case 2:pixel->g = Image_GetChannelAt(image, Image_Green, index);
    case 1:pixel->r = Image_GetChannelAt(image, Image_Red, index);
      break;
    default:ASSERT(Image_Format_ChannelCount(image->format) <= 4);
      break;
  }
}