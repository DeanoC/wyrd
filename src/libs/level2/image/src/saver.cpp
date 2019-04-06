#include "core/core.h"
#include "core/logger.h"
#include "math/math.h"
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"
#include "vfile/vfile.hpp"
#include "image/format.h"
#include "image/format_cracker.h"
#include "image/image.h"
#include "syoyo/tiny_exr.hpp"
#include "dds.hpp"
#include <float.h>

EXTERN_C bool Image_SaveDDS(Image_ImageHeader *image, VFile_Handle handle) {
  using namespace Image;

  if (!handle) {
    return false;
  }

  DDSHeader header;
  DDSHeaderDX10 headerDX10;
  memset(&header, 0, sizeof(header));
  memset(&headerDX10, 0, sizeof(headerDX10));

  header.mDWMagic = MAKE_CHAR4('D', 'D', 'S', ' ');
  header.mDWSize = 124;

  header.mDWWidth = image->width;
  header.mDWHeight = image->height;
  header.mDWDepth = (image->depth > 1) ? image->depth : 0;
  header.mDWPitchOrLinearSize = 0;
  header.mDWMipMapCount = Image_LinkedCountOf(image);
  header.mPixelFormat.mDWSize = 32;

  header.mDWFlags =
      DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT // | TODO Deano (mMipMapCount > 1 ? DDSD_MIPMAPCOUNT : 0)
          | (image->depth > 1 ? DDSD_DEPTH : 0);

  int nChannels = Image_Format_ChannelCount(image->format);

  if (Image_Format_BitWidth(image->format) <= 32) {
    if (Image_Format_IsHomogenous(image->format)) {
      switch (Image_Format_ChannelBitWidth(image->format, 0)) {
        case 4:
          // only 4 channel 32 bit formats are 2x4 and 4x4 channels
          if (nChannels == 2) {
            header.mPixelFormat.mDWRGBBitCount = 8;
            header.mPixelFormat.mDWRGBAlphaBitMask = 0xF0;
            header.mPixelFormat.mDWRBitMask = 0x0F;
          } else {
            header.mPixelFormat.mDWRGBBitCount = 16;
            header.mPixelFormat.mDWRGBAlphaBitMask = 0xF000;
            header.mPixelFormat.mDWRBitMask = 0x0F00;
            header.mPixelFormat.mDWGBitMask = 0x00F0;
            header.mPixelFormat.mDWBBitMask = 0x000F;
          }
          break;
        case 8:header.mPixelFormat.mDWRGBBitCount = 8 * nChannels;
          header.mPixelFormat.mDWRGBAlphaBitMask = (nChannels == 4) ? 0xFF000000 : (nChannels == 2) ? 0xFF00 : 0;
          header.mPixelFormat.mDWRBitMask = (nChannels > 2) ? 0x00FF0000 : 0xFF;
          header.mPixelFormat.mDWGBitMask = (nChannels > 1) ? 0x0000FF00 : 0;
          header.mPixelFormat.mDWBBitMask = (nChannels > 1) ? 0x000000FF : 0;
          break;
        case 16:
          if (nChannels == 1) {
            header.mPixelFormat.mDWRGBBitCount = 16;
            header.mPixelFormat.mDWRBitMask = 0xFFFF;
          } else {
            header.mPixelFormat.mDWRGBBitCount = 32;
            header.mPixelFormat.mDWRBitMask = 0xFFFF0000;
            header.mPixelFormat.mDWRGBAlphaBitMask = 0x0000FFFF;
          }
          break;
        case 32:header.mPixelFormat.mDWRGBBitCount = 32;
          header.mPixelFormat.mDWRBitMask = 0xFFFFFFFF;
          break;
      }
    } else {
      switch (image->format) {
        case Image_Format_R5G6B5_UNORM_PACK16:
        case Image_Format_B5G6R5_UNORM_PACK16:header.mPixelFormat.mDWRGBBitCount = 16;
          header.mPixelFormat.mDWRGBAlphaBitMask = 0;
          header.mPixelFormat.mDWBBitMask = 0xF800;
          header.mPixelFormat.mDWGBitMask = 0x07E0;
          header.mPixelFormat.mDWRBitMask = 0x001F;
          break;
        case Image_Format_R5G5B5A1_UNORM_PACK16:
        case Image_Format_B5G5R5A1_UNORM_PACK16:
        case Image_Format_A1R5G5B5_UNORM_PACK16:header.mPixelFormat.mDWRGBBitCount = 16;
          header.mPixelFormat.mDWRGBAlphaBitMask = 0x0001;
          header.mPixelFormat.mDWRBitMask = 0xF800;
          header.mPixelFormat.mDWGBitMask = 0x07C0;
          header.mPixelFormat.mDWBBitMask = 0x003E;
          break;

        case Image_Format_A2R10G10B10_UNORM_PACK32:
        case Image_Format_A2R10G10B10_USCALED_PACK32:
        case Image_Format_A2R10G10B10_UINT_PACK32:
        case Image_Format_A2B10G10R10_UNORM_PACK32:
        case Image_Format_A2B10G10R10_USCALED_PACK32:
        case Image_Format_A2B10G10R10_UINT_PACK32:header.mPixelFormat.mDWRGBBitCount = 32;
          header.mPixelFormat.mDWRGBAlphaBitMask = 0xC0000000;
          header.mPixelFormat.mDWRBitMask = 0x3FF00000;
          header.mPixelFormat.mDWGBitMask = 0x000FFC00;
          header.mPixelFormat.mDWBBitMask = 0x000003FF;
          break;
        default:return false;
      }
    }
    header.mPixelFormat.mDWFlags = ((nChannels < 3) ? 0x00020000 : DDPF_RGB) | ((nChannels & 1) ? 0 : DDPF_ALPHAPIXELS);
  } else {
    header.mPixelFormat.mDWFlags = DDPF_FOURCC;

    switch (image->format) {
      case Image_Format_R16G16_UNORM: header.mPixelFormat.mDWFourCC = 34;
        break;
      case Image_Format_R16G16B16A16_UNORM: header.mPixelFormat.mDWFourCC = 36;
        break;
      case Image_Format_R16_SFLOAT: header.mPixelFormat.mDWFourCC = 111;
        break;
      case Image_Format_R16G16_SFLOAT: header.mPixelFormat.mDWFourCC = 112;
        break;
      case Image_Format_R16G16B16A16_SFLOAT: header.mPixelFormat.mDWFourCC = 113;
        break;
      case Image_Format_R32_SFLOAT: header.mPixelFormat.mDWFourCC = 114;
        break;
      case Image_Format_R32G32_SFLOAT: header.mPixelFormat.mDWFourCC = 115;
        break;
      case Image_Format_R32G32B32A32_SFLOAT: header.mPixelFormat.mDWFourCC = 116;
        break;
      case Image_Format_BC1_RGBA_UNORM_BLOCK: header.mPixelFormat.mDWFourCC = MAKE_CHAR4('D', 'X', 'T', '1');
        break;
      case Image_Format_BC2_UNORM_BLOCK: header.mPixelFormat.mDWFourCC = MAKE_CHAR4('D', 'X', 'T', '3');
        break;
      case Image_Format_BC3_UNORM_BLOCK: header.mPixelFormat.mDWFourCC = MAKE_CHAR4('D', 'X', 'T', '5');
        break;
      case Image_Format_BC4_UNORM_BLOCK: header.mPixelFormat.mDWFourCC = MAKE_CHAR4('A', 'T', 'I', '1');
        break;
      case Image_Format_BC5_UNORM_BLOCK: header.mPixelFormat.mDWFourCC = MAKE_CHAR4('A', 'T', 'I', '2');
        break;
      default:header.mPixelFormat.mDWFourCC = MAKE_CHAR4('D', 'X', '1', '0');
        headerDX10.mArraySize = 1;
        headerDX10.mDXGIFormat = Image_IsCubemap(image) ? D3D10_RESOURCE_MISC_TEXTURECUBE : 0;
        if (Image_Is1D(image)) {
          headerDX10.mResourceDimension = D3D10_RESOURCE_DIMENSION_TEXTURE1D;
        } else if (Image_Is2D(image)) {
          headerDX10.mResourceDimension = D3D10_RESOURCE_DIMENSION_TEXTURE2D;
        } else if (Image_Is3D(image)) {
          headerDX10.mResourceDimension = D3D10_RESOURCE_DIMENSION_TEXTURE3D;
        }

        switch (image->format) {
          case Image_Format_R32G32B32A32_SFLOAT: headerDX10.mDXGIFormat = 6;
            break;
          case Image_Format_E5B9G9R9_UFLOAT_PACK32: headerDX10.mDXGIFormat = 67;
            break;
          case Image_Format_B10G11R11_UFLOAT_PACK32: headerDX10.mDXGIFormat = 26;
            break;
          default: return false;
        }
    }
  }
  // header.

  header.mCaps.mDWCaps1 =
      DDSCAPS_TEXTURE | (Image_LinkedCountOf(image) > 1 ? DDSCAPS_MIPMAP | DDSCAPS_COMPLEX : 0) |
          (image->depth != 1 ? DDSCAPS_COMPLEX : 0);
  header.mCaps.mDWCaps2 = (image->depth > 1) ? DDSCAPS2_VOLUME :
                          Image_IsCubemap(image) ? DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_ALL_FACES : 0;
  header.mCaps.mReserved[0] = 0;
  header.mCaps.mReserved[1] = 0;
  header.mDWReserved2 = 0;

  VFile::File *file = VFile::File::FromHandle(handle);
  file->Write(&header, sizeof(header));

  if (headerDX10.mDXGIFormat) {
    file->Write(&headerDX10, sizeof(headerDX10) * 1);
  }

  int size = Image_BytesRequiredForMipMapsOf(image);

  // RGB to BGR
//  if (mFormat == RGB8 || mFormat == RGBA8) {
//    swapPixelChannels(pData, size / nChannels, nChannels, 0, 2);
//  }

  for (uint32_t mipMapLevel = 0; mipMapLevel < header.mDWMipMapCount; mipMapLevel++) {
    Image_ImageHeader const *face = Image_LinkedImageOf(image, mipMapLevel);

    int faceSize = Image_ByteCountOf(face);
    void *src = Image_RawDataPtr(face);
    file->Write(src, faceSize);
  }

  file->Close();

  // Restore to RGB
//  if (mFormat == RGB8 || mFormat == RGBA8) {
//    swapPixelChannels(pData, size / nChannels, nChannels, 0, 2);
//  }

  return true;
}
/*
bool convertAndSaveImage(const Image& image, bool (Image::*saverFunction)(const char *), const char *fileName) {
  bool bSaveImageSuccess = false;
  Image imgCopy(image);
  imgCopy.Uncompress();
  if (imgCopy.Convert(RGBA8)) {
    bSaveImageSuccess = (imgCopy.*saverFunction)(fileName);
  }

  imgCopy.Destroy();
  return bSaveImageSuccess;
}
*/

