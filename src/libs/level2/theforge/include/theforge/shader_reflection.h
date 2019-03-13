#pragma once
#ifndef WYRD_THEFORGE_SHADER_REFLECTION_H
#define WYRD_THEFORGE_SHADER_REFLECTION_H

#include "core/core.h"
#include "theforge/renderer_enums.h"

typedef struct TheForge_ShaderVariable {
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
} TheForge_ShaderVariable;

typedef struct TheForge_ShaderReflection {
  TheForge_ShaderStage mShaderStage;

  // single large allocation for names to reduce number of allocations
  char *pNamePool;
  uint32_t mNamePoolSize;

  struct TheForge_VertexInput *pVertexInputs;
  uint32_t mVertexInputsCount;

  struct TheForge_ShaderResourceDesc *pShaderResources;
  uint32_t mShaderResourceCount;

  struct TheForge_ShaderVariable *pVariables;
  uint32_t mVariableCount;

  // Thread group size for compute shader
  uint32_t mNumThreadsPerGroup[3];

  //number of tessellation control point
  uint32_t mNumControlPoint;

  char const *pEntryPoint;
} TheForge_ShaderReflection;

typedef struct TheForge_VertexInput {
  // The size of the attribute
  uint32_t size;

  // resource name
  const char *name;

  // name size
  uint32_t name_size;
} TheForge_VertexInput;

typedef struct TheForge_ShaderResourceDesc {
  // resource Type
  TheForge_DescriptorTypeFlags type;

  // The resource set for binding frequency
  uint32_t set;

  // The resource binding location
  uint32_t reg;

  // The size of the resource. This will be the DescriptorInfo array size for textures
  uint32_t size;

  // what stages use this resource
  TheForge_ShaderStage used_stages;

  // resource name
  const char *name;

  // name size
  uint32_t name_size;

  // we don't usually expose the backend but this makes the implementation
  // easier and saves an alloc
  union {
    struct {
      uint32_t mtlTextureType;           // Needed to bind different types of textures as default resources on Metal.
      uint32_t mtlArgumentBufferType;    // Needed to bind multiple resources under a same descriptor on Metal.
    };
    uint64_t constant_size; // d3d12
  } backend;

} TheForge_ShaderResourceDesc;

static const uint32_t TheForge_MAX_SHADER_STAGE_COUNT = 5;

typedef struct TheForge_PipelineReflection {
  TheForge_ShaderStage mShaderStages;
  // the individual stages reflection data.
  struct TheForge_ShaderReflection mStageReflections[TheForge_MAX_SHADER_STAGE_COUNT];
  uint32_t mStageReflectionCount;

  uint32_t mVertexStageIndex;
  uint32_t mHullStageIndex;
  uint32_t mDomainStageIndex;
  uint32_t mGeometryStageIndex;
  uint32_t mPixelStageIndex;

  struct TheForge_ShaderResourceDesc *pShaderResources;
  uint32_t mShaderResourceCount;

  struct TheForge_ShaderVariable *pVariables;
  uint32_t mVariableCount;
} TheForge_PipelineReflection;

void TheForge_DestroyShaderReflection(TheForge_ShaderReflection *pReflection);
void TheForge_CreatePipelineReflection(TheForge_ShaderReflection *pReflection,
                                       uint32_t stageCount,
                                       TheForge_PipelineReflection *pOutReflection);
void TheForge_DestroyPipelineReflection(TheForge_PipelineReflection *pReflection);

//void serializeReflection(File* pInFile, Reflection* pReflection);
//void deserializeReflection(File* pOutFile, Reflection* pReflection);
#endif //WYRD_THEFORGE_SHADER_REFLECTION_H
