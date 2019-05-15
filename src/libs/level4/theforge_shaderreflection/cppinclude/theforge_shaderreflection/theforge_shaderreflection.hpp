#pragma once
#ifndef WYRD_THEFORGE_SHADER_REFLECTION_HPP
#define WYRD_THEFORGE_SHADER_REFLECTION_HPP

#include "core/core.h"
#include "theforge_shaderreflection/theforge_shaderreflection.h"
#include "tinystl/string.h"
#include "tinystl/vector.h"
#include "vfile/vfile.hpp"

namespace TheForge { namespace ShaderReflection {

//using VertexInput = TheForge_VertexInput;
using Variable = TheForge_ShaderReflection_Variable;
using Reflection = TheForge_ShaderReflection_Reflection;
using Pipeline = TheForge_ShaderReflection_Pipeline;
using Macro = TheForge_ShaderReflection_Macro;
using RendererDefinesDesc = TheForge_ShaderReflection_RendererDefinesDesc;
using StageDesc = TheForge_ShaderReflection_StageDesc;
using SourceDesc = TheForge_ShaderReflection_SourceDesc;
using IncludeFunc = TheForge_ShaderReflection_IncludeFunc;

void DestroyReflection(Reflection *pReflection);

void FinishShaderPipeline(
    TheForge::BinaryShaderDesc const *pBinaryShaderDesc,
    Pipeline *pInOutReflection);

void DestroyPipeline(Pipeline *pReflection);

bool GenerateSingleSourceFile(VFile::File *file,
                              IncludeFunc callback,
                              tinystl::string& outCode);

void CompileShader( Renderer *pRenderer, char const* fileName, uint32_t macroCount, Macro *pMacros, char **pByteCode, const char * pEntryPoint);
void CompileBinaryShader(
    TheForge::Renderer *pRenderer,
    SourceDesc* pSourceDesc,
    TheForge::BinaryShaderDesc** ppBinaryShaderDesc,
    Pipeline** ppOutReflection);

RendererDefinesDesc const* GetRendererShaderDefines(Renderer *pRenderer);

} } // end namespace

#endif //WYRD_THEFORGE_SHADER_REFLECTION_HPP
