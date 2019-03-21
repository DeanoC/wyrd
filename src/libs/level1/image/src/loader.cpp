#include "core/core.h"
#include "core/logger.h"
#include "math/math.h"
#include "stb/stb_image.h"
#include "vfile/vfile.hpp"
#include "image/format.h"
#include "image/format_cracker.h"
#include "image/image.h"
#include <float.h>

/************************************************************************************/
// Define some useful macros
#define MCHAR2(a, b) (a | (b << 8))
#define MAKE_CHAR4(a, b, c, d) (a | (b << 8) | (c << 16) | (d << 24))

// --- IMAGE HEADERS ---
#pragma pack(push, 1)

#define DDPF_ALPHAPIXELS 0x00000001
#define DDPF_FOURCC 0x00000004
#define DDPF_RGB 0x00000040

#define DDSD_CAPS 0x00000001
#define DDSD_HEIGHT 0x00000002
#define DDSD_WIDTH 0x00000004
#define DDSD_PITCH 0x00000008
#define DDSD_PIXELFORMAT 0x00001000
#define DDSD_MIPMAPCOUNT 0x00020000
#define DDSD_LINEARSIZE 0x00080000
#define DDSD_DEPTH 0x00800000

#define DDSCAPS_COMPLEX 0x00000008
#define DDSCAPS_TEXTURE 0x00001000
#define DDSCAPS_MIPMAP 0x00400000

#define DDSCAPS2_CUBEMAP 0x00000200
#define DDSCAPS2_VOLUME 0x00200000

#define DDSCAPS2_CUBEMAP_POSITIVEX 0x00000400
#define DDSCAPS2_CUBEMAP_NEGATIVEX 0x00000800
#define DDSCAPS2_CUBEMAP_POSITIVEY 0x00001000
#define DDSCAPS2_CUBEMAP_NEGATIVEY 0x00002000
#define DDSCAPS2_CUBEMAP_POSITIVEZ 0x00004000
#define DDSCAPS2_CUBEMAP_NEGATIVEZ 0x00008000
#define DDSCAPS2_CUBEMAP_ALL_FACES                                                                                       \
    (DDSCAPS2_CUBEMAP_POSITIVEX | DDSCAPS2_CUBEMAP_NEGATIVEX | DDSCAPS2_CUBEMAP_POSITIVEY | DDSCAPS2_CUBEMAP_NEGATIVEY | \
     DDSCAPS2_CUBEMAP_POSITIVEZ | DDSCAPS2_CUBEMAP_NEGATIVEZ)

#define D3D10_RESOURCE_MISC_TEXTURECUBE 0x4
#define D3D10_RESOURCE_DIMENSION_BUFFER 1
#define D3D10_RESOURCE_DIMENSION_TEXTURE1D 2
#define D3D10_RESOURCE_DIMENSION_TEXTURE2D 3
#define D3D10_RESOURCE_DIMENSION_TEXTURE3D 4

struct DDSHeader {
  uint32_t mDWMagic;
  uint32_t mDWSize;
  uint32_t mDWFlags;
  uint32_t mDWHeight;
  uint32_t mDWWidth;
  uint32_t mDWPitchOrLinearSize;
  uint32_t mDWDepth;
  uint32_t mDWMipMapCount;
  uint32_t mReserved[11];

  struct {
    uint32_t mDWSize;
    uint32_t mDWFlags;
    uint32_t mDWFourCC;
    uint32_t mDWRGBBitCount;
    uint32_t mDWRBitMask;
    uint32_t mDWGBitMask;
    uint32_t mDWBBitMask;
    uint32_t mDWRGBAlphaBitMask;
  } mPixelFormat;

  struct {
    uint32_t mDWCaps1;
    uint32_t mDWCaps2;
    uint32_t mReserved[2];    //caps3 and caps4
  } mCaps;

  uint32_t mDWReserved2;
};

struct DDSHeaderDX10 {
  uint32_t mDXGIFormat;
  uint32_t mResourceDimension;
  uint32_t mMiscFlag;
  uint32_t mArraySize;
  uint32_t mReserved;
};

// Describes the header of a PVR header-texture
typedef struct PVR_Header_Texture_TAG {
  uint32_t mVersion;
  uint32_t mFlags; //!< Various format flags.
  uint64_t mPixelFormat; //!< The pixel format, 8cc value storing the 4 channel identifiers and their respective sizes.
  uint32_t mColorSpace; //!< The Color Space of the texture, currently either linear RGB or sRGB.
  uint32_t mChannelType; //!< Variable type that the channel is stored in. Supports signed/unsigned int/short/char/float.
  uint32_t mHeight; //!< Height of the texture.
  uint32_t mWidth; //!< Width of the texture.
  uint32_t mDepth; //!< Depth of the texture. (Z-slices)
  uint32_t mNumSurfaces; //!< Number of members in a Texture Array.
  uint32_t mNumFaces; //!< Number of faces in a Cube Map. Maybe be a value other than 6.
  uint32_t mNumMipMaps; //!< Number of MIP Maps in the texture - NB: Includes top level.
  uint32_t mMetaDataSize; //!< Size of the accompanying meta data.
} PVR_Texture_Header;

