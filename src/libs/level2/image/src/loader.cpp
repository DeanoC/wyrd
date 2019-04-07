#include "core/core.h"
#include "core/logger.h"
#include "math/math.h"
#include "stb/stb_image.h"
#include "vfile/vfile.hpp"
#include "image/format.h"
#include "image/format_cracker.h"
#include "image/image.h"
#include "image/utils.h"
#include "syoyo/tiny_exr.hpp"
#include "dds.hpp"
#include <float.h>


// Describes the header of a PVR header-texture
typedef struct PVR_Header_Texture_TAG {
  uint32_t mVersion;
  uint32_t mFlags; //!< Various format flags.
  uint64_t mPixelFormat; //!< The pixel format, 8cc value storing the 4 channel identifiers and their respective sizes.
  uint32_t mColorSpace; //!< The Color Space of the texture, currently either linear RGB or sRGB.
  uint32_t
      mChannelType; //!< Variable type that the channel is stored in. Supports signed/unsigned int/short/char/float.
  uint32_t mHeight; //!< Height of the texture.
  uint32_t mWidth; //!< Width of the texture.
  uint32_t mDepth; //!< Depth of the texture. (Z-slices)
  uint32_t mNumSurfaces; //!< Number of members in a Texture Array.
  uint32_t mNumFaces; //!< Number of faces in a Cube Map. Maybe be a value other than 6.
  uint32_t mNumMipMaps; //!< Number of MIP Maps in the texture - NB: Includes top level.
  uint32_t mMetaDataSize; //!< Size of the accompanying meta data.
} PVR_Texture_Header;

const unsigned int gPvrtexV3HeaderVersion = 0x03525650;

// Load Image Data form mData functions

