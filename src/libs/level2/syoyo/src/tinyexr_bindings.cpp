#include <vfile/memory.h>
#include "core/core.h"
#include "syoyo/tiny_exr.h"
#include "tinyexr.hpp"

EXTERN_C void TinyExr_InitEXRHeader(TinyExr_EXRHeader *exr_header) {
  if (exr_header == NULL) {
    return;
  }

  memset(exr_header, 0, sizeof(TinyExr_EXRHeader));
}

EXTERN_C void TinyExr_InitEXRImage(TinyExr_EXRImage *exr_image) {
  if (exr_image == NULL) {
    return;
  }

  exr_image->width = 0;
  exr_image->height = 0;
  exr_image->num_channels = 0;

  exr_image->images = NULL;
  exr_image->tiles = NULL;

  exr_image->num_tiles = 0;
}

EXTERN_C int TinyExr_FreeEXRHeader(TinyExr_EXRHeader *exr_header) {
  if (exr_header == NULL) {
    return TINYEXR_ERROR_INVALID_ARGUMENT;
  }

  if (exr_header->channels) {
    free(exr_header->channels);
  }

  if (exr_header->pixel_types) {
    free(exr_header->pixel_types);
  }

  if (exr_header->requested_pixel_types) {
    free(exr_header->requested_pixel_types);
  }

  for (int i = 0; i < exr_header->num_custom_attributes; i++) {
    if (exr_header->custom_attributes[i].value) {
      free(exr_header->custom_attributes[i].value);
    }
  }

  if (exr_header->custom_attributes) {
    free(exr_header->custom_attributes);
  }

  return TINYEXR_SUCCESS;
}

EXTERN_C int TinyExr_FreeEXRImage(TinyExr_EXRImage *exr_image) {
  if (exr_image == NULL) {
    return TINYEXR_ERROR_INVALID_ARGUMENT;
  }

  for (int i = 0; i < exr_image->num_channels; i++) {
    if (exr_image->images && exr_image->images[i]) {
      free(exr_image->images[i]);
    }
  }

  if (exr_image->images) {
    free(exr_image->images);
  }

  if (exr_image->tiles) {
    for (int tid = 0; tid < exr_image->num_tiles; tid++) {
      for (int i = 0; i < exr_image->num_channels; i++) {
        if (exr_image->tiles[tid].images && exr_image->tiles[tid].images[i]) {
          free(exr_image->tiles[tid].images[i]);
        }
      }
      if (exr_image->tiles[tid].images) {
        free(exr_image->tiles[tid].images);
      }
    }
    free(exr_image->tiles);
  }

  return TINYEXR_SUCCESS;
}

EXTERN_C int TinyExr_ParseEXRVersion(TinyExr_EXRVersion *version, VFile_Handle handle) {
  if (version == NULL) {
    LOGERROR("Invalid argument");
    return TINYEXR_ERROR_INVALID_ARGUMENT;
  }

  if (!handle) {
    LOGERRORF("Cannot read file %s", VFile_GetName(handle));
    return TINYEXR_ERROR_CANT_OPEN_FILE;
  }

  using namespace tinyexr;

  size_t const filesize = VFile_Size(handle);
  if (filesize < kEXRVersionSize) {
    LOGERRORF("File size too short %s",VFile_GetName(handle));
    return TINYEXR_ERROR_INVALID_FILE;
  }

  uint8_t* buf = nullptr;
  // if a memory vfile we can short and save memory
  if(VFile_GetType(handle) == VFile_Type_Memory) {
    VFile_MemFile_t* memFile = (VFile_MemFile_t*) VFile_GetTypeSpecificData(handle);
    buf = ((uint8_t*) memFile->memory) + memFile->offset;
  } else {
    buf = (uint8_t * )malloc(kEXRVersionSize);
    size_t ret;
    ret = VFile_Read(handle, buf, kEXRVersionSize);
    ASSERT(ret == kEXRVersionSize);
  }

  int rete = ParseEXRVersionFromMemory(version, buf, kEXRVersionSize);

  if(VFile_GetType(handle) != VFile_Type_Memory) {
    free(buf);
  }
  return rete;
}

