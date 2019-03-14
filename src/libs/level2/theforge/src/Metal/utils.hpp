
#pragma once
#ifndef WYRD_THEFORGE_METAL_UTILS_HPP
#define WYRD_THEFORGE_METAL_UTILS_HPP

#include "core/core.h"
#include "structs.hpp"
#include "theforge/renderer.hpp"
#include "theforge/image_enums.hpp"
#include "descriptor_manager.hpp"

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
MTLPixelFormat ToMtlPixelFormat(ImageFormat format, const bool& srgb);
bool IsMtlDepthPixelFormat(const MTLPixelFormat& format);
bool IsMtlCompressedPixelFormat(const MTLPixelFormat& format);
MTLVertexFormat ToMtlVertexFormat(ImageFormat format);
MTLLoadAction ToMtlLoadAction(const LoadActionType& loadActionType);

void BindArgumentBuffer(Cmd *pCmd,
                        DescriptorManager *pManager,
                        const DescriptorInfo *descInfo,
                        const DescriptorData *descData);
void EndCurrentEncoders(Cmd *pCmd);
bool SyncEncoders(Cmd *pCmd, const CmdPoolType& newEncoderType);

}}}; // end namespace TheForge::Metal::Util


#endif //WYRD_THEFORGE_METAL_UTILS_HPP