EXTERN_C Image_ImageHeader *Image_LoadDDS(VFile_Handle handle) {
  using namespace Image;
  DDSHeader header;

  VFile::File *file = VFile::File::FromHandle(handle);
  file->Read(&header, sizeof(header));

  if (header.mDWMagic != MAKE_CHAR4('D', 'D', 'S', ' ')) {
    return nullptr;
  }

  Image_ImageHeader *image = nullptr;
  Image_Format format = Image_Format_UNDEFINED;

  if (header.mPixelFormat.mDWFourCC == MAKE_CHAR4('D', 'X', '1', '0')) {
    DDSHeaderDX10 dx10Header;
    file->Read(&dx10Header, sizeof(dx10Header));

    switch (dx10Header.mDXGIFormat) {
      case DDS_DXGI_FORMAT_R32G32B32A32_FLOAT: format = Image_Format_R32G32B32A32_SFLOAT;
        break;
      case DDS_DXGI_FORMAT_R32G32B32A32_UINT: format = Image_Format_R32G32B32A32_UINT;
        break;
      case DDS_DXGI_FORMAT_R32G32B32A32_SINT: format = Image_Format_R32G32B32A32_SINT;
        break;
      case DDS_DXGI_FORMAT_R32G32B32_FLOAT: format = Image_Format_R32G32B32_SFLOAT;
        break;
      case DDS_DXGI_FORMAT_R32G32B32_UINT: format = Image_Format_R32G32B32_UINT;
        break;
      case DDS_DXGI_FORMAT_R32G32B32_SINT: format = Image_Format_R32G32B32_SINT;
        break;
      case DDS_DXGI_FORMAT_R16G16B16A16_FLOAT: format = Image_Format_R16G16B16A16_SFLOAT;
        break;
      case DDS_DXGI_FORMAT_R16G16B16A16_UNORM: format = Image_Format_R16G16B16A16_UNORM;
        break;
      case DDS_DXGI_FORMAT_R16G16B16A16_UINT: format = Image_Format_R16G16B16A16_UINT;
        break;
      case DDS_DXGI_FORMAT_R16G16B16A16_SNORM: format = Image_Format_R16G16B16A16_SNORM;
        break;
      case DDS_DXGI_FORMAT_R16G16B16A16_SINT: format = Image_Format_R16G16B16A16_SINT;
        break;
      case DDS_DXGI_FORMAT_R32G32_FLOAT: format = Image_Format_R32G32_SFLOAT;
        break;
      case DDS_DXGI_FORMAT_R32G32_UINT: format = Image_Format_R32G32_UINT;
        break;
      case DDS_DXGI_FORMAT_R32G32_SINT: format = Image_Format_R32G32_SINT;
        break;
      case DDS_DXGI_FORMAT_R10G10B10A2_UNORM: format = Image_Format_A2B10G10R10_UNORM_PACK32;
        break;
      case DDS_DXGI_FORMAT_R10G10B10A2_UINT: format = Image_Format_A2B10G10R10_UINT_PACK32;
        break;
      case DDS_DXGI_FORMAT_R8G8B8A8_UNORM: format = Image_Format_R8G8B8A8_UNORM;
        break;
      case DDS_DXGI_FORMAT_R8G8B8A8_UNORM_SRGB: format = Image_Format_R8G8B8A8_SRGB;
        break;
      case DDS_DXGI_FORMAT_R8G8B8A8_UINT: format = Image_Format_R8G8B8A8_UINT;
        break;
      case DDS_DXGI_FORMAT_R8G8B8A8_SNORM: format = Image_Format_R8G8B8A8_SNORM;
        break;
      case DDS_DXGI_FORMAT_R8G8B8A8_SINT: format = Image_Format_R8G8B8A8_SINT;
        break;
      case DDS_DXGI_FORMAT_R16G16_FLOAT: format = Image_Format_R16G16_SFLOAT;
        break;
      case DDS_DXGI_FORMAT_R16G16_UNORM: format = Image_Format_R16G16_UNORM;
        break;
      case DDS_DXGI_FORMAT_R16G16_UINT: format = Image_Format_R16G16_UINT;
        break;
      case DDS_DXGI_FORMAT_R16G16_SNORM: format = Image_Format_R16G16_SNORM;
        break;
      case DDS_DXGI_FORMAT_R16G16_SINT: format = Image_Format_R16G16_SINT;
        break;
      case DDS_DXGI_FORMAT_D32_FLOAT: format = Image_Format_D32_SFLOAT;
        break;
      case DDS_DXGI_FORMAT_R32_FLOAT: format = Image_Format_R32_SFLOAT;
        break;
      case DDS_DXGI_FORMAT_R32_UINT: format = Image_Format_R32_UINT;
        break;
      case DDS_DXGI_FORMAT_R32_SINT: format = Image_Format_R32_SINT;
        break;
      case DDS_DXGI_FORMAT_R8G8_UNORM: format = Image_Format_R8G8_UNORM;
        break;
      case DDS_DXGI_FORMAT_R8G8_UINT: format = Image_Format_R8G8_UINT;
        break;
      case DDS_DXGI_FORMAT_R8G8_SNORM: format = Image_Format_R8G8_SNORM;
        break;
      case DDS_DXGI_FORMAT_R8G8_SINT: format = Image_Format_R8G8_SINT;
        break;
      case DDS_DXGI_FORMAT_R16_FLOAT: format = Image_Format_R16_SFLOAT;
        break;
      case DDS_DXGI_FORMAT_D16_UNORM: format = Image_Format_D16_UNORM;
        break;
      case DDS_DXGI_FORMAT_R16_UNORM: format = Image_Format_R16_UNORM;
        break;
      case DDS_DXGI_FORMAT_R16_UINT: format = Image_Format_R16_UINT;
        break;
      case DDS_DXGI_FORMAT_R16_SNORM: format = Image_Format_R16_SNORM;
        break;
      case DDS_DXGI_FORMAT_R16_SINT: format = Image_Format_R16_SINT;
        break;
      case DDS_DXGI_FORMAT_R8_UNORM: format = Image_Format_R8_UNORM;
        break;
      case DDS_DXGI_FORMAT_R8_UINT: format = Image_Format_R8_UINT;
        break;
      case DDS_DXGI_FORMAT_R8_SNORM: format = Image_Format_R8_UNORM;
        break;
      case DDS_DXGI_FORMAT_R8_SINT: format = Image_Format_R8_SINT;
        break;
      case DDS_DXGI_FORMAT_A8_UNORM: format = Image_Format_R8_UNORM;
        break;
      case DDS_DXGI_FORMAT_BC1_UNORM: format = Image_Format_BC1_RGB_UNORM_BLOCK;
        break;
      case DDS_DXGI_FORMAT_BC1_UNORM_SRGB: format = Image_Format_BC1_RGBA_UNORM_BLOCK;
        break;
      case DDS_DXGI_FORMAT_BC2_UNORM: format = Image_Format_BC2_UNORM_BLOCK;
        break;
      case DDS_DXGI_FORMAT_BC2_UNORM_SRGB: format = Image_Format_BC2_SRGB_BLOCK;
        break;
      case DDS_DXGI_FORMAT_BC3_UNORM: format = Image_Format_BC3_UNORM_BLOCK;
        break;
      case DDS_DXGI_FORMAT_BC3_UNORM_SRGB: format = Image_Format_BC2_SRGB_BLOCK;
        break;
      case DDS_DXGI_FORMAT_BC4_UNORM: format = Image_Format_BC4_UNORM_BLOCK;
        break;
      case DDS_DXGI_FORMAT_BC4_SNORM: format = Image_Format_BC4_SNORM_BLOCK;
        break;
      case DDS_DXGI_FORMAT_BC5_UNORM: format = Image_Format_BC5_UNORM_BLOCK;
        break;
      case DDS_DXGI_FORMAT_BC5_SNORM: format = Image_Format_BC5_SNORM_BLOCK;
        break;
      case DDS_DXGI_FORMAT_B5G6R5_UNORM: format = Image_Format_B5G6R5_UNORM_PACK16;
        break;
      case DDS_DXGI_FORMAT_B5G5R5A1_UNORM: format = Image_Format_B5G5R5A1_UNORM_PACK16;
        break;
      case DDS_DXGI_FORMAT_B8G8R8A8_UNORM: format = Image_Format_B8G8R8_UNORM;
        break;
      case DDS_DXGI_FORMAT_B8G8R8X8_UNORM: format = Image_Format_B8G8R8A8_UNORM;
        break;
      case DDS_DXGI_FORMAT_B8G8R8A8_UNORM_SRGB: format = Image_Format_B8G8R8A8_SRGB;
        break;
      case DDS_DXGI_FORMAT_B8G8R8X8_UNORM_SRGB: format = Image_Format_B8G8R8A8_SRGB;
        break;
      case DDS_DXGI_FORMAT_BC6H_UF16: format = Image_Format_BC6H_UFLOAT_BLOCK;
        break;
      case DDS_DXGI_FORMAT_BC6H_SF16: format = Image_Format_BC6H_SFLOAT_BLOCK;
        break;
      case DDS_DXGI_FORMAT_BC7_UNORM: format = Image_Format_BC7_UNORM_BLOCK;
        break;
      case DDS_DXGI_FORMAT_BC7_UNORM_SRGB: format = Image_Format_BC7_SRGB_BLOCK;
        break;
      case DDS_DXGI_FORMAT_P8: format = Image_Format_UNDEFINED;
        break;
      case DDS_DXGI_FORMAT_B4G4R4A4_UNORM: format = Image_Format_B4G4R4A4_UNORM_PACK16;
        break;
      default: return nullptr;
    }
  } else {
    switch (header.mPixelFormat.mDWFourCC) {
      case 34: format = Image_Format_R16G16_UNORM;
        break;
      case 36: format = Image_Format_R16G16B16A16_UNORM;
        break;
      case 111: format = Image_Format_R16_SFLOAT;
        break;
      case 112: format = Image_Format_R16G16_SFLOAT;
        break;
      case 113: format = Image_Format_R16G16B16A16_SFLOAT;
        break;
      case 114: format = Image_Format_R32_SFLOAT;
        break;
      case 115: format = Image_Format_R32G32_SFLOAT;
        break;
      case 116: format = Image_Format_R32G32B32A32_SFLOAT;
        break;

/*      case MAKE_CHAR4('A', 'T', 'C', ' '): format = ATC;
        break;
      case MAKE_CHAR4('A', 'T', 'C', 'A'): format = ATCA;
        break;
      case MAKE_CHAR4('A', 'T', 'C', 'I'): format = ATCI;
        break;
      case MAKE_CHAR4('A', 'T', 'I', '1'): format = ATI1N;
        break;
      case MAKE_CHAR4('A', 'T', 'I', '2'): format = ATI2N;
        break;
      case MAKE_CHAR4('E', 'T', 'C', ' '): format = ETC1;
        break; */ //TODO
      case MAKE_CHAR4('D', 'X', 'T', '1'): format = Image_Format_BC1_RGBA_UNORM_BLOCK;
        break;
      case MAKE_CHAR4('D', 'X', 'T', '3'): format = Image_Format_BC2_UNORM_BLOCK;
        break;
      case MAKE_CHAR4('D', 'X', 'T', '5'): format = Image_Format_BC5_UNORM_BLOCK;
        break;
      default:
        switch (header.mPixelFormat.mDWRGBBitCount) {
          case 8: format = Image_Format_R8_UNORM;
            break;
          case 16:
            // TODO need to swizzle
            format = (header.mPixelFormat.mDWRGBAlphaBitMask == 0xF000)
                     ? Image_Format_B4G4R4A4_UNORM_PACK16
                     : (header.mPixelFormat.mDWRGBAlphaBitMask == 0xFF00)
                       ? Image_Format_R8G8_UNORM
                       : (header.mPixelFormat.mDWBBitMask == 0x1F) ?
                         Image_Format_R5G6B5_UNORM_PACK16 :
                         Image_Format_R16_UNORM;
            break;
          case 24: format = Image_Format_R8G8B8_UNORM;
            break;
          case 32:
            format = (header.mPixelFormat.mDWRBitMask == 0x3FF00000) ?
                     Image_Format_A2R10G10B10_UNORM_PACK32 :
                     Image_Format_R8G8B8A8_UNORM;
            break;
          default:return nullptr;
        }
    }
  }
  if (format == Image_Format_UNDEFINED) { return nullptr; }

  image = Image_Create(header.mDWWidth,
                       header.mDWHeight,
                       (header.mDWDepth == 0) ? 1 : header.mDWDepth,
                       (header.mCaps.mDWCaps2 & DDSCAPS2_CUBEMAP) ? 6 : 1,
                       format);
  file->Read(Image_RawDataPtr(image), Image_ByteCountOf(image));

  if (header.mDWMipMapCount != 1) {
    Image_CreateMipMapChain(image, true);
  }

  /*TODO Deano use DDS mipmaps if store in file!
  size_t size = Image_BytesForImageAndMipMaps(image);
  if (header.mCaps.mDWCaps2 & DDSCAPS2_CUBEMAP) {
    for (int face = 0; face < 6; face++) {
      for (uint32_t mipMapLevel = 0; mipMapLevel < mMipMapCount; mipMapLevel++) {
        int faceSize = GetMipMappedSize(mipMapLevel, 1) / 6;
        unsigned char *src = GetPixels(pData, mipMapLevel) + face * faceSize;

        file->Read(src, faceSize);
        //MemFopen::FileRead(src, 1, faceSize, file);
      }
    }
  } else {
    file->Read(pData, size);
  }

  if ((mFormat == RGB8 || mFormat == RGBA8) && header.mPixelFormat.mDWBBitMask == 0xFF) {
    int nChannels = GetChannelCount(mFormat);
    swapPixelChannels(pData, size / nChannels, nChannels, 0, 2);
  }
*/
  return image;
}

