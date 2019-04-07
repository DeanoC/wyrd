#include "core/core.h"
#include "image/format.h"
#include "image/format_cracker.h"
#include "image/image.h"
#include "image/create.h"

EXTERN_C Image_ImageHeader *Image_Create1D(uint32_t width, enum Image_Format format) {
  return Image_Create(width, 1, 1, 1, format);
}
EXTERN_C Image_ImageHeader *Image_Create1DNoClear(uint32_t width, enum Image_Format format) {
  return Image_CreateNoClear(width, 1, 1, 1, format);
}
EXTERN_C Image_ImageHeader *Image_Create1DArray(uint32_t width, uint32_t slices, enum Image_Format format) {
  return Image_Create(width, 1, 1, slices, format);
}
EXTERN_C Image_ImageHeader *Image_Create1DArrayNoClear(uint32_t width, uint32_t slices, enum Image_Format format) {
  return Image_CreateNoClear(width, 1, 1, slices, format);
}

EXTERN_C Image_ImageHeader *Image_Create2D(uint32_t width, uint32_t height, enum Image_Format format) {
  return Image_Create(width, height, 1, 1, format);
}
EXTERN_C Image_ImageHeader *Image_Create2DNoClear(uint32_t width, uint32_t height, enum Image_Format format) {
  return Image_CreateNoClear(width, height, 1, 1, format);
}
EXTERN_C Image_ImageHeader *Image_Create2DArray(uint32_t width,
                                                uint32_t height,
                                                uint32_t slices,
                                                enum Image_Format format) {
  return Image_Create(width, height, 1, slices, format);
}
EXTERN_C Image_ImageHeader *Image_Create2DArrayNoClear(uint32_t width,
                                                       uint32_t height,
                                                       uint32_t slices,
                                                       enum Image_Format format) {
  return Image_CreateNoClear(width, height, 1, slices, format);
}

EXTERN_C Image_ImageHeader *Image_Create3D(uint32_t width, uint32_t height, uint32_t depth, enum Image_Format format) {
  return Image_Create(width, height, depth, 1, format);
}
EXTERN_C Image_ImageHeader *Image_Create3DNoClear(uint32_t width,
                                                  uint32_t height,
                                                  uint32_t depth,
                                                  enum Image_Format format) {
  return Image_CreateNoClear(width, height, depth, 1, format);
}
EXTERN_C Image_ImageHeader *Image_Create3DArray(uint32_t width,
                                                uint32_t height,
                                                uint32_t depth,
                                                uint32_t slices,
                                                enum Image_Format format) {
  return Image_Create(width, height, depth, slices, format);
}
EXTERN_C Image_ImageHeader *Image_Create3DArrayNoClear(uint32_t width,
                                                       uint32_t height,
                                                       uint32_t depth,
                                                       uint32_t slices,
                                                       enum Image_Format format) {
  return Image_CreateNoClear(width, height, depth, slices, format);
}

EXTERN_C Image_ImageHeader *Image_CreateCubemap(uint32_t width, uint32_t height, enum Image_Format format) {
  Image_ImageHeader* image = Image_Create(width, height, 1, 6, format);
  if(image) {
    image->flags = Image_Flag_Cubemap;
  }
  return image;}
EXTERN_C Image_ImageHeader *Image_CreateCubemapNoClear(uint32_t width, uint32_t height, enum Image_Format format) {
  Image_ImageHeader* image = Image_CreateNoClear(width, height, 1, 6, format);
  if(image) {
    image->flags = Image_Flag_Cubemap;
  }
  return image;
}
EXTERN_C Image_ImageHeader *Image_CreateCubemapArray(uint32_t width,
                                                     uint32_t height,
                                                     uint32_t slices,
                                                     enum Image_Format format) {
  Image_ImageHeader* image = Image_Create(width, height, 1, slices * 6, format);
  if(image) {
    image->flags = Image_Flag_Cubemap;
  }
  return image;
}
EXTERN_C Image_ImageHeader *Image_CreateCubemapArrayNoClear(uint32_t width,
                                                            uint32_t height,
                                                            uint32_t slices,
                                                            enum Image_Format format) {
  Image_ImageHeader* image = Image_CreateNoClear(width, height, 1, slices * 6, format);
  if(image) {
    image->flags = Image_Flag_Cubemap;
  }
  return image;
}


