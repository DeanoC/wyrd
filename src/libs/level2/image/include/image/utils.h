#pragma once
#ifndef WYRD_IMAGE_UTILS_H
#define WYRD_IMAGE_UTILS_H

#include "core/core.h"

EXTERN_C bool Image_GetColorRangeOf(Image_ImageHeader const * src, Image_PixelD* omin, Image_PixelD* omax);
EXTERN_C bool Image_GetColorRangeOfF(Image_ImageHeader const * image, float* omin, float* omax);
EXTERN_C bool Image_GetColorRangeOfD(Image_ImageHeader const * image, double* omin, double* omax);

EXTERN_C bool Image_NormalizeEachChannelOf(Image_ImageHeader const * src);
EXTERN_C bool Image_NormalizeAcrossChannelsOf(Image_ImageHeader const * src);

EXTERN_C void Image_CreateMipMapChain(Image_ImageHeader *image, bool generateFromImage);
EXTERN_C Image_ImageHeader* Image_Clone(Image_ImageHeader* image);
EXTERN_C Image_ImageHeader *Image_CloneStructure(Image_ImageHeader *image);

EXTERN_C Image_ImageHeader* Image_PreciseConvert(Image_ImageHeader* src, Image_Format const newFormat);
EXTERN_C Image_ImageHeader* Image_FastConvert(Image_ImageHeader* src, Image_Format const newFormat, bool allowInplace);

#endif //WYRD_IMAGE_UTILS_H
