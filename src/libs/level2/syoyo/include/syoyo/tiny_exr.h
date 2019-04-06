#pragma once
#ifndef WYRD_SYOYO_TINY_EXR_H
#define WYRD_SYOYO_TINY_EXR_H

#include "core/core.h"
#include "vfile/vfile.h"

// @note { OpenEXR file format: http://www.openexr.com/openexrfilelayout.pdf }
enum {
  TINYEXR_SUCCESS = (0),
  TINYEXR_ERROR_INVALID_MAGIC_NUMBER = (-1),
  TINYEXR_ERROR_INVALID_EXR_VERSION = (-2),
  TINYEXR_ERROR_INVALID_ARGUMENT = (-3),
  TINYEXR_ERROR_INVALID_DATA = (-4),
  TINYEXR_ERROR_INVALID_FILE = (-5),
  TINYEXR_ERROR_INVALID_PARAMETER = (-5),
  TINYEXR_ERROR_CANT_OPEN_FILE = (-6),
  TINYEXR_ERROR_UNSUPPORTED_FORMAT = (-7),
  TINYEXR_ERROR_INVALID_HEADER = (-8),
  TINYEXR_ERROR_UNSUPPORTED_FEATURE = (-9),
  TINYEXR_ERROR_CANT_WRITE_FILE = (-10),
  TINYEXR_ERROR_SERIALZATION_FAILED = (-11),

  // pixel type: possible values are: UINT = 0 HALF = 1 FLOAT = 2
  TINYEXR_PIXELTYPE_UINT = (0),
  TINYEXR_PIXELTYPE_HALF = (1),
  TINYEXR_PIXELTYPE_FLOAT = (2),

  TINYEXR_MAX_HEADER_ATTRIBUTES = (1024),
  TINYEXR_MAX_CUSTOM_ATTRIBUTES = (128),

  TINYEXR_COMPRESSIONTYPE_NONE = (0),
  TINYEXR_COMPRESSIONTYPE_RLE = (1),
  TINYEXR_COMPRESSIONTYPE_ZIPS = (2),
  TINYEXR_COMPRESSIONTYPE_ZIP = (3),
  TINYEXR_COMPRESSIONTYPE_PIZ = (4),
  TINYEXR_COMPRESSIONTYPE_ZFP = (128),  // TinyEXR extension

  TINYEXR_ZFP_COMPRESSIONTYPE_RATE = (0),
  TINYEXR_ZFP_COMPRESSIONTYPE_PRECISION = (1),
  TINYEXR_ZFP_COMPRESSIONTYPE_ACCURACY = (2),

  TINYEXR_TILE_ONE_LEVEL = (0),
  TINYEXR_TILE_MIPMAP_LEVELS = (1),
  TINYEXR_TILE_RIPMAP_LEVELS = (2),

  TINYEXR_TILE_ROUND_DOWN = (0),
  TINYEXR_TILE_ROUND_UP = (1),
};

typedef struct _TinyExr_EXRVersion {
  int version;    // this must be 2
  int tiled;      // tile format image
  int long_name;  // long name attribute
  int non_image;  // deep image(EXR 2.0)
  int multipart;  // multi-part(EXR 2.0)
} TinyExr_EXRVersion;

typedef struct _TinyExr_EXRAttribute {
  char name[256];  // name and type are up to 255 chars long.
  char type[256];
  unsigned char *value;  // uint8_t*
  int size;
  int pad0;
} TinyExr_EXRAttribute;

typedef struct _TinyExr_EXRChannelInfo {
  char name[256];  // less than 255 bytes long
  int pixel_type;
  int x_sampling;
  int y_sampling;
  unsigned char p_linear;
  unsigned char pad[3];
} TinyExr_EXRChannelInfo;

typedef struct _TinyExr_EXRTile {
  int offset_x;
  int offset_y;
  int level_x;
  int level_y;

  int width;   // actual width in a tile.
  int height;  // actual height int a tile.

  unsigned char **images;  // image[channels][pixels]
} TinyExr_EXRTile;