static void stbIoCallbackWrite(void *user, void *data, int size) {
  VFile_Handle handle = (VFile_Handle) user;
  VFile_Write(handle, data, size);
}

EXTERN_C bool Image_SaveTGA(Image_ImageHeader *image, VFile_Handle handle) {
  using namespace Image;

  if (!handle) {
    return false;
  }
  void *src = Image_RawDataPtr(image);

  switch (image->format) {
    case Image_Format_R8_UINT:
    case Image_Format_R8_SINT:
    case Image_Format_R8_UNORM:
    case Image_Format_R8_SNORM:
    case Image_Format_R8_USCALED:
    case Image_Format_R8_SSCALED:
    case Image_Format_R8_SRGB:
      return 0 != stbi_write_tga_to_func(&stbIoCallbackWrite, handle,
                                         image->width, image->height, 1, src);
    case Image_Format_R8G8_UINT:
    case Image_Format_R8G8_SINT:
    case Image_Format_R8G8_UNORM:
    case Image_Format_R8G8_SNORM:
    case Image_Format_R8G8_USCALED:
    case Image_Format_R8G8_SSCALED:
    case Image_Format_R8G8_SRGB:
      return 0 != stbi_write_tga_to_func(&stbIoCallbackWrite, handle,
                                         image->width, image->height, 2, src);
    case Image_Format_R8G8B8_UINT:
    case Image_Format_R8G8B8_SINT:
    case Image_Format_R8G8B8_UNORM:
    case Image_Format_R8G8B8_SNORM:
    case Image_Format_R8G8B8_USCALED:
    case Image_Format_R8G8B8_SSCALED:
    case Image_Format_R8G8B8_SRGB:
      return 0 != stbi_write_tga_to_func(&stbIoCallbackWrite, handle,
                                         image->width, image->height, 3, src);
    case Image_Format_R8G8B8A8_UINT:
    case Image_Format_R8G8B8A8_SINT:
    case Image_Format_R8G8B8A8_UNORM:
    case Image_Format_R8G8B8A8_SNORM:
    case Image_Format_R8G8B8A8_USCALED:
    case Image_Format_R8G8B8A8_SSCALED:
    case Image_Format_R8G8B8A8_SRGB:
      return 0 != stbi_write_tga_to_func(&stbIoCallbackWrite, handle,
                                         image->width, image->height, 4, src);
    default: {
      // uncompress/convert and try again
      //return convertAndSaveImage(*this, &Image::iSaveTGA, fileName);
      return false;
    }
  }
}

