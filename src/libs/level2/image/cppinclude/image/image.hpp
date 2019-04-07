#pragma once
#ifndef WYRD_IMAGE_IMAGE_HPP
#define WYRD_IMAGE_IMAGE_HPP

#include "core/core.h"
#include "image/image.h"

namespace Image {

struct Image : public Image_ImageHeader {

  static Image *From(Image_ImageHeader *handle) {
    return (Image *) handle;
  }
  static Image *Create(uint32_t width, uint32_t height,
                       uint32_t depth, uint32_t slices,
                       enum Image_Format format) {
    return (Image *) Image_Create(width, height, depth, slices, format);
  }
  static Image *CreateNoClear(uint32_t width, uint32_t height,
                              uint32_t depth, uint32_t slices,
                              enum Image_Format format) {
    return (Image *) Image_CreateNoClear(width, height, depth, slices, format);
  }

  explicit operator Image_ImageHeader *() const { return (Image_ImageHeader *) this; }

  void Destroy() { Image_Destroy(this); }

  size_t Index(uint32_t x, uint32_t y, uint32_t z, uint32_t slice) const {
    return Image_CalculateIndex(this, x, y, z, slice);
  }

  Image_PixelD PixelAt(size_t index) const {
    Image_PixelD ret;
    Image_GetPixelAt(this, &ret, index);
    return ret;
  }
  double ChannelAt(size_t index, Image_Channel channel) const {
    return Image_GetChannelAt(this, channel, index);
  }

  void SetPixelAt(size_t index, Image_PixelD const& pixel) {
    Image_SetPixelAt(this, &pixel, index);
  }
  void SetChannelAt(size_t index, Image_Channel channel, double val) {
    Image_SetChannelAt(this, channel, index, val);
  }
  bool Is1D() const { return Image_Is1D(this); }
  bool Is2D() const { return Image_Is2D(this); }
  bool Is3D() const { return Image_Is3D(this); }
  bool IsArray() const { return Image_IsArray(this); }
  bool IsCubemap() const { return Image_IsCubemap(this); }

  bool HasMipmaps() const { return flags & Image_IT_MipMaps; }
  bool HasLayers() const { return flags & IMAGE_IT_Layers; }

  size_t LinkedCount() const { return Image_LinkedImageCountOf(this); }
  Image *LinkedImageAt(size_t const index) const { return (Image *) Image_LinkedImageOf(this, index); }

  template<typename T = void>
  T const *Data() const { return (T const *) Image_RawDataPtr(this); }
  template<typename T = void>
  T *Data() { return (T *) Image_RawDataPtr(this); }

  size_t PixelCountPerRow() const { return width; }
  size_t PixelCountPerPage() const { return width * height; }
  size_t PixelCountPerSlice() const { return width * height * depth; }
  size_t PixelCount() const { return width * height * depth * slices; }
  size_t PixelCountOfImageChain() const { return Image_PixelCountOfImageChainOf(this); }

  size_t ByteCountPerRow() const { return Image_ByteCountPerRowOf(this); }
  size_t ByteCountPerPage() const { return Image_ByteCountPerPageOf(this); }
  size_t ByteCountPerSlice() const { return Image_ByteCountPerSliceOf(this); }
  size_t ByteCount() const { return Image_ByteCountOf(this); }
  size_t ByteCountOfImageChain() const { return Image_ByteCountOfImageChainOf(this); }

 private:

  Image() = delete;
};

}

#endif //WYRD_IMAGE_IMAGE_HPP
