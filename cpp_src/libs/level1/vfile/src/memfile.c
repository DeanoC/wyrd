
#include "core/core.h"
#include "core/logger.h"
#include "os/file.h"
#include "vfile/vfile.h"
#include "vfile/interface.h"
#include "memfile.h"

static void VFile_MemFile_Close(VFile_Interface_t *vif) {
  VFile_MemFile_t *vof = (VFile_MemFile_t *) (vif + 1);
  if (vof->takeOwnership) {
    free(vof->memory);
  }
}

static void VFile_MemFile_Flush(VFile_Interface_t *vof) {
  // do nothing
}

static size_t VFile_MemFile_Read(VFile_Interface_t *vif, void *buffer, size_t byteCount) {
  VFile_MemFile_t *vof = (VFile_MemFile_t *) (vif + 1);
  size_t size = byteCount;

  if (vof->offset + byteCount >= vof->size) {
    size = vof->size - vof->offset;
    if (size < 0 || size > vof->size) {
      return 0;
    }
  }
  memcpy(buffer, ((uint8_t *) vof->memory) + vof->offset, size);
  vof->offset += size;
  return size;
}

static size_t VFile_MemFile_Write(VFile_Interface_t *vif, void const *buffer, size_t byteCount) {
  VFile_MemFile_t *vof = (VFile_MemFile_t *) (vif + 1);
  size_t size = byteCount;

  if (vof->offset + byteCount >= vof->size) {
    size = vof->size - vof->offset;
    if (size < 0 || size > vof->size) {
      return 0;
    }
  }
  memcpy(((uint8_t *) vof->memory) + vof->offset, buffer, size);
  vof->offset += size;
  return size;
}

static bool VFile_MemFile_Seek(VFile_Interface_t *vif, int64_t offset, enum VFile_SeekDir origin) {
  VFile_MemFile_t *vof = (VFile_MemFile_t *) (vif + 1);

  size_t voff = 0;
  switch (origin) {
    case VFile_SD_Begin: voff = 0;
      break;
    case VFile_SD_Current: voff = vof->offset;
      break;
    case VFile_SD_End: voff = vof->size;
      break;
    default:return false;
  }

  if (voff + offset < 0) {
    vof->offset = 0;
    return false;
  } else if (voff + offset < vof->size) {
    vof->offset = voff + offset;
    return true;
  } else {
    vof->offset = vof->size;
    return false;
  }
}

static int64_t VFile_MemFile_Tell(VFile_Interface_t *vif) {
  VFile_MemFile_t *vof = (VFile_MemFile_t *) (vif + 1);
  return (int64_t) vof->offset;
}

static size_t VFile_MemFile_Size(VFile_Interface_t *vif) {
  VFile_MemFile_t *vof = (VFile_MemFile_t *) (vif + 1);
  return vof->size;
}

EXTERN_C VFile_Handle VFile_FromMemory(void *memory, size_t size, bool takeOwnership) {

  static const uint32_t mallocSize =
      sizeof(VFile_Interface_t) +
          sizeof(VFile_MemFile_t);

  VFile_Interface_t *vif = (VFile_Interface_t *) malloc(mallocSize);
  VFile_MemFile_t *vof = (VFile_MemFile_t *) (vif + 1);
  vif->magic = InterfaceMagic;
  vif->closeFunc = &VFile_MemFile_Close;
  vif->flushFunc = &VFile_MemFile_Flush;
  vif->readFunc = &VFile_MemFile_Read;
  vif->writeFunc = &VFile_MemFile_Write;
  vif->seekFunc = &VFile_MemFile_Seek;
  vif->tellFunc = &VFile_MemFile_Tell;
  vif->sizeFunc = &VFile_MemFile_Size;
  vof->memory = memory;
  vof->size = size;
  vof->takeOwnership = takeOwnership;
  vof->offset = 0;

  return (VFile_Handle) vif;
}
