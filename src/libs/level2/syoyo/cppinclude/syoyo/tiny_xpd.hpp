// MIT license - full text at bottom of file
// Minor mods by Deano for the Wyrd project
#pragma once
#ifndef WYRD_SYOYO_TINY_XPD_HPP
#define WYRD_SYOYO_TINY_XPD_HPP

#include "tinystl/unordered_map.h"
#include "tinystl/string.h"
#include "tinystl/vector.h"

namespace tiny_xpd {

// ---------------------------------------------
// Based on Xpd header file.
struct Xpd {
  enum PrimType { Point = 0, Spline, Card, Sphere, Archive, CustomPT = 99 };
  enum CoordSpace { World = 0, Object, Local, Micro, CustomCS = 99 };
};

// ---------------------------------------------

// Based on XPD3 file format
// https://knowledge.autodesk.com/support/maya/learn-explore/caas/CloudHelp/cloudhelp/2016/ENU/Maya/files/GUID-43899CB9-CE0F-476E-9E94-591AE2F1F807-htm.html
struct XPDHeader {
  // See XpdFile.h in XGen SDK for details.

  unsigned char fileVersion;
  Xpd::PrimType primType;
  unsigned char primVersion;
  float time;
  uint32_t numCVs;
  Xpd::CoordSpace coordSpace;
  uint32_t numFaces;

  uint32_t numBlocks;
  tinystl::vector<tinystl::string> block;
  tinystl::vector<uint32_t> primSize;

  tinystl::vector<tinystl::string> key;
  tinystl::unordered_map<tinystl::string, int> keyToId;

  tinystl::vector<int> faceid;
  tinystl::vector<uint32_t> numPrims;
  tinystl::vector<uint64_t> blockPosition; // Absolute from the beginning of XPD data

  XPDHeader()
      : fileVersion(0),
        primType(Xpd::PrimType::Point),  // TODO(syoyo): Set invalid value
        primVersion(0),
        time(0.0f),
        numCVs(0),
        coordSpace(Xpd::CoordSpace::World),  // TODO(syoyo): Set invalid value
        numFaces(0),
        numBlocks(0) {}
};

///
/// Struct for serialization.
///
struct XPDHeaderInput {

  unsigned char fileVersion;
  Xpd::PrimType primType;
  unsigned char primVersion;
  float time;
  uint32_t numCVs;
  Xpd::CoordSpace coordSpace;
  uint32_t numFaces;

  uint32_t numBlocks;
  tinystl::vector<tinystl::string> block;
  tinystl::vector<uint32_t> primSize;

  tinystl::vector<tinystl::string> key;
  tinystl::unordered_map<tinystl::string, int> keyToId;

  tinystl::vector<int> faceid;
  tinystl::vector<uint32_t> numPrims;
  tinystl::vector<uint64_t> blockOffset; // Relative offset.