EXTERN_C bool Image_SaveBMP(Image_ImageHeader *image, VFile_Handle handle) {
  using namespace Image;

  if (!handle) {
    return false;
  }
  void *src = Image_RawDataPtr(image);

  switch (image->format) {
    case Image_Format_R8_UINT:
    case Image_Format_R8_SINT:
    case Image_Format_R8_UNORM:
    case Image_Format_R8_SNORM:
    case Image_Format_R8_USCALED:
    case Image_Format_R8_SSCALED:
    case Image_Format_R8_SRGB:
      return 0 != stbi_write_bmp_to_func(&stbIoCallbackWrite, handle,
                                         image->width, image->height, 1, src);
    case Image_Format_R8G8_UINT:
    case Image_Format_R8G8_SINT:
    case Image_Format_R8G8_UNORM:
    case Image_Format_R8G8_SNORM:
    case Image_Format_R8G8_USCALED:
    case Image_Format_R8G8_SSCALED:
    case Image_Format_R8G8_SRGB:
      return 0 != stbi_write_bmp_to_func(&stbIoCallbackWrite, handle,
                                         image->width, image->height, 2, src);
    case Image_Format_R8G8B8_UINT:
    case Image_Format_R8G8B8_SINT:
    case Image_Format_R8G8B8_UNORM:
    case Image_Format_R8G8B8_SNORM:
    case Image_Format_R8G8B8_USCALED:
    case Image_Format_R8G8B8_SSCALED:
    case Image_Format_R8G8B8_SRGB:
      return 0 != stbi_write_bmp_to_func(&stbIoCallbackWrite, handle,
                                         image->width, image->height, 3, src);
    case Image_Format_R8G8B8A8_UINT:
    case Image_Format_R8G8B8A8_SINT:
    case Image_Format_R8G8B8A8_UNORM:
    case Image_Format_R8G8B8A8_SNORM:
    case Image_Format_R8G8B8A8_USCALED:
    case Image_Format_R8G8B8A8_SSCALED:
    case Image_Format_R8G8B8A8_SRGB:
      return 0 != stbi_write_bmp_to_func(&stbIoCallbackWrite, handle,
                                         image->width, image->height, 4, src);
    default: {
      // uncompress/convert and try again
      //return convertAndSaveImage(*this, &Image::iSaveTGA, fileName);
      return false;
    }
  }
}