const unsigned int gPvrtexV3HeaderVersion = 0x03525650;

#pragma pack(pop)

enum DDS_DXGI_FORMAT {
  DDS_DXGI_FORMAT_UNKNOWN,
  DDS_DXGI_FORMAT_R32G32B32A32_TYPELESS,
  DDS_DXGI_FORMAT_R32G32B32A32_FLOAT,
  DDS_DXGI_FORMAT_R32G32B32A32_UINT,
  DDS_DXGI_FORMAT_R32G32B32A32_SINT,
  DDS_DXGI_FORMAT_R32G32B32_TYPELESS,
  DDS_DXGI_FORMAT_R32G32B32_FLOAT,
  DDS_DXGI_FORMAT_R32G32B32_UINT,
  DDS_DXGI_FORMAT_R32G32B32_SINT,
  DDS_DXGI_FORMAT_R16G16B16A16_TYPELESS,
  DDS_DXGI_FORMAT_R16G16B16A16_FLOAT,
  DDS_DXGI_FORMAT_R16G16B16A16_UNORM,
  DDS_DXGI_FORMAT_R16G16B16A16_UINT,
  DDS_DXGI_FORMAT_R16G16B16A16_SNORM,
  DDS_DXGI_FORMAT_R16G16B16A16_SINT,
  DDS_DXGI_FORMAT_R32G32_TYPELESS,
  DDS_DXGI_FORMAT_R32G32_FLOAT,
  DDS_DXGI_FORMAT_R32G32_UINT,
  DDS_DXGI_FORMAT_R32G32_SINT,
  DDS_DXGI_FORMAT_R32G8X24_TYPELESS,
  DDS_DXGI_FORMAT_D32_FLOAT_S8X24_UINT,
  DDS_DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS,
  DDS_DXGI_FORMAT_X32_TYPELESS_G8X24_UINT,
  DDS_DXGI_FORMAT_R10G10B10A2_TYPELESS,
  DDS_DXGI_FORMAT_R10G10B10A2_UNORM,
  DDS_DXGI_FORMAT_R10G10B10A2_UINT,
  DDS_DXGI_FORMAT_R11G11B10_FLOAT,
  DDS_DXGI_FORMAT_R8G8B8A8_TYPELESS,
  DDS_DXGI_FORMAT_R8G8B8A8_UNORM,
  DDS_DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
  DDS_DXGI_FORMAT_R8G8B8A8_UINT,
  DDS_DXGI_FORMAT_R8G8B8A8_SNORM,
  DDS_DXGI_FORMAT_R8G8B8A8_SINT,
  DDS_DXGI_FORMAT_R16G16_TYPELESS,
  DDS_DXGI_FORMAT_R16G16_FLOAT,
  DDS_DXGI_FORMAT_R16G16_UNORM,
  DDS_DXGI_FORMAT_R16G16_UINT,
  DDS_DXGI_FORMAT_R16G16_SNORM,
  DDS_DXGI_FORMAT_R16G16_SINT,
  DDS_DXGI_FORMAT_R32_TYPELESS,
  DDS_DXGI_FORMAT_D32_FLOAT,
  DDS_DXGI_FORMAT_R32_FLOAT,
  DDS_DXGI_FORMAT_R32_UINT,
  DDS_DXGI_FORMAT_R32_SINT,
  DDS_DXGI_FORMAT_R24G8_TYPELESS,
  DDS_DXGI_FORMAT_D24_UNORM_S8_UINT,
  DDS_DXGI_FORMAT_R24_UNORM_X8_TYPELESS,
  DDS_DXGI_FORMAT_X24_TYPELESS_G8_UINT,
  DDS_DXGI_FORMAT_R8G8_TYPELESS,
  DDS_DXGI_FORMAT_R8G8_UNORM,
  DDS_DXGI_FORMAT_R8G8_UINT,
  DDS_DXGI_FORMAT_R8G8_SNORM,
  DDS_DXGI_FORMAT_R8G8_SINT,
  DDS_DXGI_FORMAT_R16_TYPELESS,
  DDS_DXGI_FORMAT_R16_FLOAT,
  DDS_DXGI_FORMAT_D16_UNORM,
  DDS_DXGI_FORMAT_R16_UNORM,
  DDS_DXGI_FORMAT_R16_UINT,
  DDS_DXGI_FORMAT_R16_SNORM,
  DDS_DXGI_FORMAT_R16_SINT,
  DDS_DXGI_FORMAT_R8_TYPELESS,
  DDS_DXGI_FORMAT_R8_UNORM,
  DDS_DXGI_FORMAT_R8_UINT,
  DDS_DXGI_FORMAT_R8_SNORM,
  DDS_DXGI_FORMAT_R8_SINT,
  DDS_DXGI_FORMAT_A8_UNORM,
  DDS_DXGI_FORMAT_R1_UNORM,
  DDS_DXGI_FORMAT_R9G9B9E5_SHAREDEXP,
  DDS_DXGI_FORMAT_R8G8_B8G8_UNORM,
  DDS_DXGI_FORMAT_G8R8_G8B8_UNORM,
  DDS_DXGI_FORMAT_BC1_TYPELESS,
  DDS_DXGI_FORMAT_BC1_UNORM,
  DDS_DXGI_FORMAT_BC1_UNORM_SRGB,
  DDS_DXGI_FORMAT_BC2_TYPELESS,
  DDS_DXGI_FORMAT_BC2_UNORM,
  DDS_DXGI_FORMAT_BC2_UNORM_SRGB,
  DDS_DXGI_FORMAT_BC3_TYPELESS,
  DDS_DXGI_FORMAT_BC3_UNORM,
  DDS_DXGI_FORMAT_BC3_UNORM_SRGB,
  DDS_DXGI_FORMAT_BC4_TYPELESS,
  DDS_DXGI_FORMAT_BC4_UNORM,
  DDS_DXGI_FORMAT_BC4_SNORM,
  DDS_DXGI_FORMAT_BC5_TYPELESS,
  DDS_DXGI_FORMAT_BC5_UNORM,
  DDS_DXGI_FORMAT_BC5_SNORM,
  DDS_DXGI_FORMAT_B5G6R5_UNORM,
  DDS_DXGI_FORMAT_B5G5R5A1_UNORM,
  DDS_DXGI_FORMAT_B8G8R8A8_UNORM,
  DDS_DXGI_FORMAT_B8G8R8X8_UNORM,
  DDS_DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM,
  DDS_DXGI_FORMAT_B8G8R8A8_TYPELESS,
  DDS_DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,
  DDS_DXGI_FORMAT_B8G8R8X8_TYPELESS,
  DDS_DXGI_FORMAT_B8G8R8X8_UNORM_SRGB,
  DDS_DXGI_FORMAT_BC6H_TYPELESS,
  DDS_DXGI_FORMAT_BC6H_UF16,
  DDS_DXGI_FORMAT_BC6H_SF16,
  DDS_DXGI_FORMAT_BC7_TYPELESS,
  DDS_DXGI_FORMAT_BC7_UNORM,
  DDS_DXGI_FORMAT_BC7_UNORM_SRGB,
  DDS_DXGI_FORMAT_AYUV,
  DDS_DXGI_FORMAT_Y410,
  DDS_DXGI_FORMAT_Y416,
  DDS_DXGI_FORMAT_NV12,
  DDS_DXGI_FORMAT_P010,
  DDS_DXGI_FORMAT_P016,
  DDS_DXGI_FORMAT_420_OPAQUE,
  DDS_DXGI_FORMAT_YUY2,
  DDS_DXGI_FORMAT_Y210,
  DDS_DXGI_FORMAT_Y216,
  DDS_DXGI_FORMAT_NV11,
  DDS_DXGI_FORMAT_AI44,
  DDS_DXGI_FORMAT_IA44,
  DDS_DXGI_FORMAT_P8,
  DDS_DXGI_FORMAT_A8P8,
  DDS_DXGI_FORMAT_B4G4R4A4_UNORM,
  DDS_DXGI_FORMAT_P208,
  DDS_DXGI_FORMAT_V208,
  DDS_DXGI_FORMAT_V408,
  DDS_DXGI_FORMAT_FORCE_UINT
};
// Load Image Data form mData functions