  XPDHeaderInput()
      : fileVersion(0),
        primType(Xpd::PrimType::Point),
        primVersion(0),
        time(0.0f),
        numCVs(0),
        coordSpace(Xpd::CoordSpace::World),
        numFaces(0),
        numBlocks(0) {}
};

///
/// Parse XPD header from a file.
/// This API reads XPD file and stores whole content of read XPD file data to
/// `binary`(i.e. data size equal to the XPD file size). This API is handy but
/// consumes memory. If you want to handle larger XPD and/or multiple XPD file
/// at once(e.g. 1GB or more), consider using ParseXPDHeaderFromMemory.
///
/// @param[in] filename XPD filename.
/// @param[out] xpd_header Parsed XPD header.
/// @param[out] binary Binary data of whole content of XPD file.
/// @param[out] err Error string. Filled when failed to parse XPD file.
///
/// Return false when failed to parse XPD file and report an error string to
/// `err`
///
bool ParseXPDFromFile(const tinystl::string &filename, XPDHeader *xpd_header,
                      tinystl::vector<uint8_t> *binary, tinystl::string *err);

///
/// Parse XPD header from a memory(stream).
/// This API does not create an internal copy of memory(`binary`), thus
/// `binary`(XPD data) must not be free'ed until finising accessing XPD data
/// using parsed XPDHeader.
///
/// @param[in] binary Pointer to XPD binary data.
/// @param[in] binary_length Data length of XPD binary data.
/// @param[out] xpd_header Parsed XPD header.
/// @param[out] err Error string. Filled when failed to parse XPD file.
///
/// Return false when failed to parse XPD file and report an error string to
/// `err`
///
bool ParseXPDHeaderFromMemory(const uint8_t *binary, const size_t binary_length,
                              XPDHeader *xpd_header, tinystl::string *err);

///
/// Serialize XPD data(XPD header + prim data) to a binary.
///
/// App user must know how to setup XPD header info and serialize prim data.
/// (see `examples/simple_writer` example for details)
///
/// Aboslute offfset `blockPosition` is calculated inside of this `SerializeToXPD` API,
/// based on relateive offset `XPDHeaderInput::blockOffset`.
///
/// @param[in] input Input XPD header info
/// @param[in] prim_data Binary stream of prim_data
/// @param[out] xpd_binary Serialized XPD data.
/// @param[out] err Error message(filled when failed to serialize)
///
bool SerializeToXPD(XPDHeaderInput &input, tinystl::vector<uint8_t> &prim_data, tinystl::vector<uint8_t> *xpd_binary, tinystl::string *err);

}  // namespace tiny_xpd

#if defined(TINY_XPD_IMPLEMENTATION)

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>
#include <iostream>  // dbg

namespace tiny_xpd {

///
/// Simple stream reader
///
class StreamReader {
  static inline void swap2(unsigned short *val) {
    unsigned short tmp = *val;
    uint8_t *dst = reinterpret_cast<uint8_t *>(val);
    uint8_t *src = reinterpret_cast<uint8_t *>(&tmp);

    dst[0] = src[1];
    dst[1] = src[0];
  }

  static inline void swap4(uint32_t *val) {
    uint32_t tmp = *val;
    uint8_t *dst = reinterpret_cast<uint8_t *>(val);
    uint8_t *src = reinterpret_cast<uint8_t *>(&tmp);

    dst[0] = src[3];
    dst[1] = src[2];
    dst[2] = src[1];
    dst[3] = src[0];
  }

  static inline void swap4(int *val) {
    int tmp = *val;
    uint8_t *dst = reinterpret_cast<uint8_t *>(val);
    uint8_t *src = reinterpret_cast<uint8_t *>(&tmp);

    dst[0] = src[3];
    dst[1] = src[2];
    dst[2] = src[1];
    dst[3] = src[0];
  }

  static inline void swap8(uint64_t *val) {
    uint64_t tmp = (*val);
    uint8_t *dst = reinterpret_cast<uint8_t *>(val);
    uint8_t *src = reinterpret_cast<uint8_t *>(&tmp);

    dst[0] = src[7];
    dst[1] = src[6];
    dst[2] = src[5];
    dst[3] = src[4];
    dst[4] = src[3];
    dst[5] = src[2];
    dst[6] = src[1];
    dst[7] = src[0];
  }

  static inline void swap8(int64_t *val) {
    int64_t tmp = (*val);
    uint8_t *dst = reinterpret_cast<uint8_t *>(val);
    uint8_t *src = reinterpret_cast<uint8_t *>(&tmp);

    dst[0] = src[7];
    dst[1] = src[6];
    dst[2] = src[5];
    dst[3] = src[4];
    dst[4] = src[3];
    dst[5] = src[2];
    dst[6] = src[1];
    dst[7] = src[0];
  }

  static void cpy4(int *dst_val, const int *src_val) {
    unsigned char *dst = reinterpret_cast<unsigned char *>(dst_val);
    const unsigned char *src = reinterpret_cast<const unsigned char *>(src_val);

    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
    dst[3] = src[3];
  }