EXTERN_C bool Image_SavePNG(Image_ImageHeader *image, VFile_Handle handle) {
  using namespace Image;

  if (!handle) {
    return false;
  }
  void *src = Image_RawDataPtr(image);

  switch (image->format) {
    case Image_Format_R8_UINT:
    case Image_Format_R8_SINT:
    case Image_Format_R8_UNORM:
    case Image_Format_R8_SNORM:
    case Image_Format_R8_USCALED:
    case Image_Format_R8_SSCALED:
    case Image_Format_R8_SRGB:
      return 0 != stbi_write_png_to_func(&stbIoCallbackWrite, handle,
                                         image->width, image->height, 1, src, 0);
    case Image_Format_R8G8_UINT:
    case Image_Format_R8G8_SINT:
    case Image_Format_R8G8_UNORM:
    case Image_Format_R8G8_SNORM:
    case Image_Format_R8G8_USCALED:
    case Image_Format_R8G8_SSCALED:
    case Image_Format_R8G8_SRGB:
      return 0 != stbi_write_png_to_func(&stbIoCallbackWrite, handle,
                                         image->width, image->height, 2, src, 0);
    case Image_Format_R8G8B8_UINT:
    case Image_Format_R8G8B8_SINT:
    case Image_Format_R8G8B8_UNORM:
    case Image_Format_R8G8B8_SNORM:
    case Image_Format_R8G8B8_USCALED:
    case Image_Format_R8G8B8_SSCALED:
    case Image_Format_R8G8B8_SRGB:
      return 0 != stbi_write_png_to_func(&stbIoCallbackWrite, handle,
                                         image->width, image->height, 3, src, 0);
    case Image_Format_R8G8B8A8_UINT:
    case Image_Format_R8G8B8A8_SINT:
    case Image_Format_R8G8B8A8_UNORM:
    case Image_Format_R8G8B8A8_SNORM:
    case Image_Format_R8G8B8A8_USCALED:
    case Image_Format_R8G8B8A8_SSCALED:
    case Image_Format_R8G8B8A8_SRGB:
      return 0 != stbi_write_png_to_func(&stbIoCallbackWrite, handle,
                                         image->width, image->height, 4, src, 0);
    default: {
      // uncompress/convert and try again
      //return convertAndSaveImage(*this, &Image::iSaveTGA, fileName);
      return false;
    }
  }
}