EXTERN_C int TinyExr_ParseEXRHeader(TinyExr_EXRHeader *header,
                                    const TinyExr_EXRVersion *version,
                                    VFile_Handle handle) {
  if (header == NULL || version == NULL) {
    LOGERROR("Invalid argument");
    return TINYEXR_ERROR_INVALID_ARGUMENT;
  }

  if (!handle) {
    LOGERRORF("Cannot read file %s", VFile_GetName(handle));
    return TINYEXR_ERROR_CANT_OPEN_FILE;
  }

  using namespace tinyexr;

  size_t const filesize = VFile_Size(handle);
  if (filesize < 16) {
    LOGERRORF("File size too short %s",VFile_GetName(handle));
    return TINYEXR_ERROR_INVALID_FILE;
  }

  uint8_t* buf = nullptr;
  // if a memory vfile we can short and save memory
  if(VFile_GetType(handle) == VFile_Type_Memory) {
    VFile_MemFile_t* memFile = (VFile_MemFile_t*) VFile_GetTypeSpecificData(handle);
    buf = ((uint8_t*) memFile->memory) + memFile->offset;
  } else {
    buf = (uint8_t * )malloc(filesize);
    size_t ret;
    ret = VFile_Read(handle, buf, filesize);
    ASSERT(ret <= filesize);
  }

  int rete = ParseEXRHeaderFromMemory(header, version, buf, filesize);

  if(VFile_GetType(handle) != VFile_Type_Memory) {
    free(buf);
  }
  return rete;
}

EXTERN_C int TinyExr_ParseEXRMultipartHeader(TinyExr_EXRHeader ***headers,
                                             int *num_headers,
                                             const TinyExr_EXRVersion *version,
                                             VFile_Handle handle) {
  if (headers == NULL || num_headers == NULL || version == NULL) {
    LOGERROR("Invalid argument");
    return TINYEXR_ERROR_INVALID_ARGUMENT;
  }

  if (!handle) {
    LOGERRORF("Cannot read file %s", VFile_GetName(handle));
    return TINYEXR_ERROR_CANT_OPEN_FILE;
  }

  using namespace tinyexr;

  size_t const filesize = VFile_Size(handle);
  if (filesize < 16) {
    LOGERRORF("File size too short %s",VFile_GetName(handle));
    return TINYEXR_ERROR_INVALID_FILE;
  }

  uint8_t* buf = nullptr;
  // if a memory vfile we can short and save memory
  if(VFile_GetType(handle) == VFile_Type_Memory) {
    VFile_MemFile_t* memFile = (VFile_MemFile_t*) VFile_GetTypeSpecificData(handle);
    buf = ((uint8_t*) memFile->memory) + memFile->offset;
  } else {
    buf = (uint8_t * )malloc(filesize);
    size_t ret;
    ret = VFile_Read(handle, buf, filesize);
    ASSERT(ret <= filesize);
  }

  int rete = ParseEXRMultipartHeaderFromMemory(headers, num_headers, version, buf, filesize);

  if(VFile_GetType(handle) != VFile_Type_Memory) {
    free(buf);
  }
  return rete;

}