  static void cpy4(uint32_t *dst_val, const uint32_t *src_val) {
    unsigned char *dst = reinterpret_cast<unsigned char *>(dst_val);
    const unsigned char *src = reinterpret_cast<const unsigned char *>(src_val);

    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
    dst[3] = src[3];
  }

 public:
  explicit StreamReader(const uint8_t *binary, const size_t length,
                        const bool swap_endian)
      : binary_(binary), length_(length), swap_endian_(swap_endian), idx_(0) {
    (void)pad_;
  }

  bool seek_set(const uint64_t offset) {
    if (offset > length_) {
      return false;
    }

    idx_ = offset;
    return true;
  }

  bool seek_from_currect(const int64_t offset) {
    if ((int64_t(idx_) + offset) < 0) {
      return false;
    }

    if (size_t((int64_t(idx_) + offset)) > length_) {
      return false;
    }

    idx_ = size_t(int64_t(idx_) + offset);
    return true;
  }

  size_t read(const size_t n, const uint64_t dst_len, uint8_t *dst) {
    size_t len = n;
    if ((idx_ + len) > length_) {
      len = length_ - idx_;
    }

    if (len > 0) {
      if (dst_len < len) {
        // dst does not have enough space. return 0 for a while.
        return 0;
      }

      memcpy(dst, &binary_[idx_], len);
      idx_ += len;
      return len;

    } else {
      return 0;
    }
  }

  bool read1(uint8_t *ret) {
    if ((idx_ + 1) > length_) {
      return false;
    }

    const uint8_t val = binary_[idx_];

    (*ret) = val;
    idx_ += 1;

    return true;
  }

  bool read_bool(bool *ret) {
    if ((idx_ + 1) > length_) {
      return false;
    }

    const char val = static_cast<const char>(binary_[idx_]);

    (*ret) = bool(val);
    idx_ += 1;

    return true;
  }

  bool read1(char *ret) {
    if ((idx_ + 1) > length_) {
      return false;
    }

    const char val = static_cast<const char>(binary_[idx_]);

    (*ret) = val;
    idx_ += 1;

    return true;
  }

#if 0
  bool read2(unsigned short *ret) {
    if ((idx_ + 2) > length_) {
      return false;
    }

    unsigned short val =
        *(reinterpret_cast<const unsigned short *>(&binary_[idx_]));

    if (swap_endian_) {
      swap2(&val);
    }

    (*ret) = val;
    idx_ += 2;

    return true;
  }
#endif

  bool read4(uint32_t *ret) {
    if ((idx_ + 4) > length_) {
      return false;
    }

    // use cpy4 considering unaligned access.
    const uint32_t *ptr = reinterpret_cast<const uint32_t *>(&binary_[idx_]);
    uint32_t val;
    cpy4(&val, ptr);

    if (swap_endian_) {
      swap4(&val);
    }

    (*ret) = val;
    idx_ += 4;

    return true;
  }

  bool read4(int *ret) {
    if ((idx_ + 4) > length_) {
      return false;
    }

    // use cpy4 considering unaligned access.
    const int32_t *ptr = reinterpret_cast<const int32_t *>(&binary_[idx_]);
    int32_t val;
    cpy4(&val, ptr);

    if (swap_endian_) {
      swap4(&val);
    }

    (*ret) = val;
    idx_ += 4;

    return true;
  }

#if 0
  bool read8(uint64_t *ret) {
    if ((idx_ + 8) > length_) {
      return false;
    }

    uint64_t val = *(reinterpret_cast<const uint64_t *>(&binary_[idx_]));

    if (swap_endian_) {
      swap8(&val);
    }

    (*ret) = val;
    idx_ += 8;

    return true;
  }

  bool read8(int64_t *ret) {
    if ((idx_ + 8) > length_) {
      return false;
    }

    int64_t val = *(reinterpret_cast<const int64_t *>(&binary_[idx_]));

    if (swap_endian_) {
      swap8(&val);
    }

    (*ret) = val;
    idx_ += 8;

    return true;
  }
#endif

