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

void DestroyShaderReflection(ShaderReflection *pReflection);
void CreatePipelineReflection(ShaderReflection *pReflection,
                              uint32_t stageCount,
                              PipelineReflection *pOutReflection);
void DestroyPipelineReflection(PipelineReflection *pReflection);

} // end namespace TheForge

#endif //WYRD_THEFORGE_SHADER_REFLECTION_HPP