typedef struct _TinyExr_EXRHeader {
  float pixel_aspect_ratio;
  int line_order;
  int data_window[4];
  int display_window[4];
  float screen_window_center[2];
  float screen_window_width;

  int chunk_count;

  // Properties for tiled format(`tiledesc`).
  int tiled;
  int tile_size_x;
  int tile_size_y;
  int tile_level_mode;
  int tile_rounding_mode;

  int long_name;
  int non_image;
  int multipart;
  unsigned int header_len;

  // Custom attributes(exludes required attributes(e.g. `channels`,
  // `compression`, etc)
  int num_custom_attributes;
  TinyExr_EXRAttribute *custom_attributes;  // array of EXRAttribute. size =
  // `num_custom_attributes`.

  TinyExr_EXRChannelInfo *channels;  // [num_channels]

  int *pixel_types;  // Loaded pixel type(TINYEXR_PIXELTYPE_*) of `images` for
  // each channel. This is overwritten with `requested_pixel_types` when
  // loading.
  int num_channels;

  int compression_type;        // compression type(TINYEXR_COMPRESSIONTYPE_*)
  int *requested_pixel_types;  // Filled initially by
  // ParseEXRHeaderFrom(Meomory|File), then users
  // can edit it(only valid for HALF pixel type
  // channel)

} TinyExr_EXRHeader;

typedef struct _TinyExr_EXRMultiPartHeader {
  int num_headers;
  TinyExr_EXRHeader *headers;

} TinyExr_EXRMultiPartHeader;

typedef struct _TinyExr_EXRImage {
  TinyExr_EXRTile *tiles;  // Tiled pixel data. The application must reconstruct image
  // from tiles manually. NULL if scanline format.
  unsigned char **images;  // image[channels][pixels]. NULL if tiled format.

  int width;
  int height;
  int num_channels;

  // Properties for tile format.
  int num_tiles;

} TinyExr_EXRImage;

typedef struct _TinyExr_EXRMultiPartImage {
  int num_images;
  TinyExr_EXRImage *images;

} TinyExr_EXRMultiPartImage;

typedef struct _TinyExr_DeepImage {
  const char **channel_names;
  float ***image;      // image[channels][scanlines][samples]
  int **offset_table;  // offset_table[scanline][offsets]
  int num_channels;
  int width;
  int height;
  int pad0;
} TinyExr_DeepImage;

// Initialize EXRHeader struct
EXTERN_C void TinyExr_InitEXRHeader(TinyExr_EXRHeader *exr_header);

// Initialize EXRImage struct
EXTERN_C void TinyExr_InitEXRImage(TinyExr_EXRImage *exr_image);

// Free's internal data of EXRHeader struct
EXTERN_C int TinyExr_FreeEXRHeader(TinyExr_EXRHeader *exr_header);

// Free's internal data of EXRImage struct
EXTERN_C int TinyExr_FreeEXRImage(TinyExr_EXRImage *exr_image);

// Parse EXR version header of a VFile.
EXTERN_C int TinyExr_ParseEXRVersion(TinyExr_EXRVersion *version, VFile_Handle handle);

// Parse single-part OpenEXR header from a file and initialize `EXRHeader`.
EXTERN_C int TinyExr_ParseEXRHeader(TinyExr_EXRHeader *header,
                                    const TinyExr_EXRVersion *version,
                                    VFile_Handle handle);

// Parse multi-part OpenEXR headers from a file and initialize `EXRHeader*`
// array.
EXTERN_C int TinyExr_ParseEXRMultipartHeader(TinyExr_EXRHeader ***headers,
                                             int *num_headers,
                                             const TinyExr_EXRVersion *version,
                                             VFile_Handle handle);

// Loads single-part OpenEXR image from a file.
// Application must setup `ParseEXRHeaderFromFile` before calling this function.
// Application can free EXRImage using `FreeEXRImage`
// Returns negative value and may set error string in `err` when there's an
// error
EXTERN_C int TinyExr_LoadEXRImage(TinyExr_EXRImage *image, const TinyExr_EXRHeader *header,
                                  VFile_Handle handle);


// Loads multi-part OpenEXR image from a file.
// Application must setup `ParseEXRMultipartHeaderFromFile` before calling this
// function.
// Application can free EXRImage using `FreeEXRImage`
// Returns negative value and may set error string in `err` when there's an
// error
EXTERN_C int TinyExr_LoadEXRMultipartImage(TinyExr_EXRImage *images,
                                           const TinyExr_EXRHeader **headers,
                                           unsigned int num_parts,
                                           VFile_Handle handle);

// Saves multi-channel, single-frame OpenEXR image to a file.
// Returns negative value and may set error string in `err` when there's an
// error
EXTERN_C int TinyExr_SaveEXRImage(const TinyExr_EXRImage *image,
                                  const TinyExr_EXRHeader *exr_header,
                                  VFile_Handle handle);

// Loads single-frame OpenEXR deep image.
// Application must free memory of variables in DeepImage(image, offset_table)
// Returns negative value and may set error string in `err` when there's an
// error
EXTERN_C int TinyExr_LoadDeepEXR(TinyExr_DeepImage *out_image,
                                 VFile_Handle handle);

#endif //WYRD_TINY_EXR_H
