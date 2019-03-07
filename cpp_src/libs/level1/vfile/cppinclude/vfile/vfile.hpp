#pragma once
#ifndef WYRD_VFILE_VFILE_HPP
#define WYRD_VFILE_VFILE_HPP

#include "core/core.h"
#include "vfile/vfile.h"
#include "tinystl/string.h"

namespace VFile{
    struct VFile {

      static VFile *FromFile(char const *filename, enum Os_FileMode mode) {
        return (VFile * )
        VFile_FromFile(filename, mode);
      }
      static VFile *FromMemory(void *memory, size_t size, bool takeOwnership) {
        return (VFile * )
        VFile_FromMemory(memory, size, takeOwnership);
      }

      // free the memory as well (same as C interface)
      void Close() {
        VFile_Close((VFile_Handle) this);
      }

      void Flush() {
        VFile_Flush((VFile_Handle) this);
      }

      size_t
      Read(
          void *buffer, size_t
      byteCount) const {
        return VFile_Read((VFile_Handle) this, buffer, byteCount);
      }
      size_t
      Write(
          void const *buffer, size_t
      byteCount) const {
        return VFile_Write((VFile_Handle) this, buffer, byteCount);
      }
      bool Seek(int64_t offset, enum VFile_SeekDir origin) const {
        return VFile_Seek((VFile_Handle) this, offset, origin);
      }
      int64_t
      Tell() const {
        return VFile_Tell((VFile_Handle) this);
      }
      size_t
      Size() const {
        return VFile_Size((VFile_Handle) this);
      }
      tinystl::string_view
      GetName() const {
        return VFile_GetName((VFile_Handle) this);
      }
      private:
      VFile() = delete;
      ~VFile() = delete;
    };

    struct ScopedVFile {
      ScopedVFile(VFile * ptr) : owner(ptr)
      {};
      ~ScopedVFile()
      { if (owner) { owner->Close(); }}

      operator
      bool() const {
        return owner;
      }
      VFile * operator->()
      {
        return owner;
      }

      VFile * operator->() const {
        return owner;
      }

      VFile *owner;
    };
} // end namespace VFile

#endif //WYRD_VFILE_VFILE_HPP