EXTERN_C Image_ImageHeader *Image_LoadPVR(VFile_Handle handle) {
  // TODO: Image
  // - no support for PVRTC2 at the moment since it isn't supported on iOS devices.
  // - only new PVR header V3 is supported at the moment.  Should we add legacy for V2 and V1?
  // - metadata is ignored for now.  Might be useful to implement it if the need for metadata arises (eg. padding, atlas coordinates, orientations, border data, etc...).
  // - flags are also ignored for now.  Currently a flag of 0x02 means that the color have been pre-multiplied byt the alpha values.

  // Assumptions:
  // - it's assumed that the texture is already twiddled (ie. Morton).  This should always be the case for PVRTC V3.

  PVR_Texture_Header header;
  VFile::File *file = VFile::File::FromHandle(handle);
  file->Read(&header, sizeof(header));

  if (header.mVersion != gPvrtexV3HeaderVersion) {
    LOGERRORF("Load PVR failed: Not a valid PVR V3 header.");
    return nullptr;
  }

  if (header.mPixelFormat > 3) {
    LOGERRORF("Load PVR failed: Not a supported PVR pixel format.  Only PVRTC is supported at the moment.");
    return nullptr;
  }

  if (header.mNumSurfaces > 1 && header.mNumFaces > 1) {
    LOGERRORF("Load PVR failed: Loading arrays of cubemaps isn't supported.");
    return nullptr;
  }

  uint32_t width = header.mWidth;
  uint32_t height = header.mHeight;
  uint32_t depth = header.mDepth;
  uint32_t slices = header.mNumSurfaces * header.mNumFaces;
  uint32_t mipMapCount = header.mNumMipMaps;
  Image_Format format = Image_Format_UNDEFINED;

  bool isSrgb = (header.mColorSpace == 1);

  switch (header.mPixelFormat) {
    case 0:format = isSrgb ? Image_Format_PVR_2BPP_SRGB_BLOCK : Image_Format_PVR_2BPP_BLOCK;
      break;
    case 1:format = isSrgb ? Image_Format_PVR_2BPPA_SRGB_BLOCK : Image_Format_PVR_2BPPA_BLOCK;
      break;
    case 2:format = isSrgb ? Image_Format_PVR_4BPP_SRGB_BLOCK : Image_Format_PVR_4BPP_BLOCK;
      break;
    case 3:format = isSrgb ? Image_Format_PVR_4BPPA_SRGB_BLOCK : Image_Format_PVR_4BPPA_BLOCK;
      break;
    default:    // NOT SUPPORTED
      LOGERRORF("Load PVR failed: pixel type not supported. ");
      return nullptr;
  }

  // TODO Dean load mipmaps
  // TODO read pvr data so no mipmaps at all for now :(

  // skip the meta data
  file->Seek(header.mMetaDataSize, VFile_SD_Current);

  // Create and extract the pixel data
  Image_ImageHeader *image = Image_Create(width, height, depth, slices, format);

  file->Read(Image_RawDataPtr(image), Image_ByteCountOf(image));
  // TODO we should skip to the end here, but we
  // don't have pack or streams files so no harm yet

  return image;
}

