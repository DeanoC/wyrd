
#pragma once
#ifndef WYRD_THEFORGE_METAL_UTILS_HPP
#define WYRD_THEFORGE_METAL_UTILS_HPP

#include "core/core.h"
#include "structs.hpp"
#include "theforge/renderer.hpp"
#include "image/image.h"
#include "theforge/metal/descriptor.hpp"

enum LogType {
  LOG_TYPE_INFO,
  LOG_TYPE_WARN,
  LOG_TYPE_DEBUG,
  LOG_TYPE_ERROR
};
void InternalLog(LogType type, const char *msg, const char *component);

namespace TheForge { namespace Metal { namespace Util {

uint64_t PthreadToUint64(const pthread_t& value);
bool IsCompatibleTextureView(const MTLTextureType& textureType, const MTLTextureType& subviewTye);
MTLPixelFormat ToMtlPixelFormat(Image_Format format);
bool IsMtlDepthPixelFormat(const MTLPixelFormat& format);
bool IsMtlCompressedPixelFormat(const MTLPixelFormat& format);
MTLVertexFormat ToMtlVertexFormat(Image_Format format);
MTLLoadAction ToMtlLoadAction(const LoadActionType& loadActionType);

void EndCurrentEncoders(Cmd *pCmd);
bool SyncEncoders(Cmd *pCmd, const CmdPoolType& newEncoderType);

}}}; // end namespace TheForge::Metal::Util


#endif //WYRD_THEFORGE_METAL_UTILS_HPP
