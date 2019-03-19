#pragma once
#ifndef WYRD_SPIRVTOOLS_SPIRVTOOLS_H
#define WYRD_SPIRVTOOLS_SPIRVTOOLS_H

typedef struct Spirvtools_Type
{
  // Resources are identified with their SPIR-V ID.
  // This is the ID of the OpVariable.
  uint32_t id;

  // The type ID of the variable which includes arrays and all type modifications.
  // This type ID is not suitable for parsing OpMemberDecoration of a struct and other decorations in general
  // since these modifications typically happen on the base_type_id.
  uint32_t type_id;

  // The base type of the declared resource.
  // This type is the base type which ignores pointers and arrays of the type_id.
  // This is mostly useful to parse decorations of the underlying type.
  // base_type_id can also be obtained with get_type(get_type(type_id).self).
  uint32_t base_type_id;
} Spirvtools_Type;

enum
{
  Spirvtools_RT_STAGE_INPUTS = 0,
  Spirvtools_RT_STAGE_OUTPUTS,
  Spirvtools_RT_UNIFORM_BUFFERS,
  Spirvtools_RT_STORAGE_BUFFERS,
  Spirvtools_RT_IMAGES,
  Spirvtools_RT_STORAGE_IMAGES,
  Spirvtools_RT_SAMPLERS,
  Spirvtools_RT_PUSH_CONSTANT,
  Spirvtools_RT_SUBPASS_INPUTS,
  Spirvtools_RT_UNIFORM_TEXEL_BUFFERS,
  Spirvtools_RT_STORAGE_TEXEL_BUFFERS,
  Spirvtools_RT_COUNT
};
typedef uint8_t Spirvtools_Resource_Type;

enum
{
  Spirvtools_TD_UNDEFINED = 0,
  Spirvtools_TD_1D,
  Spirvtools_TD_2D,
  Spirvtools_TD_3D,
  Spirvtools_TD_1D_ARRAY,
  Spirvtools_TD_2D_ARRAY,
  Spirvtools_TD_CUBE,
  Spirvtools_TD_COUNT
};
typedef uint8_t Spirvtools_Texture_Dim;

typedef struct Spirvtools_Resource
{
  // Spirv data type
  Spirvtools_Type SPIRV_code;

  // resource Type
  Spirvtools_Resource_Type type;

  // Texture dimension. Undefined if not a texture.
  Spirvtools_Texture_Dim dim;

  // If the resouce was used in the shader
  bool is_used;

  // The resouce set if it has one
  uint32_t set;

  // The resource binding location
  uint32_t binding;

  // The size of the resouce. This will be the descriptor array size for textures
  uint32_t size;

  // The declared name (OpName) of the resource.
  // For Buffer blocks, the name actually reflects the externally
  // visible Block name.
  const char* name;

  // name size
  uint32_t name_size;
} Spirvtools_Resource;

typedef struct Spirvtools_Variable
{
  // Spirv data type
  uint32_t SPIRV_type_id;

  // parents SPIRV code
  Spirvtools_Type parent_SPIRV_code;

  // parents resource index
  uint32_t parent_index;

  // If the data was used
  bool is_used;

  // The offset of the Variable.
  uint32_t offset;

  // The size of the Variable.
  uint32_t size;

  // Variable name
  const char* name;

  // name size
  uint32_t name_size;
} Spirvtools_Variable;

typedef struct Spirvtools_CrossCompiler
{
  // this points to the internal compiler class
  void* pCompiler;

  // resources
  Spirvtools_Resource* pShaderResouces;
  uint32_t ShaderResourceCount;

  // uniforms
  Spirvtools_Variable* pUniformVariables;
  uint32_t UniformVariablesCount;

  char* pEntryPoint;
  uint32_t EntryPointSize;
} Spirvtools_Cross;

typedef struct{}* Spirvtools_CompilerHandle;


EXTERN_C void Spirvtools_CompilerCreate(Spirvtools_CompilerHandle* handle);
EXTERN_C void Spirvtools_CompilerDestroy(Spirvtools_CompilerHandle handle);
EXTERN_C size_t Spirvtools_CompilerCompile(Spirvtools_CompilerHandle handle, const char *text, const size_t length, uint32_t **outSpirvCode);

EXTERN_C void Spirvtools_CrossCreate(const uint32_t* SpirvBinary, uint32_t BinarySize, Spirvtools_CrossCompiler* outCompiler);
EXTERN_C void Spirvtools_CrossDestroy(Spirvtools_Cross* compiler);
EXTERN_C void Spirvtools_CrossReflectEntryPoint(Spirvtools_Cross* compiler);
EXTERN_C void Spirvtools_CrossReflectShaderResources(Spirvtools_Cross* compiler);
EXTERN_C void Spirvtools_CrossReflectShaderVariables(Spirvtools_Cross* compiler);
EXTERN_C void Spirvtools_CrossReflectComputeShaderWorkGroupSize(Spirvtools_Cross* compiler, uint32_t* pSizeX, uint32_t* pSizeY, uint32_t* pSizeZ);
EXTERN_C void Spirvtools_CrossReflectHullShaderControlPoint(Spirvtools_Cross* pCompiler, uint32_t* pSizeX);
EXTERN_C size_t Spirvtools_CrossCompile(Spirvtools_Cross* compiler, char **source);

#endif //WYRD_SPIRVTOOLS_SPIRVTOOLS_H
/*
 * Copyright (c) 2018-2019 Confetti Interactive Inc.
 *
 * This file is part of The-Forge
 * (see https://github.com/ConfettiFX/The-Forge).
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
*/