EXTERN_C int TinyExr_LoadEXRImage(TinyExr_EXRImage *exr_image,
                                  const TinyExr_EXRHeader *exr_header,
                                  VFile_Handle handle) {
  if (exr_image == NULL) {
    LOGERROR("Invalid argument for LoadEXRImageFromFile");
    return TINYEXR_ERROR_INVALID_ARGUMENT;
  }

  if (!handle) {
    LOGERRORF("Cannot read file %s", VFile_GetName(handle));
    return TINYEXR_ERROR_CANT_OPEN_FILE;
  }

  using namespace tinyexr;

  size_t const filesize = VFile_Size(handle);
  if (filesize < 16) {
    LOGERRORF("File size too short %s",VFile_GetName(handle));
    return TINYEXR_ERROR_INVALID_FILE;
  }

  uint8_t* buf = nullptr;
  // if a memory vfile we can short and save memory
  if(VFile_GetType(handle) == VFile_Type_Memory) {
    VFile_MemFile_t* memFile = (VFile_MemFile_t*) VFile_GetTypeSpecificData(handle);
    buf = ((uint8_t*) memFile->memory) + memFile->offset;
  } else {
    buf = (uint8_t * )malloc(filesize);
    size_t ret;
    ret = VFile_Read(handle, buf, filesize);
    ASSERT(ret <= filesize);
  }

  int rete = tinyexr::LoadEXRImageFromMemory(exr_image, exr_header, buf, filesize);
  free(buf);
  return rete;
}

EXTERN_C int TinyExr_LoadEXRMultipartImage(
    TinyExr_EXRImage *exr_images,
    const TinyExr_EXRHeader **exr_headers,
    unsigned int num_parts, VFile_Handle handle) {
  if (exr_images == NULL || exr_headers == NULL || num_parts == 0) {
    LOGERROR("Invalid argument ");
    return TINYEXR_ERROR_INVALID_ARGUMENT;
  }

  if (!handle) {
    LOGERRORF("Cannot read file %s", VFile_GetName(handle));
    return TINYEXR_ERROR_CANT_OPEN_FILE;
  }

  size_t const filesize = VFile_Size(handle);

  uint8_t* buf = nullptr;
  // if a memory vfile we can short and save memory
  if(VFile_GetType(handle) == VFile_Type_Memory) {
    VFile_MemFile_t* memFile = (VFile_MemFile_t*) VFile_GetTypeSpecificData(handle);
    buf = ((uint8_t*) memFile->memory) + memFile->offset;
  } else {
    buf = (uint8_t * )malloc(filesize);
    size_t ret;
    ret = VFile_Read(handle, buf, filesize);
    ASSERT(ret <= filesize);
  }

  return tinyexr::LoadEXRMultipartImageFromMemory(exr_images, exr_headers, num_parts,
                                                 buf, filesize);
}

EXTERN_C int TinyExr_SaveEXRImage(
    const TinyExr_EXRImage *exr_image,
    const TinyExr_EXRHeader *exr_header, VFile_Handle handle) {
  if (exr_image == NULL || handle == NULL ||
      exr_header->compression_type < 0) {
    LOGERROR("Invalid argument for SaveEXRImageToFile");
    return TINYEXR_ERROR_INVALID_ARGUMENT;
  }

#if !TINYEXR_USE_PIZ
  if (exr_header->compression_type == TINYEXR_COMPRESSIONTYPE_PIZ) {
    LOGERROR("PIZ compression is not supported in this build");
    return TINYEXR_ERROR_UNSUPPORTED_FEATURE;
  }
#endif

#if !TINYEXR_USE_ZFP
  if (exr_header->compression_type == TINYEXR_COMPRESSIONTYPE_ZFP) {
    LOGERROR("ZFP compression is not supported in this build");
    return TINYEXR_ERROR_UNSUPPORTED_FEATURE;
  }
#endif

  unsigned char *mem = NULL;
  size_t mem_size = tinyexr::SaveEXRImageToMemory(exr_image, exr_header, &mem);
  if (mem_size == 0) {
    return TINYEXR_ERROR_SERIALZATION_FAILED;
  }

  size_t written_size = 0;
  if ((mem_size > 0) && mem) {
    written_size = VFile_Write(handle, mem, mem_size);
  }
  free(mem);

  VFile_Close(handle);

  if (written_size != mem_size) {
    LOGERROR("Cannot write a file");
    return TINYEXR_ERROR_CANT_WRITE_FILE;
  }

  return TINYEXR_SUCCESS;
}