static int stbIoCallbackRead(void *user, char *data, int size) {
  VFile_Handle handle = (VFile_Handle) user;
  return (int) VFile_Read(handle, data, size);
}
static void stbIoCallbackSkip(void *user, int n) {
  VFile_Handle handle = (VFile_Handle) user;
  VFile_Seek(handle, n, VFile_SD_Current);
}
static int stbIoCallbackEof(void *user) {
  VFile_Handle handle = (VFile_Handle) user;
  return VFile_IsEOF(handle);
}

EXTERN_C Image_ImageHeader *Image_LoadLDR(VFile_Handle handle) {

  stbi_io_callbacks callbacks{
      &stbIoCallbackRead,
      &stbIoCallbackSkip,
      &stbIoCallbackEof
  };

  int w = 0, h = 0, cmp = 0, requiredCmp = 0;
  stbi_info_from_callbacks(&callbacks, handle, &w, &h, &cmp);

  if (w == 0 || h == 0 || cmp == 0) {
    return nullptr;
  }

  requiredCmp = cmp;
  if (cmp == 3) {
    requiredCmp = 4;
  }

  Image_Format format = Image_Format_UNDEFINED;
  uint64_t memoryRequirement = sizeof(stbi_uc) * w * h * requiredCmp;

  switch (requiredCmp) {
    case 1: format = Image_Format_R8_UNORM;
      break;
    case 2: format = Image_Format_R8G8_UNORM;
      break;
    case 3: format = Image_Format_R8G8B8_UNORM;
      break;
    case 4: format = Image_Format_R8G8B8A8_UNORM;
      break;
  }

  stbi_uc *uncompressed = stbi_load_from_callbacks(&callbacks, handle, &w, &h, &cmp, requiredCmp);
  if (uncompressed == nullptr) {
    return nullptr;
  }

  // Create and extract the pixel data
  Image_ImageHeader *image = Image_Create(w, h, 1, 1, format);
  ASSERT(memoryRequirement == Image_ByteCountOf(image));

  memcpy(Image_RawDataPtr(image), uncompressed, memoryRequirement);
  stbi_image_free(uncompressed);
  return image;
}

