#pragma once
#ifndef WYRD_VFILE_VFILE_HPP
#define WYRD_VFILE_VFILE_HPP

#include "core/core.h"
#include "vfile/vfile.h"
#include "vfile/utils.h"
#include "tinystl/string.h"

namespace VFile {
struct File {

  static File *FromFile(char const *filename, enum Os_FileMode mode) {
    return (File *) VFile_FromFile(filename, mode);
  }

  static File *FromFile(tinystl::string const& filename, enum Os_FileMode mode) {
    return (File *) VFile_FromFile(filename.c_str(), mode);
  }

  static File *FromMemory(void *memory, size_t size, bool takeOwnership) {
    return (File *) VFile_FromMemory(memory, size, takeOwnership);
  }
  static File * FromHandle(VFile_Handle handle) {
    return (File*)handle;
  }

  // frees the memory as well (same as C interface)
  void Close() { VFile_Close((VFile_Handle) this); }

  void Flush() { VFile_Flush((VFile_Handle) this); }

  size_t Read(void *buffer, size_t byteCount) const {
    return VFile_Read((VFile_Handle) this, buffer, byteCount);
  }

  size_t Write(void const *buffer, size_t byteCount) const {
    return VFile_Write((VFile_Handle) this, buffer, byteCount);
  }

  bool Seek(int64_t offset, enum VFile_SeekDir origin) const {
    return VFile_Seek((VFile_Handle) this, offset, origin);
  }

  int64_t Tell() const { return VFile_Tell((VFile_Handle) this); }

  size_t Size() const { return VFile_Size((VFile_Handle) this); }

  char const* GetName() const { return VFile_GetName((VFile_Handle) this); }

  bool IsEOF() const { return VFile_IsEOF((VFile_Handle) this); }

  uint8_t ReadByte() { return VFile_ReadByte((VFile_Handle) this); }
  char ReadChar() { return VFile_ReadChar((VFile_Handle) this); }

  int8_t ReadInt8() { return VFile_ReadInt8((VFile_Handle) this); }
  int16_t ReadInt16() { return VFile_ReadInt16((VFile_Handle) this); }
  int32_t ReadInt32() { return VFile_ReadInt32((VFile_Handle) this); }
  int64_t ReadInt64() { return VFile_ReadInt64((VFile_Handle) this); }
  uint8_t ReadUInt8() { return VFile_ReadUInt8((VFile_Handle) this); }
  uint16_t ReadUInt16() { return VFile_ReadUInt16((VFile_Handle) this); }
  uint32_t ReadUInt32() { return VFile_ReadUInt32((VFile_Handle) this); }
  uint64_t ReadUInt64() { return VFile_ReadUInt64((VFile_Handle) this); }

  bool ReadBool() { return VFile_ReadBool((VFile_Handle) this); }
  float ReadFloat() { return VFile_ReadFloat((VFile_Handle) this); }
  double ReadDouble() { return VFile_ReadDouble((VFile_Handle) this); }
  struct vec2_t ReadVector2() { return VFile_ReadVector2((VFile_Handle) this); }
  struct vec3_t ReadVector3() { return VFile_ReadVector3((VFile_Handle) this); }
  struct vec3_t ReadPackedVector3(float maxAbsCoord) {
    return VFile_ReadPackedVector3((VFile_Handle) this,
                                   maxAbsCoord);
  }
  struct vec4_t ReadVector4() { return VFile_ReadVector4((VFile_Handle) this); }
  void ReadFileID(char buffer[4]) { return VFile_ReadFileID((VFile_Handle) this, buffer); }

  tinystl::string ReadString() {
    tinystl::string str;
    str.resize(2048);
    size_t size = VFile_ReadString((VFile_Handle) this, str.data(), str.size());
    str.resize(size);
    return str;
  }
  tinystl::string ReadLine() {
    tinystl::string str;
    str.resize(2048);
    size_t size = VFile_ReadLine((VFile_Handle) this, str.data(), str.size());
    str.resize(size);
    return str;
  }

 private:
  File() {};
  ~File() {}
};

struct ScopedFile {
  ScopedFile(File *ptr) : owned(ptr) {};
  ~ScopedFile() { if (owned) { owned->Close(); }}

  operator bool() const {
    return owned;
  }
  operator File *() const { return owned;}
  operator VFile_Handle() const { return (VFile_Handle)owned;}

  File *operator->() {
    return owned;
  }

  File *operator->() const {
    return owned;
  }

  File *owned;
};
} // end namespace VFile

#endif //WYRD_VFILE_VFILE_HPP