EXTERN_C bool Image_SaveJPG(Image_ImageHeader *image, VFile_Handle handle) {
  using namespace Image;

  if (!handle) {
    return false;
  }
  void *src = Image_RawDataPtr(image);

  switch (image->format) {
    case Image_Format_R8_UINT:
    case Image_Format_R8_SINT:
    case Image_Format_R8_UNORM:
    case Image_Format_R8_SNORM:
    case Image_Format_R8_USCALED:
    case Image_Format_R8_SSCALED:
    case Image_Format_R8_SRGB:
      return 0 != stbi_write_jpg_to_func(&stbIoCallbackWrite, handle,
                                         image->width, image->height, 1, src, 0);
    case Image_Format_R8G8_UINT:
    case Image_Format_R8G8_SINT:
    case Image_Format_R8G8_UNORM:
    case Image_Format_R8G8_SNORM:
    case Image_Format_R8G8_USCALED:
    case Image_Format_R8G8_SSCALED:
    case Image_Format_R8G8_SRGB:
      return 0 != stbi_write_jpg_to_func(&stbIoCallbackWrite, handle,
                                         image->width, image->height, 2, src, 0);
    case Image_Format_R8G8B8_UINT:
    case Image_Format_R8G8B8_SINT:
    case Image_Format_R8G8B8_UNORM:
    case Image_Format_R8G8B8_SNORM:
    case Image_Format_R8G8B8_USCALED:
    case Image_Format_R8G8B8_SSCALED:
    case Image_Format_R8G8B8_SRGB:
      return 0 != stbi_write_jpg_to_func(&stbIoCallbackWrite, handle,
                                         image->width, image->height, 3, src, 0);
    case Image_Format_R8G8B8A8_UINT:
    case Image_Format_R8G8B8A8_SINT:
    case Image_Format_R8G8B8A8_UNORM:
    case Image_Format_R8G8B8A8_SNORM:
    case Image_Format_R8G8B8A8_USCALED:
    case Image_Format_R8G8B8A8_SSCALED:
    case Image_Format_R8G8B8A8_SRGB:
      return 0 != stbi_write_jpg_to_func(&stbIoCallbackWrite, handle,
                                         image->width, image->height, 4, src, 0);
    default: {
      // uncompress/convert and try again
      //return convertAndSaveImage(*this, &Image::iSaveTGA, fileName);
      return false;
    }
  }
}

EXTERN_C bool Image_SaveHDR(Image_ImageHeader *image, VFile_Handle handle) {
  using namespace Image;

  if (!handle) {
    return false;
  }
  float const *src = (float const*) Image_RawDataPtr(image);

  switch (image->format) {
    case Image_Format_R32_SFLOAT:
      return 0 != stbi_write_hdr_to_func(&stbIoCallbackWrite, handle,
                                         image->width, image->height, 1, src);
    case Image_Format_R32G32_SFLOAT:
      return 0 != stbi_write_hdr_to_func(&stbIoCallbackWrite, handle,
                                         image->width, image->height, 2, src);
    case Image_Format_R32G32B32_SFLOAT:
      return 0 != stbi_write_hdr_to_func(&stbIoCallbackWrite, handle,
                                         image->width, image->height, 3, src);
    case Image_Format_R32G32B32A32_SFLOAT:
      return 0 != stbi_write_hdr_to_func(&stbIoCallbackWrite, handle,
                                         image->width, image->height, 4, src);
    default: {
      // uncompress/convert and try again
      //return convertAndSaveImage(*this, &Image::iSaveTGA, fileName);
      return false;
    }
  }
}