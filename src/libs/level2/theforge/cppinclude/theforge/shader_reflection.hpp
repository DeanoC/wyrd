#pragma once
#ifndef WYRD_THEFORGE_SHADER_REFLECTION_HPP
#define WYRD_THEFORGE_SHADER_REFLECTION_HPP

#include "core/core.h"
#include "theforge/shader_reflection.h"

namespace TheForge {

using VertexInput = TheForge_VertexInput;
using ShaderResource = TheForge_ShaderResourceDesc;
using ShaderVariable = TheForge_ShaderVariable;
using ShaderReflection = TheForge_ShaderReflection;
using PipelineReflection = TheForge_PipelineReflection;

inline void DestroyShaderReflection(ShaderReflection *pReflection) {
  TheForge_DestroyShaderReflection(pReflection);
}

inline void CreatePipelineReflection(ShaderReflection *pReflection,
                              uint32_t stageCount,
                              PipelineReflection *pOutReflection) {
  TheForge_CreatePipelineReflection(pReflection, stageCount, pOutReflection);
}

inline void DestroyPipelineReflection(PipelineReflection *pReflection) {
  TheForge_DestroyPipelineReflection(pReflection);
}

} // end namespace TheForge

#endif //WYRD_THEFORGE_SHADER_REFLECTION_HPP
