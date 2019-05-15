#pragma once
#ifndef WYRD_THEFORGE_SHADER_REFLECTION_H
#define WYRD_THEFORGE_SHADER_REFLECTION_H

#include "core/core.h"
#include "theforge/renderer_enums.h"
#include "theforge/renderer_structs.h"
#include "vfile/vfile.h"

typedef struct TheForge_ShaderReflection_Macro {
  char const *definition;
  char const *value;
} TheForge_ShaderReflection_Macro;

typedef struct TheForge_ShaderReflection_RendererDefinesDesc {
  TheForge_ShaderReflection_Macro *rendererShaderDefines;
  uint32_t rendererShaderDefinesCnt;
} TheForge_ShaderReflection_RendererDefinesDesc;

typedef struct TheForge_ShaderReflection_StageDesc {
  char const *mName;
  char const *mCode;
  char const *mEntryPoint;
  size_t mNumMacros;
  TheForge_ShaderReflection_Macro const *mMacros;
} TheForge_ShaderReflection_StageDesc;

typedef struct TheForge_ShaderReflection_Desc {
  TheForge_ShaderStage mStages;
  TheForge_ShaderReflection_StageDesc mVert;
  TheForge_ShaderReflection_StageDesc mFrag;
  TheForge_ShaderReflection_StageDesc mGeom;
  TheForge_ShaderReflection_StageDesc mHull;
  TheForge_ShaderReflection_StageDesc mDomain;
  TheForge_ShaderReflection_StageDesc mComp;
} TheForge_ShaderReflection_SourceDesc;

typedef struct TheForge_ShaderReflection_Variable {
  // parents resource index
  uint32_t parent_index;

  // The offset of the Variable.
  uint32_t offset;

  // The size of the Variable.
  uint32_t size;

  // Variable name
  const char *name;

  // name size
  uint32_t name_size;
} TheForge_ShaderReflection_Variable;

typedef struct TheForge_ShaderReflection_Reflection {
  TheForge_ShaderStage mShaderStage;

  // single large allocation for names to reduce number of allocations
  char *pNamePool;
  uint32_t mNamePoolSize;

//  struct TheForge_VertexInput *pVertexInputs;
//  uint32_t mVertexInputsCount;

  struct TheForge_ShaderResourceDesc *pShaderResources;
  uint32_t mShaderResourceCount;

  struct TheForge_ShaderReflection_Variable *pVariables;
  uint32_t mVariableCount;

  // Thread group size for compute shader
  uint32_t mNumThreadsPerGroup[3];

  //number of tessellation control point
  uint32_t mNumControlPoint;

  char const *pEntryPoint;
} TheForge_ShaderReflection_Reflection;
/*
typedef struct TheForge_VertexInput {
  // The size of the attribute
  uint32_t size;

  // resource name
  const char *name;

  // name size
  uint32_t name_size;
} TheForge_VertexInput;
*/
static const uint32_t TheForge_ShaderReflection_MAX_SHADER_STAGE_COUNT = 5;

typedef struct TheForge_ShaderReflection_Pipeline {
  TheForge_ShaderStage mShaderStages;

  uint32_t mVertexStageIndex;
  uint32_t mHullStageIndex;
  uint32_t mDomainStageIndex;
  uint32_t mGeometryStageIndex;
  uint32_t mPixelStageIndex;

  // the individual stages reflection data.
  TheForge_ShaderReflection_Reflection mStageReflections[TheForge_ShaderReflection_MAX_SHADER_STAGE_COUNT];
  uint32_t mStageReflectionCount;

  TheForge_ShaderReflection_Variable *pVariables;
  uint32_t mVariableCount;

  TheForge_ShaderResourceDesc *pResources;
  uint32_t mResourceCount;

} TheForge_ShaderReflection_Pipeline;

EXTERN_C typedef VFile_Handle (*TheForge_ShaderReflection_IncludeFunc)(char const *);

EXTERN_C bool TheForge_ShaderReflection_GenerateSingleSourceFile(
    VFile_Handle handle,
    TheForge_ShaderReflection_IncludeFunc callback,
    char **outCode);

EXTERN_C void TheForge_ShaderReflection_CompileShader(
    TheForge_Renderer *pRenderer,
    char const *fileName,
    uint32_t macroCount,
    TheForge_ShaderReflection_Macro *pMacros,
    char **pByteCode,
    const char *pEntryPoint);

EXTERN_C void TheForge_ShaderReflection_CreatePipeline(
    TheForge_BinaryShaderDesc *pBinaryShaderDesc,
    TheForge_ShaderReflection_Pipeline *pOutReflection);

EXTERN_C void TheForge_ShaderReflection_CompileBinaryShader(
    TheForge_Renderer *pRenderer,
    TheForge_ShaderReflection_SourceDesc *pSourceDesc,
    TheForge_BinaryShaderDesc **ppBinaryShaderDesc,
    TheForge_ShaderReflection_Pipeline **ppOutReflection);

EXTERN_C TheForge_ShaderReflection_RendererDefinesDesc const *TheForge_ShaderReflection_GetRendererShaderDefines(
    TheForge_Renderer *pRenderer);

#endif //WYRD_THEFORGE_SHADER_REFLECTION_H