Image_ImageHeader* Image_LoadDDS(VFile_Handle handle) {
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

  if(header.mDWMipMapCount != 1) {
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

Image_ImageHeader* Image_LoadPVR(VFile_Handle handle) {
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
  uint32_t mipMapCount =header.mNumMipMaps;
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
  Image_ImageHeader* image = Image_Create(width, height, depth, slices, format);

  file->Read(Image_RawDataPtr(image), Image_ByteCountOf(image));
  // TODO we should skip to the end here, but we
  // don't have pack or streams files so no harm yet

  return image;
}

static int stbIoCallbackRead(void* user, char* data, int size) {
  VFile_Handle handle = (VFile_Handle)user;
  return (int) VFile_Read(handle, data, size);
}
static void stbIoCallbackSkip(void* user, int n) {
  VFile_Handle handle = (VFile_Handle)user;
  VFile_Seek(handle, n, VFile_SD_Current);
}
static int stbIoCallbackEof(void* user) {
  VFile_Handle handle = (VFile_Handle)user;
  return VFile_IsEOF(handle);
}

Image_ImageHeader* Image_LoadSTBI(VFile_Handle handle) {

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
  Image_ImageHeader* image = Image_Create(w, h, 1, 1, format);
  ASSERT(memoryRequirement == Image_ByteCountOf(image));

  memcpy(Image_RawDataPtr(image), uncompressed, memoryRequirement);
  stbi_image_free(uncompressed);
  return image;
}

Image_ImageHeader* Image_LoadSTBIFP32(VFile_Handle handle) {

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
  Image_ImageHeader* image = Image_Create(w, h, 1, 1, format);
  ASSERT(memoryRequirement == Image_ByteCountOf(image));

  memcpy(Image_RawDataPtr(image), uncompressed, memoryRequirement);
  stbi_image_free(uncompressed);
  return image;
}

// TODO Deano
#if 0
bool Image::iLoadEXRFP32FromMemory(
    const char* buffer, uint32_t memSize, const bool useMipmaps, memoryAllocationFunc pAllocator, void* pUserData)
{
  // tinyexr does not generate or load mipmaps. (useMipmaps is ignored)
  if (buffer == 0 || memSize == 0)
    return false;

  const char* err;

  EXRImage exrImage;
  InitEXRImage(&exrImage);

  int ret = ParseMultiChannelEXRHeaderFromMemory(&exrImage, (const unsigned char*)buffer, &err);
  if (ret != 0)
  {
    LOGERRORF("Parse EXR err: %s\n", err);
    return false;
  }

  // Read HALF image as FLOAT.
  for (int i = 0; i < exrImage.num_channels; i++)
  {
    if (exrImage.pixel_types[i] == TINYEXR_PIXELTYPE_HALF)
      exrImage.requested_pixel_types[i] = TINYEXR_PIXELTYPE_FLOAT;
  }

  ret = LoadMultiChannelEXRFromMemory(&exrImage, (const unsigned char*)buffer, &err);
  if (ret != 0)
  {
    LOGERRORF("Load EXR err: %s\n", err);
    return false;
  }

  // RGBA
  int idxR = -1;
  int idxG = -1;
  int idxB = -1;
  int idxA = -1;
  int numChannels = 0;
  for (int c = 0; c < exrImage.num_channels; c++)
  {
    if (strcmp(exrImage.channel_names[c], "R") == 0)
    {
      idxR = c;
      numChannels++;
    }
    else if (strcmp(exrImage.channel_names[c], "G") == 0)
    {
      idxG = c;
      numChannels++;
    }
    else if (strcmp(exrImage.channel_names[c], "B") == 0)
    {
      idxB = c;
      numChannels++;
    }
    else if (strcmp(exrImage.channel_names[c], "A") == 0)
    {
      idxA = c;
      numChannels++;
    }
  }

  int idxChannels[] = { -1, -1, -1, -1 };
  int idxCur = 0;
  if (idxR != -1)
    idxChannels[idxCur++] = idxR;
  if (idxG != -1)
    idxChannels[idxCur++] = idxG;
  if (idxB != -1)
    idxChannels[idxCur++] = idxB;
  if (idxA != -1)
    idxChannels[idxCur++] = idxA;

  unsigned int* out = (unsigned int*)conf_malloc(numChannels * sizeof(float) * exrImage.width * exrImage.height);
  for (int i = 0; i < exrImage.width * exrImage.height; i++)
    for (int chn = 0; chn < numChannels; chn++)
      out[i * numChannels + chn] = ((unsigned int**)exrImage.images)[idxChannels[chn]][i];

  pData = (unsigned char*)out;

  mWidth = exrImage.width;
  mHeight = exrImage.height;
  mDepth = 1;
  mMipMapCount = 1;
  mArrayCount = 1;

  switch (numChannels)
  {
    case 1: mFormat = ImageFormat::R32F; break;
    case 2: mFormat = ImageFormat::RG32F; break;
      // RGB32F format not supported on all APIs so convert to RGBA32F
    case 3:
      mFormat = ImageFormat::RGB32F;
      Convert(ImageFormat::RGBA32F);
      break;
    case 4: mFormat = ImageFormat::RGBA32F; break;
  }

  return true;
}
#endif


/*

bool Image::Convert(const ImageFormat newFormat) {
  uint8_t *newPixels;
  uint32_t nPixels = GetNumberOfPixels(0, mMipMapCount) * mArrayCount;

  if (mFormat == RGBE8 && (newFormat == RGB32F || newFormat == RGBA32F)) {
    newPixels = (uint8_t *) malloc(sizeof(uint8_t) * GetMipMappedSize(0, mMipMapCount, newFormat) * mArrayCount);
    float *dest = (float *) newPixels;

    bool writeAlpha = (newFormat == RGBA32F);
    uint8_t *src = pData;
    do {
      *((vec3_t *) dest) = Math_RGBEToRGB(src);
      if (writeAlpha) {
        dest[3] = 1.0f;
        dest += 4;
      } else {
        dest += 3;
      }
      src += 4;
    } while (--nPixels);
  } else {
    if (!IsPlainFormat(mFormat) || !(IsPlainFormat(newFormat) || newFormat == RGB10A2 ||
        newFormat == RGBE8 || newFormat == RGB9E5)) {
      LOGERRORF(
          "Image: %s fail to convert from  %s  to  %s", mLoadFileName.c_str(), GetImageFormatString(mFormat),
          GetImageFormatString(newFormat));
      return false;
    }
    if (mFormat == newFormat) {
      return true;
    }

    uint8_t *src = pData;
    uint8_t *dest = newPixels = (uint8_t *) malloc(
        sizeof(uint8_t) * GetMipMappedSize(0, mMipMapCount, newFormat) * mArrayCount);

    if (mFormat == RGB8 && newFormat == RGBA8) {
      // Fast path for RGB->RGBA8
      do {
        dest[0] = src[0];
        dest[1] = src[1];
        dest[2] = src[2];
        dest[3] = 255;
        dest += 4;
        src += 3;
      } while (--nPixels);
    } else if (mFormat == RGBA8 && newFormat == BGRA8) {
      // Fast path for RGBA8->BGRA8 (just swizzle)
      do {
        dest[0] = src[2];
        dest[1] = src[1];
        dest[2] = src[0];
        dest[3] = src[3];
        dest += 4;
        src += 4;
      } while (--nPixels);
    } else {
      int srcSize = GetBytesPerPixel(mFormat);
      int nSrcChannels = GetChannelCount(mFormat);

      int destSize = GetBytesPerPixel(newFormat);
      int nDestChannels = GetChannelCount(newFormat);

      do {
        float rgba[4];

        if (IsFloatFormat(mFormat)) {
          if (mFormat <= RGBA16F) {
            for (int i = 0; i < nSrcChannels; i++) {
              rgba[i] = Math_Half2Float(((uint16_t *) src)[i]);
            }
          } else {
            for (int i = 0; i < nSrcChannels; i++) {
              rgba[i] = ((float *) src)[i];
            }
          }
        } else if (mFormat >= I16 && mFormat <= RGBA16) {
          for (int i = 0; i < nSrcChannels; i++) {
            rgba[i] = ((uint16_t *) src)[i] * (1.0f / 65535.0f);
          }
        } else {
          for (int i = 0; i < nSrcChannels; i++) {
            rgba[i] = src[i] * (1.0f / 255.0f);
          }
        }
        if (nSrcChannels < 4) {
          rgba[3] = 1.0f;
        }
        if (nSrcChannels == 1) {
          rgba[2] = rgba[1] = rgba[0];
        }

        if (nDestChannels == 1) {
          rgba[0] = 0.30f * rgba[0] + 0.59f * rgba[1] + 0.11f * rgba[2];
        }

        if (IsFloatFormat(newFormat)) {
          if (newFormat <= RGBA32F) {
            if (newFormat <= RGBA16F) {
              for (int i = 0; i < nDestChannels; i++) {
                ((uint16_t *) dest)[i] = Math_Float2Half(rgba[i]);
              }
            } else {
              for (int i = 0; i < nDestChannels; i++) {
                ((float *) dest)[i] = rgba[i];
              }
            }
          } else {
            if (newFormat == RGBE8) {
              *(uint32_t *) dest = Math_FloatRGBToRGBE8(rgba[0], rgba[1], rgba[2]);
            } else {
              *(uint32_t *) dest = Math_FloatRGBToRGB9E5(rgba[0], rgba[1], rgba[2]);
            }
          }
        } else if (newFormat >= I16 && newFormat <= RGBA16) {
          for (int i = 0; i < nDestChannels; i++) {
            ((uint16_t *) dest)[i] = (uint16_t) (65535 * Math_SaturateF(rgba[i]) + 0.5f);
          }
        } else if (newFormat == RGB10A2) {
          *(uint32_t *) dest =
              (uint32_t(1023.0f * Math_SaturateF(rgba[0]) + 0.5f) << 22) |
                  (uint32_t(1023.0f * Math_SaturateF(rgba[1]) + 0.5f) << 12) |
                  (uint32_t(1023.0f * Math_SaturateF(rgba[2]) + 0.5f) << 2) |
                  (uint32_t(3.0f * Math_SaturateF(rgba[3]) + 0.5f));
        } else {
          for (int i = 0; i < nDestChannels; i++) {
            dest[i] = (unsigned char) (255 * Math_SaturateF(rgba[i]) + 0.5f);
          }
        }

        src += srcSize;
        dest += destSize;
      } while (--nPixels);
    }
  }
  free(pData);
  pData = newPixels;
  mFormat = newFormat;

  return true;
}

template<typename T>
void buildMipMap(T *dst, const T *src, const uint32_t w, const uint32_t h, const uint32_t d, const uint32_t c) {
  uint32_t xOff = (w < 2) ? 0 : c;
  uint32_t yOff = (h < 2) ? 0 : c * w;
  uint32_t zOff = (d < 2) ? 0 : c * w * h;

  for (uint32_t z = 0; z < d; z += 2) {
    for (uint32_t y = 0; y < h; y += 2) {
      for (uint32_t x = 0; x < w; x += 2) {
        for (uint32_t i = 0; i < c; i++) {
          *dst++ =
              (src[0] + src[xOff] + src[yOff] + src[yOff + xOff] + src[zOff] + src[zOff + xOff] + src[zOff + yOff] +
                  src[zOff + yOff + xOff]) /
                  8;
          src++;
        }
        src += xOff;
      }
      src += yOff;
    }
    src += zOff;
  }
}

bool Image::GenerateMipMaps(const uint32_t mipMaps) {
  if (IsCompressedFormat(mFormat)) {
    return false;
  }
  if (!(mWidth) || !Math_IsPowerOf2U32(mHeight) || !Math_IsPowerOf2U32(mDepth)) {
    return false;
  }

  uint32_t actualMipMaps = Math_MinU32(mipMaps, GetMipMapCountFromDimensions());

  if (mMipMapCount != actualMipMaps) {
    int size = GetMipMappedSize(0, actualMipMaps);
    if (mArrayCount > 1) {
      uint8_t *newPixels = (uint8_t *) malloc(sizeof(uint8_t) * size * mArrayCount);

      // Copy top mipmap of all array slices to new location
      int firstMipSize = GetMipMappedSize(0, 1);
      int oldSize = GetMipMappedSize(0, mMipMapCount);

      for (uint32_t i = 0; i < mArrayCount; i++) {
        memcpy(newPixels + i * size, pData + i * oldSize, firstMipSize);
      }

      free(pData);
      pData = newPixels;
    } else {
      pData = (uint8_t *) realloc(pData, size);
    }
    mMipMapCount = actualMipMaps;
  }

  int nChannels = GetChannelCount(mFormat);

  int n = IsCube() ? 6 : 1;

  for (uint32_t arraySlice = 0; arraySlice < mArrayCount; arraySlice++) {
    uint8_t *src = GetPixels(0, arraySlice);
    uint8_t *dst = GetPixels(1, arraySlice);

    for (uint32_t level = 1; level < mMipMapCount; level++) {
      uint32_t w = GetWidth(level - 1);
      uint32_t h = GetHeight(level - 1);
      uint32_t d = GetDepth(level - 1);

      uint32_t srcSize = GetMipMappedSize(level - 1, 1) / n;
      uint32_t dstSize = GetMipMappedSize(level, 1) / n;

      for (uint32_t i = 0; i < n; i++) {
        if (IsPlainFormat(mFormat)) {
          if (IsFloatFormat(mFormat)) {
            buildMipMap((float *) dst, (float *) src, w, h, d, nChannels);
          } else if (mFormat >= I16) {
            buildMipMap((uint16_t *) dst, (uint16_t *) src, w, h, d, nChannels);
          } else {
            buildMipMap(dst, src, w, h, d, nChannels);
          }
        }
        src += srcSize;
        dst += dstSize;
      }
    }
  }

  return true;
}

bool Image::iSwap(const int c0, const int c1) {
  if (!IsPlainFormat(mFormat)) {
    return false;
  }

  unsigned int nPixels = GetNumberOfPixels(0, mMipMapCount) * mArrayCount;
  unsigned int nChannels = GetChannelCount(mFormat);

  if (mFormat <= RGBA8) {
    swapPixelChannels((uint8_t *) pData, nPixels, nChannels, c0, c1);
  } else if (mFormat <= RGBA16F) {
    swapPixelChannels((uint16_t *) pData, nPixels, nChannels, c0, c1);
  } else {
    swapPixelChannels((float *) pData, nPixels, nChannels, c0, c1);
  }

  return true;
}

// -- IMAGE SAVING --

bool Image_SaveDDS(const char *fileName) {
  DDSHeader header;
  DDSHeaderDX10 headerDX10;
  memset(&header, 0, sizeof(header));
  memset(&headerDX10, 0, sizeof(headerDX10));

  header.mDWMagic = MAKE_CHAR4('D', 'D', 'S', ' ');
  header.mDWSize = 124;

  header.mDWWidth = mWidth;
  header.mDWHeight = mHeight;
  header.mDWDepth = (mDepth > 1) ? mDepth : 0;
  header.mDWPitchOrLinearSize = 0;
  header.mDWMipMapCount = (mMipMapCount > 1) ? mMipMapCount : 0;
  header.mPixelFormat.mDWSize = 32;

  header.mDWFlags =
      DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT | (mMipMapCount > 1 ? DDSD_MIPMAPCOUNT : 0)
          | (mDepth > 1 ? DDSD_DEPTH : 0);

  int nChannels = GetChannelCount(mFormat);

  if (mFormat == RGB10A2 || mFormat <= I16) {
    if (mFormat <= RGBA8) {
      header.mPixelFormat.mDWRGBBitCount = 8 * nChannels;
      header.mPixelFormat.mDWRGBAlphaBitMask = (nChannels == 4) ? 0xFF000000 : (nChannels == 2) ? 0xFF00 : 0;
      header.mPixelFormat.mDWRBitMask = (nChannels > 2) ? 0x00FF0000 : 0xFF;
      header.mPixelFormat.mDWGBitMask = (nChannels > 1) ? 0x0000FF00 : 0;
      header.mPixelFormat.mDWBBitMask = (nChannels > 1) ? 0x000000FF : 0;
    } else if (mFormat == I16) {
      header.mPixelFormat.mDWRGBBitCount = 16;
      header.mPixelFormat.mDWRBitMask = 0xFFFF;
    } else {
      header.mPixelFormat.mDWRGBBitCount = 32;
      header.mPixelFormat.mDWRGBAlphaBitMask = 0xC0000000;
      header.mPixelFormat.mDWRBitMask = 0x3FF00000;
      header.mPixelFormat.mDWGBitMask = 0x000FFC00;
      header.mPixelFormat.mDWBBitMask = 0x000003FF;
    }
    header.mPixelFormat.mDWFlags = ((nChannels < 3) ? 0x00020000 : DDPF_RGB) | ((nChannels & 1) ? 0 : DDPF_ALPHAPIXELS);
  } else {
    header.mPixelFormat.mDWFlags = DDPF_FOURCC;

    switch (mFormat) {
      case RG16: header.mPixelFormat.mDWFourCC = 34;
        break;
      case RGBA16: header.mPixelFormat.mDWFourCC = 36;
        break;
      case R16F: header.mPixelFormat.mDWFourCC = 111;
        break;
      case RG16F: header.mPixelFormat.mDWFourCC = 112;
        break;
      case RGBA16F: header.mPixelFormat.mDWFourCC = 113;
        break;
      case R32F: header.mPixelFormat.mDWFourCC = 114;
        break;
      case RG32F: header.mPixelFormat.mDWFourCC = 115;
        break;
      case RGBA32F: header.mPixelFormat.mDWFourCC = 116;
        break;
      case DXT1: header.mPixelFormat.mDWFourCC = MAKE_CHAR4('D', 'X', 'T', '1');
        break;
      case DXT3: header.mPixelFormat.mDWFourCC = MAKE_CHAR4('D', 'X', 'T', '3');
        break;
      case DXT5: header.mPixelFormat.mDWFourCC = MAKE_CHAR4('D', 'X', 'T', '5');
        break;
      case ATI1N: header.mPixelFormat.mDWFourCC = MAKE_CHAR4('A', 'T', 'I', '1');
        break;
      case ATI2N: header.mPixelFormat.mDWFourCC = MAKE_CHAR4('A', 'T', 'I', '2');
        break;
      default:header.mPixelFormat.mDWFourCC = MAKE_CHAR4('D', 'X', '1', '0');
        headerDX10.mArraySize = 1;
        headerDX10.mDXGIFormat = (mDepth == 0) ? D3D10_RESOURCE_MISC_TEXTURECUBE : 0;
        if (Is1D()) {
          headerDX10.mResourceDimension = D3D10_RESOURCE_DIMENSION_TEXTURE1D;
        } else if (Is2D()) {
          headerDX10.mResourceDimension = D3D10_RESOURCE_DIMENSION_TEXTURE2D;
        } else if (Is3D()) {
          headerDX10.mResourceDimension = D3D10_RESOURCE_DIMENSION_TEXTURE3D;
        }

        switch (mFormat) {
          case RGB32F: headerDX10.mDXGIFormat = 6;
            break;
          case RGB9E5: headerDX10.mDXGIFormat = 67;
            break;
          case RG11B10F: headerDX10.mDXGIFormat = 26;
            break;
          default: return false;
        }
    }
  }
  // header.

  header.mCaps.mDWCaps1 =
      DDSCAPS_TEXTURE | (mMipMapCount > 1 ? DDSCAPS_MIPMAP | DDSCAPS_COMPLEX : 0) | (mDepth != 1 ? DDSCAPS_COMPLEX : 0);
  header.mCaps.mDWCaps2 =
      (mDepth > 1) ? DDSCAPS2_VOLUME : (mDepth == 0) ? DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_ALL_FACES : 0;
  header.mCaps.mReserved[0] = 0;
  header.mCaps.mReserved[1] = 0;
  header.mDWReserved2 = 0;

  VFile::ScopedFile file = VFile::File::FromFile(fileName, Os_FM_WriteBinary);
  if (!file) {
    return false;
  }

  file->Write(&header, sizeof(header));

  if (headerDX10.mDXGIFormat) {
    file->Write(&headerDX10, sizeof(headerDX10) * 1);
  }

  int size = GetMipMappedSize(0, mMipMapCount);

  // RGB to BGR
  if (mFormat == RGB8 || mFormat == RGBA8) {
    swapPixelChannels(pData, size / nChannels, nChannels, 0, 2);
  }

  if (IsCube()) {
    for (int face = 0; face < 6; face++) {
      for (uint32_t mipMapLevel = 0; mipMapLevel < mMipMapCount; mipMapLevel++) {
        int faceSize = GetMipMappedSize(mipMapLevel, 1) / 6;
        uint8_t *src = GetPixels(mipMapLevel) + face * faceSize;
        file->Write(src, faceSize);
      }
    }
  } else {
    file->Write(pData, size);
  }
  file->Close();

  // Restore to RGB
  if (mFormat == RGB8 || mFormat == RGBA8) {
    swapPixelChannels(pData, size / nChannels, nChannels, 0, 2);
  }

  return true;
}

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

bool Image_SaveTGA(const char *fileName) {
  switch (mFormat) {
    case R8: return 0 != stbi_write_tga(fileName, mWidth, mHeight, 1, pData);
      break;
    case RG8: return 0 != stbi_write_tga(fileName, mWidth, mHeight, 2, pData);
      break;
    case RGB8: return 0 != stbi_write_tga(fileName, mWidth, mHeight, 3, pData);
      break;
    case RGBA8: return 0 != stbi_write_tga(fileName, mWidth, mHeight, 4, pData);
      break;
    default: {
      // uncompress/convert and try again
      return convertAndSaveImage(*this, &Image::iSaveTGA, fileName);
    }
  }

  //return false; //Unreachable
}

bool Image_SaveBMP(const char *fileName) {
  switch (mFormat) {
    case R8: stbi_write_bmp(fileName, mWidth, mHeight, 1, pData);
      break;
    case RG8: stbi_write_bmp(fileName, mWidth, mHeight, 2, pData);
      break;
    case RGB8: stbi_write_bmp(fileName, mWidth, mHeight, 3, pData);
      break;
    case RGBA8: stbi_write_bmp(fileName, mWidth, mHeight, 4, pData);
      break;
    default: {
      // uncompress/convert and try again
      return convertAndSaveImage(*this, &Image::iSaveBMP, fileName);
    }
  }
  return true;
}

bool Image_SavePNG(const char *fileName) {
  switch (mFormat) {
    case R8: stbi_write_png(fileName, mWidth, mHeight, 1, pData, 0);
      break;
    case RG8: stbi_write_png(fileName, mWidth, mHeight, 2, pData, 0);
      break;
    case RGB8: stbi_write_png(fileName, mWidth, mHeight, 3, pData, 0);
      break;
    case RGBA8: stbi_write_png(fileName, mWidth, mHeight, 4, pData, 0);
      break;
    default: {
      // uncompress/convert and try again
      return convertAndSaveImage(*this, &Image::iSavePNG, fileName);
    }
  }

  return true;
}

bool Image_SaveHDR(const char *fileName) {
  switch (mFormat) {
    case R32F: stbi_write_hdr(fileName, mWidth, mHeight, 1, (float *) pData);
      break;
    case RG32F: stbi_write_hdr(fileName, mWidth, mHeight, 2, (float *) pData);
      break;
    case RGB32F: stbi_write_hdr(fileName, mWidth, mHeight, 3, (float *) pData);
      break;
    case RGBA32F: stbi_write_hdr(fileName, mWidth, mHeight, 4, (float *) pData);
      break;
    default: {
      // uncompress/convert and try again
      return convertAndSaveImage(*this, &Image::iSaveHDR, fileName);
    }
  }

  return true;
}

bool Image_SaveJPG(const char *fileName) {
  switch (mFormat) {
    case R8: stbi_write_jpg(fileName, mWidth, mHeight, 1, pData, 0);
      break;
    case RG8: stbi_write_jpg(fileName, mWidth, mHeight, 2, pData, 0);
      break;
    case RGB8: stbi_write_jpg(fileName, mWidth, mHeight, 3, pData, 0);
      break;
    case RGBA8: stbi_write_jpg(fileName, mWidth, mHeight, 4, pData, 0);
      break;
    default: {
      // uncompress/convert and try again
      return convertAndSaveImage(*this, &Image::iSaveJPG, fileName);
    }
  }

  return true;
}
*/