EXTERN_C Image_ImageHeader *Image_LoadHDR(VFile_Handle handle) {

  stbi_io_callbacks callbacks{
      &stbIoCallbackRead,
      &stbIoCallbackSkip,
      &stbIoCallbackEof
  };

  int w = 0, h = 0, cmp = 0, requiredCmp = 0;
  stbi_info_from_callbacks(&callbacks, handle, &w, &h, &cmp);

  if (w == 0 || h == 0 || cmp == 0) {
    return nullptr;
  }

  requiredCmp = cmp;

  Image_Format format = Image_Format_UNDEFINED;
  switch (requiredCmp) {
    case 1: format = Image_Format_R32_SFLOAT;
      break;
    case 2: format = Image_Format_R32G32_SFLOAT;
      break;
    case 3: format = Image_Format_R32G32B32_SFLOAT;
      break;
    case 4: format = Image_Format_R32G32B32A32_SFLOAT;
      break;
  }

  uint64_t memoryRequirement = sizeof(float) * w * h * requiredCmp;

  float *uncompressed = stbi_loadf_from_callbacks(&callbacks, handle, &w, &h, &cmp, requiredCmp);
  if (uncompressed == nullptr) {
    return nullptr;
  }

  // Create and extract the pixel data
  Image_ImageHeader *image = Image_Create(w, h, 1, 1, format);
  ASSERT(memoryRequirement == Image_ByteCountOf(image));

  memcpy(Image_RawDataPtr(image), uncompressed, memoryRequirement);
  stbi_image_free(uncompressed);
  return image;
}

