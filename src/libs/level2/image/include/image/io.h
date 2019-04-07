#pragma once
#ifndef WYRD_IMAGE_IO_HPP
#define WYRD_IMAGE_IO_HPP

#include "core/core.h"
#include "image/image.h"
#include "vfile/vfile.h"

EXTERN_C Image_ImageHeader *Image_LoadDDS(VFile_Handle handle);
EXTERN_C Image_ImageHeader *Image_LoadPVR(VFile_Handle handle);
EXTERN_C Image_ImageHeader *Image_LoadLDR(VFile_Handle handle);
EXTERN_C Image_ImageHeader *Image_LoadHDR(VFile_Handle handle);
EXTERN_C Image_ImageHeader *Image_LoadEXR(VFile_Handle handle);

EXTERN_C bool Image_SaveDDS(Image_ImageHeader *image, VFile_Handle handle);
EXTERN_C bool Image_SaveTGA(Image_ImageHeader *image, VFile_Handle handle);
EXTERN_C bool Image_SaveBMP(Image_ImageHeader *image, VFile_Handle handle);
EXTERN_C bool Image_SavePNG(Image_ImageHeader *image, VFile_Handle handle);
EXTERN_C bool Image_SaveJPG(Image_ImageHeader *image, VFile_Handle handle);
EXTERN_C bool Image_SaveHDR(Image_ImageHeader *image, VFile_Handle handle);

// try to figure out which format the file is in and load it
EXTERN_C Image_ImageHeader *Image_Load(VFile_Handle handle);

#endif //WYRD_IMAGE_IO_HPP