  bool read_float(float *ret) {
    if (!ret) {
      return false;
    }

    float value;
    if (!read4(reinterpret_cast<int *>(&value))) {
      return false;
    }

    (*ret) = value;

    return true;
  }

#if 0
  bool read_double(double *ret) {
    if (!ret) {
      return false;
    }

    double value;
    if (!read8(reinterpret_cast<uint64_t *>(&value))) {
      return false;
    }

    (*ret) = value;

    return true;
  }
#endif

  bool read_string(tinystl::string *ret) {
    if (!ret) {
      return false;
    }

    tinystl::string value;

    // read untile '\0' or end of stream.
    for (;;) {
      char c;
      if (!read1(&c)) {
        return false;
      }

      value.push_back(c);

      if (c == '\0') {
        break;
      }
    }

    (*ret) = value;

    return true;
  }

#if 0
  bool read_value(Value *inout) {
    if (!inout) {
      return false;
    }

    if (inout->Type() == VALUE_TYPE_FLOAT) {
      float value;
      if (!read_float(&value)) {
        return false;
      }

      (*inout) = Value(value);
    } else if (inout->Type() == VALUE_TYPE_INT) {
      int value;
      if (!read4(&value)) {
        return false;
      }

      (*inout) = Value(value);
    } else {
      TINYVDBIO_ASSERT(0);
      return false;
    }

    return true;
  }
#endif

  size_t tell() const { return idx_; }

  const uint8_t *data() const { return binary_; }

  bool swap_endian() const { return swap_endian_; }

  size_t size() const { return length_; }