EXTERN_C Image_ImageHeader *Image_LoadEXR(VFile_Handle handle) {
  VFile::File *file = VFile::File::FromHandle(handle);

  using namespace tinyexr;
  EXRVersion version;
  EXRHeader header;
  InitEXRHeader(&header);
  int ret = 0;
  ret = ParseEXRVersion(&version, handle);
  if (ret != 0) {
    LOGERRORF("Parse EXR error");
    return nullptr;
  }

  file->Seek(0, VFile_SD_Begin);
  ret = ParseEXRHeader(&header, &version, handle);
  if (ret != 0) {
    LOGERRORF("Parse EXR error");
    return nullptr;
  }

  // all support homogenous image (all formats the same)
  int firstPixelType;
  for (int i = 0; i < Math_MinI32(header.num_channels,4); i++) {
    if(i == 0) {
      firstPixelType = header.pixel_types[0];
    } else {
      if(header.pixel_types[i] != firstPixelType) {
        LOGERROR("EXR image not homogenous");
        return nullptr;
      }
    }
  }

  EXRImage exrImage;
  InitEXRImage(&exrImage);

  ret = LoadEXRImage(&exrImage, &header, handle);
  if (ret != 0) {
    LOGERRORF("Load EXR error\n");
    return nullptr;
  }

  // RGBA
  int idxR = -1;
  int idxG = -1;
  int idxB = -1;
  int idxA = -1;
  int numChannels = 0;
  for (int c = 0; c < header.num_channels; c++) {
    if (strcmp(header.channels[c].name, "R") == 0) {
      idxR = c;
      numChannels++;
    } else if (strcmp(header.channels[c].name, "G") == 0) {
      idxG = c;
      numChannels++;
    } else if (strcmp(header.channels[c].name, "B") == 0) {
      idxB = c;
      numChannels++;
    } else if (strcmp(header.channels[c].name, "A") == 0) {
      idxA = c;
      numChannels++;
    }
  }

  int idxChannels[] = {-1, -1, -1, -1};
  int idxCur = 0;
  if (idxR != -1) {
    idxChannels[idxCur++] = idxR;
  }
  if (idxG != -1) {
    idxChannels[idxCur++] = idxG;
  }
  if (idxB != -1) {
    idxChannels[idxCur++] = idxB;
  }
  if (idxA != -1) {
    idxChannels[idxCur++] = idxA;
  }

  Image_Format format = Image_Format_UNDEFINED;

  if(firstPixelType == TINYEXR_PIXELTYPE_FLOAT) {
    switch (numChannels) {
      case 1: format = Image_Format_R32_SFLOAT;
        break;
      case 2: format = Image_Format_R32G32_SFLOAT;
        break;
      case 3:format = Image_Format_R32G32B32_SFLOAT;
        break;
      case 4: format = Image_Format_R32G32B32A32_SFLOAT;
        break;
    }
  } else if(firstPixelType == TINYEXR_PIXELTYPE_HALF) {
    switch (numChannels) {
      case 1: format = Image_Format_R16_SFLOAT;
        break;
      case 2: format = Image_Format_R16G16_SFLOAT;
        break;
      case 3:format = Image_Format_R16G16B16_SFLOAT;
        break;
      case 4: format = Image_Format_R16G16B16A16_SFLOAT;
        break;
    }
  } else {
    LOGERROR("EXR unsupported pixel type");
    return nullptr;
  }

  // Create and extract the pixel data
  Image_ImageHeader *image = Image_Create(exrImage.width, exrImage.height, 1, 1, format);

  if(firstPixelType == TINYEXR_PIXELTYPE_FLOAT) {
    float* out = (float*)Image_RawDataPtr(image);

    for (int i = 0; i < image->width * image->height; i++) {
      for (int chn = 0; chn < numChannels; chn++) {
        out[i * numChannels + chn] = ((float**) exrImage.images)[idxChannels[chn]][i];
      }
    }
  } else if(firstPixelType == TINYEXR_PIXELTYPE_HALF) {
    uint16_t* out = (uint16_t*)Image_RawDataPtr(image);

    for (int i = 0; i < image->width * image->height; i++) {
      for (int chn = 0; chn < numChannels; chn++) {
        out[i * numChannels + chn] = ((uint16_t**) exrImage.images)[idxChannels[chn]][i];
      }
    }
  }

  return image;
}

