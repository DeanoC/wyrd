#pragma once
#ifndef WYRD_IMAGE_CREATE_H
#define WYRD_IMAGE_CREATE_H

#include "core/core.h"
#include "image/image.h"

// helpers
EXTERN_C Image_ImageHeader *Image_Create1D(uint32_t width, enum Image_Format format);
EXTERN_C Image_ImageHeader *Image_Create1DNoClear(uint32_t width, enum Image_Format format);
EXTERN_C Image_ImageHeader *Image_Create1DArray(uint32_t width, uint32_t slices, enum Image_Format format);
EXTERN_C Image_ImageHeader *Image_Create1DArrayNoClear(uint32_t width, uint32_t slices, enum Image_Format format);
EXTERN_C Image_ImageHeader *Image_Create2D(uint32_t width, uint32_t height, enum Image_Format format);
EXTERN_C Image_ImageHeader *Image_Create2DNoClear(uint32_t width, uint32_t height, enum Image_Format format);
EXTERN_C Image_ImageHeader *Image_Create2DArray(uint32_t width,
                                                uint32_t height,
                                                uint32_t slices,
                                                enum Image_Format format);
EXTERN_C Image_ImageHeader *Image_Create2DArrayNoClear(uint32_t width,
                                                       uint32_t height,
                                                       uint32_t slices,
                                                       enum Image_Format format);
EXTERN_C Image_ImageHeader *Image_Create3D(uint32_t width, uint32_t height, uint32_t depth, enum Image_Format format);
EXTERN_C Image_ImageHeader *Image_Create3DNoClear(uint32_t width,
                                                  uint32_t height,
                                                  uint32_t depth,
                                                  enum Image_Format format);
EXTERN_C Image_ImageHeader *Image_Create3DArray(uint32_t width,
                                                uint32_t height,
                                                uint32_t depth,
                                                uint32_t slices,
                                                enum Image_Format format);
EXTERN_C Image_ImageHeader *Image_Create3DArrayNoClear(uint32_t width,
                                                       uint32_t height,
                                                       uint32_t depth,
                                                       uint32_t slices,
                                                       enum Image_Format format);

EXTERN_C Image_ImageHeader *Image_CreateCubemap(uint32_t width, uint32_t height, enum Image_Format format);
EXTERN_C Image_ImageHeader *Image_CreateCubemapNoClear(uint32_t width, uint32_t height, enum Image_Format format);
EXTERN_C Image_ImageHeader *Image_CreateCubemapArray(uint32_t width,
                                                     uint32_t height,
                                                     uint32_t slices,
                                                     enum Image_Format format);
EXTERN_C Image_ImageHeader *Image_CreateCubemapArrayNoClear(uint32_t width,
                                                            uint32_t height,
                                                            uint32_t slices,
                                                            enum Image_Format format);




#endif //WYRD_IMAGE_CREATE_H
