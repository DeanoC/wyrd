
#include "core/core.h"
#include "core/logger.h"
#include "os/file.h"
#include "vfile/vfile.h"
#include "vfile/interface.h"
#include "osfile.h"

static void VFile_OsFile_Close(VFile_Interface_t *vif) {
  VFile_OsFile_t *vof = (VFile_OsFile_t *) (vif + 1);
  Os_FileClose(vof->fileHandle);
}

static void VFile_OsFile_Flush(VFile_Interface_t *vif) {
  VFile_OsFile_t *vof = (VFile_OsFile_t *) (vif + 1);
  Os_FileFlush(vof->fileHandle);
}

static size_t VFile_OsFile_Read(VFile_Interface_t *vif, void *buffer, size_t byteCount) {
  VFile_OsFile_t *vof = (VFile_OsFile_t *) (vif + 1);
  return Os_FileRead(vof->fileHandle, buffer, byteCount);
}
static size_t VFile_OsFile_Write(VFile_Interface_t *vif, void const *buffer, size_t byteCount) {
  VFile_OsFile_t *vof = (VFile_OsFile_t *) (vif + 1);
  return Os_FileWrite(vof->fileHandle, buffer, byteCount);
}

static bool VFile_OsFile_Seek(VFile_Interface_t *vif, int64_t offset, enum VFile_SeekDir origin) {
  VFile_OsFile_t *vof = (VFile_OsFile_t *) (vif + 1);
  return Os_FileSeek(vof->fileHandle, offset, (enum Os_FileSeekDir) (origin));
}

static int64_t VFile_OsFile_Tell(VFile_Interface_t *vif) {
  VFile_OsFile_t *vof = (VFile_OsFile_t *) (vif + 1);
  return Os_FileTell(vof->fileHandle);
}

static size_t VFile_OsFile_Size(VFile_Interface_t *vif) {
  VFile_OsFile_t *vof = (VFile_OsFile_t *) (vif + 1);
  return Os_FileSize(vof->fileHandle);
}

EXTERN_C VFile_Handle VFile_FromOsFile(Os_FileHandle handle) {

  static const uint32_t mallocSize =
      sizeof(VFile_Interface_t) +
          sizeof(VFile_OsFile_t);

  VFile_Interface_t *vif = (VFile_Interface_t *) malloc(mallocSize);
  VFile_OsFile_t *vof = (VFile_OsFile_t *) (vif + 1);
  vif->magic = InterfaceMagic;
  vof->fileHandle = handle;
  vif->closeFunc = &VFile_OsFile_Close;
  vif->flushFunc = &VFile_OsFile_Flush;
  vif->readFunc = &VFile_OsFile_Read;
  vif->writeFunc = &VFile_OsFile_Write;
  vif->seekFunc = &VFile_OsFile_Seek;
  vif->tellFunc = &VFile_OsFile_Tell;
  vif->sizeFunc = &VFile_OsFile_Size;

  return (VFile_Handle) vif;
}