 private:
  const uint8_t *binary_;
  const size_t length_;
  bool swap_endian_;
  char pad_[7];
  uint64_t idx_;
};

static bool ParseXPDHeader(StreamReader *sr, XPDHeader *xpd, tinystl::string *err) {
  // Header: XPD3
  uint8_t magic[4];
  if (!sr->read(4, 4, magic)) {
    if (err) {
      (*err) += "Failed to read magic number.";
    }
    return false;
  }

  if ((magic[0] == 'X') && (magic[1] == 'P') && (magic[2] == 'D') &&
      (magic[3] == '3')) {
    // ok
  } else {
    if (err) {
      (*err) += "Magic number is not a 'XPD3'.";
    }
    return false;
  }

  // fileVersion(char)
  if (!sr->read1(&xpd->fileVersion)) {
    if (err) {
      (*err) += "Failed to read `fileVersion'.";
    }
    return false;
  }

  // primType
  if (!sr->read4(reinterpret_cast<uint32_t *>(&xpd->primType))) {
    if (err) {
      (*err) += "Failed to read `primType'.";
    }
    return false;
  }

  // primVersion
  if (!sr->read1(&xpd->primVersion)) {
    if (err) {
      (*err) += "Failed to read `primVersion'";
    }
    return false;
  }

  // time
  if (!sr->read_float(&xpd->time)) {
    if (err) {
      (*err) += "Failed to read `time'.";
    }
    return false;
  }

  // numCVs
  if (!sr->read4(&xpd->numCVs)) {
    if (err) {
      (*err) += "Failed to read `numCVs'.";
    }
    return false;
  }

  // coordSpace
  if (!sr->read4(reinterpret_cast<uint32_t *>(&xpd->coordSpace))) {
    if (err) {
      (*err) += "Failed to read `coordSpace'.";
    }
    return false;
  }

  // numBlocks
  if (!sr->read4(&xpd->numBlocks)) {
    if (err) {
      (*err) += "Failed to read `numBlocks'.";
    }
    return false;
  }

  // blockSize.
  // Number of characters for all block names combined(including the end of
  // strinc character for each block)
  {
    uint32_t blockSize(0);
    if (!sr->read4(&blockSize)) {
      if (err) {
        (*err) += "Failed to parse `blockSize`.";
      }
      return false;
    }

    tinystl::vector<char> blockNames(blockSize);

    if (!sr->read(blockSize, blockSize,
                  reinterpret_cast<uint8_t *>(blockNames.data()))) {
      if (err) {
        (*err) += "Failed to read `blockNames'.";
      }
      return false;
    }

    // split names
    {
      size_t last_idx = 0;
      for (size_t i = 0; i < blockSize; i++) {
        if (blockNames[i] == '\0') {
          tinystl::string name(&blockNames[last_idx], &blockNames[i]);
          xpd->block.push_back(name);
          last_idx = i + 1;
        }
      }
    }
  }

  // primSize
  {
    for (size_t i = 0; i < xpd->block.size(); i++) {
      uint32_t primSize;
      if (!sr->read4(&primSize)) {
        if (err) {
          (*err) += "Failed to read `primSize'.";
        }
        return false;
      }

      if (primSize < 1) {
        // ???
        if (err) {
          (*err) += "primSize[" + tinystl::to_string(i) + " is zero.";
        }
        return false;
      }

      xpd->primSize.push_back(primSize);

    }
  }

  // numKeys
  // Number of characters for all key names combined(including the end of strinc
  // character for each key)
  {
    uint32_t numKeys(0);
    if (!sr->read4(&numKeys)) {
      if (err) {
        (*err) += "Failed to parse `numKeys`.";
      }
      return false;
    }

    uint32_t keySize(0);
    if (!sr->read4(&keySize)) {
      if (err) {
        (*err) += "Failed to parse `keySize`.";
      }
      return false;
    }

    // keySize may be 0.
    if (keySize > 0) {
      tinystl::vector<char> keyNames(keySize);

      if (!sr->read(keySize, keySize,
                    reinterpret_cast<uint8_t *>(keyNames.data()))) {
        if (err) {
          (*err) += "Failed to read `keyNames'.";
        }
        return false;
      }

      // split names
      {
        size_t last_idx = 0;
        for (size_t i = 0; i < keySize; i++) {
          if (keyNames[i] == '\0') {
            tinystl::string name(&keyNames[last_idx], &keyNames[i]);
            xpd->key.push_back(name);
            last_idx = i + 1;
          }
        }
      }
    }
  }

  // numFaces
  if (!sr->read4(&xpd->numFaces)) {
    if (err) {
      (*err) += "Failed to parse `numFaces`.";
    }
    return false;
  }

  if (xpd->numFaces < 1) {
    if (err) {
      (*err) += "numFaces is zero";
    }
    return false;
  }

  // faceid. length = numFaces.
  xpd->faceid.resize(xpd->numFaces);

  if (!sr->read(sizeof(int32_t) * xpd->numFaces,
                sizeof(int32_t) * xpd->numFaces,
                reinterpret_cast<uint8_t *>(xpd->faceid.data()))) {
    if (err) {
      (*err) += "Failed to parse `faceid`.";
    }
    return false;
  }

  // numPrims. length = numFaces.
  xpd->numPrims.resize(xpd->numFaces);
  if (!sr->read(sizeof(uint32_t) * xpd->numFaces,
                sizeof(uint32_t) * xpd->numFaces,
                reinterpret_cast<uint8_t *>(xpd->numPrims.data()))) {
    if (err) {
      (*err) += "Failed to parse `numPrims`.";
    }
    return false;
  }

  // blockPosition. length = numFaces * numBlocks.
  xpd->blockPosition.resize(xpd->numFaces * xpd->numBlocks);
  if (!sr->read(sizeof(uint64_t) * xpd->blockPosition.size(),
                sizeof(uint64_t) * xpd->blockPosition.size(),
                reinterpret_cast<uint8_t *>(xpd->blockPosition.data()))) {
    if (err) {
      (*err) += "Failed to parse `blockPosition`.";
    }
    return false;
  }

  return true;
}

#if 0
bool ParseXPDFromFile(const tinystl::string &filename, XPDHeader *xpd_header,
                      tinystl::vector<uint8_t> *binary, tinystl::string *err) {
  if (filename.empty()) {
    if (err) {
      (*err) = "`filename` is empty.\n";
    }

    return false;
  }

  if (!xpd_header) {
    if (err) {
      (*err) = "`xpd_header` argument is null.\n";
    }

    return false;
  }

  if (!binary) {
    if (err) {
      (*err) = "`binary` argument is null.\n";
    }

    return false;
  }

  tinystl::ifstream ifs(filename, tinystl::ios::in | tinystl::ios::binary);
  if (!ifs) {
    if (err) {
      (*err) = "Failed to open a file.\n";
    }
    return false;
  }

  // Read whole content of XPD file.
  ifs.seekg(0, ifs.end);
  size_t sz = static_cast<size_t>(ifs.tellg());
  if (int64_t(sz) < 0) {
    // Looks reading directory, not a file.
    if (err) {
      (*err) += "Looks like filename is a directory.\n";
    }
    return false;
  }

  if (sz < 16) {
    // ???
    if (err) {
      (*err) +=
          "File size too short. Looks like this file is not a XPD format.\n";
    }
    return false;
  }

  binary->resize(sz);

  ifs.seekg(0, ifs.beg);
  ifs.read(reinterpret_cast<char *>(binary->data()),
           static_cast<tinystl::streamsize>(sz));

  bool ret =
      ParseXPDHeaderFromMemory(binary->data(), binary->size(), xpd_header, err);

  return ret;
}
#endif

bool ParseXPDHeaderFromMemory(const uint8_t *binary, const size_t binary_length,
                              XPDHeader *xpd_header, tinystl::string *err) {
  if (!xpd_header) {
    if (err) {
      (*err) = "`xpd_header` argument is null.\n";
    }

    return false;
  }

  if (!binary) {
    if (err) {
      (*err) = "`binary` argument is null\n";
    }

    return false;
  }

  if (binary_length < 16) {
    if (err) {
      (*err) = "`binary_length` is too short. It looks its not a XPD data\n";
    }

    return false;
  }

  // TODO(syoyo): Consider endianness
  StreamReader sr(binary, binary_length, /* swap endian */ false);

  if (!ParseXPDHeader(&sr, xpd_header, err)) {
    return false;
  }

  return true;
}

#if 0
bool SerializeToXPD(XPDHeaderInput &input, tinystl::vector<uint8_t> &prim_data, tinystl::vector<uint8_t> *xpd_binary, tinystl::string *err) {

  if (prim_data.size() < 4) {
    if (err) {
      (*err) += "Data size too short for primitive data.\n";
    }
    return false;
  }

  if (!xpd_binary) {
    if (err) {
      (*err) += "Pointer to `xpd_binary' is null.\n";
    }
    return false;
  }

  if (input.numFaces == 0) {
    if (err) {
      (*err) += "`numFaces' is zero.\n";
    }
    return false;
  }

  if (input.numBlocks == 0) {
    if (err) {
      (*err) += "`numBlocks' is zero.\n";
    }
    return false;
  }

  if (input.faceid.size() == 0) {
    if (err) {
      (*err) += "Array length of `faceid` is zero.\n";
    }
    return false;
  }

  if (input.numPrims.size() == 0) {
    if (err) {
      (*err) += "Array length of `numPrims` is zero.\n";
    }
    return false;
  }

  if (input.blockOffset.size() == 0) {
    if (err) {
      (*err) += "Array length of `blockOffset` is zero.\n";
    }
    return false;
  }

  if (input.numFaces != (input.faceid.size())) {
    if (err) {
      (*err) += "`numFaces`(" + tinystl::to_string(input.numFaces) + ") must be same with `faceid`.size() which is " + tinystl::to_string(input.faceid.size()) + ".\n";
    }
    return false;
  }

  if ((input.numFaces * input.numBlocks) != (input.blockOffset.size())) {
    if (err) {
      (*err) += "`numFaces * numBlocks`(" + tinystl::to_string(input.numFaces * input.numBlocks) + ") must be same with `blockOffset`.size() which is " + tinystl::to_string(input.blockOffset.size()) + ".\n";
    }
  }

  if (input.block.size() != (input.primSize.size())) {
    if (err) {
      (*err) += "`block.size()`(" + tinystl::to_string(input.block.size()) + ") must be same with `primSize.size()`(" + tinystl::to_string(input.primSize.size()) + ".\n";
    }
    return false;
  }

  tinystl::ostringstream ss;

  // TODO(syoyo): Consider endinanness.
  {
    // magic
    char magic[4] = {'X', 'P', 'D', '3'};
    ss.write(magic, 4);

    // fileVersion(char)
    ss.write(reinterpret_cast<const char *>(&input.fileVersion), 1);

    ss.write(reinterpret_cast<const char *>(&input.primType), sizeof(uint32_t));
    ss.write(reinterpret_cast<const char *>(&input.primVersion), 1);
    ss.write(reinterpret_cast<const char *>(&input.time), sizeof(float));

    ss.write(reinterpret_cast<const char *>(&input.numCVs), sizeof(uint32_t));
    ss.write(reinterpret_cast<const char *>(&input.coordSpace), sizeof(uint32_t));

  }

  // block
  {
    ss.write(reinterpret_cast<const char *>(&input.numBlocks), sizeof(uint32_t));

    // Build a string with flattened block names(using delimiter '\0')
    uint32_t block_name_size = 0;
    tinystl::string block_name_str;
    for (size_t i = 0; i < input.numBlocks; i++) {
      block_name_str += input.block[i];
      block_name_str += '\0';

      block_name_size += input.block[i].size() + 1; // +1 for '\0'
    }

    ss.write(reinterpret_cast<const char *>(&block_name_size), sizeof(uint32_t));

    ss.write(block_name_str.c_str(), block_name_size);
  }

  // primSize
  {
    for (size_t i = 0; i < input.block.size(); i++) {
      ss.write(reinterpret_cast<const char *>(&input.primSize[i]), sizeof(uint32_t));
    }
  }

  // keys
  {
    uint32_t numKeys = uint32_t(input.key.size());
    ss.write(reinterpret_cast<const char *>(&numKeys), sizeof(uint32_t));

    // Build a string with flattened string(using delimiter '\0')
    uint32_t key_name_size = 0;
    tinystl::string key_name_str;
    for (size_t i = 0; i < numKeys; i++) {
      key_name_str += input.key[i];
      key_name_str += '\0';

      key_name_size += input.key[i].size() + 1; // +1 for '\0'
    }

    ss.write(reinterpret_cast<const char *>(&key_name_size), sizeof(uint32_t));

    if (key_name_size > 0) {
      ss.write(key_name_str.c_str(), key_name_size);
    }
  }


  // faceid
  {
    ss.write(reinterpret_cast<const char *>(&input.numFaces), sizeof(uint32_t));


    if (input.numFaces > 0) {
      ss.write(reinterpret_cast<const char *>(input.faceid.data()), input.numFaces * sizeof(uint32_t));
    }

  }

  // numPrims. length = numFaces
  {
    ss.write(reinterpret_cast<const char *>(input.numPrims.data()), input.numFaces * sizeof(uint32_t));
  }


  // header size includes `blockPosition` data.
  size_t header_size = ss.str().size() + input.blockOffset.size() * sizeof(uint64_t);

  // Compute absolute blockPosition
  for (size_t b = 0; b < input.blockOffset.size(); b++) {
    uint64_t position = header_size + input.blockOffset[b];
    ss.write(reinterpret_cast<const char *>(&position), sizeof(uint64_t));
  }

  // Append PrimData.
  ss.write(reinterpret_cast<const char *>(prim_data.data()), tinystl::streamsize(prim_data.size()));

  size_t binary_length = ss.str().size();

  xpd_binary->resize(binary_length);
  memcpy(xpd_binary->data(), ss.str().c_str(), binary_length);

  return true;
}
#endif

}  // namespace tiny_xpd

#endif  // TINY_XPD_IMPLEMENTATION

#endif //WYRD_TINY_XPD_HPP

/*
The MIT License (MIT)

Copyright (c) 2019 Syoyo Fujita.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
