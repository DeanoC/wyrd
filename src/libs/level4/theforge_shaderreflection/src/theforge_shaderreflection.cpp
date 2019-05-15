#include "core/core.h"
#include "core/logger.h"
#include "theforge/renderer.hpp"
#include "theforge_shaderreflection/theforge_shaderreflection.hpp"
#include "vfile/vfile.hpp"
#include "os/filesystem.hpp"
//#include "Interfaces/IMemoryManager.h"

namespace TheForge { namespace ShaderReflection {

//This file contains shader reflection code that is the same for all platforms.
//We know it's the same for all platforms since it only interacts with the
// platform abstractions we created.

#define RESOURCE_NAME_CHECK
static bool ShaderResourceCmp(ShaderResource *a, ShaderResource *b) {
  bool isSame = true;

  isSame = isSame && (a->type == b->type);
  isSame = isSame && (a->set == b->set);
  isSame = isSame && (a->reg == b->reg);
  isSame = isSame && (a->size == b->size);

#ifdef RESOURCE_NAME_CHECK
  // we may not need this, the rest is enough but if we want to be super sure we can do this check
  isSame = isSame && (a->name_size == b->name_size);
  // early exit before string cmp
  if (isSame == false) {
    return isSame;
  }

  isSame = isSame && (strcmp(a->name, b->name) == 0);
#endif
  return isSame;
}

static bool VariableCmp(Variable *a, Variable *b) {
  bool isSame = true;

  isSame = isSame && (a->offset == b->offset);
  isSame = isSame && (a->size == b->size);
  isSame = isSame && (a->name_size == b->name_size);

  // early exit before string cmp
  if (isSame == false) {
    return isSame;
  }

  isSame = isSame && (strcmp(a->name, b->name) == 0);

  return isSame;
}

void DestroyReflection(Reflection *pReflection) {
  if (pReflection == NULL) {
    return;
  }

  free(pReflection->pNamePool);
//  free(pReflection->pVertexInputs);
  free(pReflection->pShaderResources);
  free(pReflection->pVariables);
}

void FinishShaderPipeline(
    TheForge::BinaryShaderDesc const *pBinaryShaderDesc,
    Pipeline *pInOutPipeline) {
  if (pBinaryShaderDesc == NULL) {
    LOGERROR("Parameter 'pBinaryShaderDesc' is NULL.");
    return;
  }
  if (pInOutPipeline == NULL) {
    LOGERROR("Parameter 'pInOutPipeline' is NULL.");
    return;
  }

  // Sanity check to make sure we don't have repeated stages.
  ShaderStage combinedShaderStages = (ShaderStage) 0;
  for (uint32_t i = 0; i < pInOutPipeline->mStageReflectionCount; ++i) {
    if ((combinedShaderStages & pInOutPipeline->mStageReflections[i].mShaderStage) != 0) {
      LOGERROR("Duplicate shader stage was detected in shader reflection array.");
      return;
    }
    combinedShaderStages = (ShaderStage) (combinedShaderStages | pInOutPipeline->mStageReflections[i].mShaderStage);
  }

  // Combine all shaders
  // this will have a large amount of looping
  // 1. count number of resources
  uint32_t vertexStageIndex = ~0u;
  uint32_t hullStageIndex = ~0u;
  uint32_t domainStageIndex = ~0u;
  uint32_t geometryStageIndex = ~0u;
  uint32_t pixelStageIndex = ~0u;
  ShaderResource *pResources = NULL;
  uint32_t resourceCount = 0;
  Variable *pVariables = NULL;
  uint32_t variableCount = 0;

  //Should we be using dynamic arrays for these? Perhaps we can add std::vector
  // like functionality?
  ShaderResource *uniqueResources[512];
  ShaderStage shaderUsage[512];
  Variable *uniqueVariable[512];
  ShaderResource *uniqueVariableParent[512];
  for (uint32_t i = 0; i < pInOutPipeline->mStageReflectionCount; ++i) {
    Reflection const *pSrcRef = pInOutPipeline->mStageReflections + i;

    if (pSrcRef->mShaderStage == SHADER_STAGE_VERT) {
      vertexStageIndex = i;
    }
      else if (pSrcRef->mShaderStage == SHADER_STAGE_HULL) {
        hullStageIndex = i;
      } else if (pSrcRef->mShaderStage == SHADER_STAGE_DOMN) {
        domainStageIndex = i;
      } else if (pSrcRef->mShaderStage == SHADER_STAGE_GEOM) {
        geometryStageIndex = i;
      }
    else if (pSrcRef->mShaderStage == SHADER_STAGE_FRAG) {
      pixelStageIndex = i;
    }

    //Loop through all shader resources
    for (uint32_t j = 0; j < pSrcRef->mShaderResourceCount; ++j) {
      bool unique = true;

      //Go through all already added shader resources to see if this shader
      // resource was already added from a different shader stage. If we find a
      // duplicate shader resource, we add the shader stage to the shader stage
      // mask of that resource instead.
      for (uint32_t k = 0; k < resourceCount; ++k) {
        unique = !ShaderResourceCmp(&pSrcRef->pShaderResources[j], uniqueResources[k]);
        if (unique == false) {
          // update shader usage
          // NOT SURE
          //shaderUsage[k] = (ShaderStage)(shaderUsage[k] & pSrcRef->pShaderResources[j].used_stages);
          shaderUsage[k] |= pSrcRef->pShaderResources[j].used_stages;
          break;
        }
      }

      //If it's unique, we add it to the list of shader resourceas
      if (unique == true) {
        shaderUsage[resourceCount] = pSrcRef->pShaderResources[j].used_stages;
        uniqueResources[resourceCount] = &pSrcRef->pShaderResources[j];
        resourceCount++;
      }
    }

    //Loop through all shader variables (constant/uniform buffer members)
    for (uint32_t j = 0; j < pSrcRef->mVariableCount; ++j) {
      bool unique = true;
      //Go through all already added shader variables to see if this shader
      // variable was already added from a different shader stage. If we find a
      // duplicate shader variables, we don't add it.
      for (uint32_t k = 0; k < variableCount; ++k) {
        unique = !VariableCmp(&pSrcRef->pVariables[j], uniqueVariable[k]);
        if (unique == false) {
          break;
        }
      }

      //If it's unique we add it to the list of shader variables
      if (unique) {
        uniqueVariableParent[variableCount] = &pSrcRef->pShaderResources[pSrcRef->pVariables[j].parent_index];
        uniqueVariable[variableCount] = &pSrcRef->pVariables[j];
        variableCount++;
      }
    }
  }

  //Copy over the shader resources in a dynamic array of the correct size
  if (resourceCount) {
    pResources = (ShaderResource *) malloc(sizeof(ShaderResource) * resourceCount);

    for (uint32_t i = 0; i < resourceCount; ++i) {
      pResources[i] = *uniqueResources[i];
      pResources[i].used_stages = shaderUsage[i];
    }
  }

  //Copy over the shader variables in a dynamic array of the correct size
  if (variableCount) {
    pVariables = (Variable *) malloc(sizeof(Variable) * variableCount);

    for (uint32_t i = 0; i < variableCount; ++i) {
      pVariables[i] = *uniqueVariable[i];
      ShaderResource *parentResource = uniqueVariableParent[i];
      // look for parent
      for (uint32_t j = 0; j < resourceCount; ++j) {
        if (ShaderResourceCmp(&pResources[j], parentResource)) {
          pVariables[i].parent_index = j;
          break;
        }
      }
    }
  }

  // all refection structs should be built now

  pInOutPipeline->mShaderStages = combinedShaderStages;
  pInOutPipeline->mVertexStageIndex = vertexStageIndex;
  pInOutPipeline->mHullStageIndex = hullStageIndex;
  pInOutPipeline->mDomainStageIndex = domainStageIndex;
  pInOutPipeline->mGeometryStageIndex = geometryStageIndex;
  pInOutPipeline->mPixelStageIndex = pixelStageIndex;
  pInOutPipeline->pVariables = pVariables;
  pInOutPipeline->mVariableCount = variableCount;
  pInOutPipeline->pResources = pResources;
  pInOutPipeline->mResourceCount = resourceCount;

}

void DestroyPipeline(Pipeline *pReflection) {
  if (pReflection == NULL) {
    return;
  }

  for (uint32_t i = 0; i < pReflection->mStageReflectionCount; ++i) {
    DestroyReflection(&pReflection->mStageReflections[i]);
  }

  free(pReflection->pResources);
  free(pReflection->pVariables);
}

bool GenerateSingleSourceFile(VFile::File *file,
                              IncludeFunc callback,
                              tinystl::string& outCode) {
  using namespace VFile;

  const tinystl::string pIncludeDirective = "#include";
  while (!file->IsEOF()) {
    tinystl::string line = file->ReadLine();
    tinystl::string::size_type filePos = line.find(pIncludeDirective, 0);
    const tinystl::string::size_type commentPosCpp = line.find("//", 0);
    const tinystl::string::size_type commentPosC = line.find("/*", 0);

    // if we have an "#include \"" in our current line
    const bool bLineHasIncludeDirective = filePos != tinystl::string::npos;
    const bool bLineIsCommentedOut = (commentPosCpp != tinystl::string::npos && commentPosCpp < filePos) ||
        (commentPosC != tinystl::string::npos && commentPosC < filePos);

    if (bLineHasIncludeDirective && !bLineIsCommentedOut) {
      // get the include file name
      uint32_t currentPos = filePos + (uint32_t) pIncludeDirective.size();
      tinystl::string fileName;
      while (line.at(currentPos++) == ' ') {}    // skip empty spaces
      if (currentPos >= (uint32_t) line.size()) {
        continue;
      }
      if (line.at(currentPos - 1) != '\"') {
        continue;
      } else {
        // read char by char until we have the include file name
        while (line.at(currentPos) != '\"') {
          fileName.push_back(line.at(currentPos));
          ++currentPos;
        }
      }

      tinystl::string includeFileName;
      tinystl::string includeExtension;
      Os::FileSystem::SplitPath(file->GetName(), includeFileName, includeExtension);
      // get the include file path
      if (includeFileName.at(0) == '<') {    // disregard bracketsauthop
        continue;
      }

      // open the include file
      ScopedFile includeFile(File::FromHandle(callback(includeFileName.c_str())));
      if (!includeFile) {
        LOGERRORF("Cannot open #include file: %s", includeFileName.c_str());
        return false;
      }

      // Add the include file into the current code recursively
      if (!GenerateSingleSourceFile(includeFile.owned, callback, outCode)) {
        return false;
      }
    }

    outCode += line + "\n";
  }

  return true;
}
/*
#if defined(__ANDROID__)
// Translate Vulkan Shader Type to shaderc shader type
shaderc_shader_kind getShadercShaderType(ShaderStage type)
{
    switch (type)
    {
        case ShaderStage::SHADER_STAGE_VERT: return shaderc_glsl_vertex_shader;
        case ShaderStage::SHADER_STAGE_FRAG: return shaderc_glsl_fragment_shader;
        case ShaderStage::SHADER_STAGE_TESC: return shaderc_glsl_tess_control_shader;
        case ShaderStage::SHADER_STAGE_TESE: return shaderc_glsl_tess_evaluation_shader;
        case ShaderStage::SHADER_STAGE_GEOM: return shaderc_glsl_geometry_shader;
        case ShaderStage::SHADER_STAGE_COMP: return shaderc_glsl_compute_shader;
        default: ASSERT(0); abort();
    }
    return static_cast<shaderc_shader_kind>(-1);
}
#endif

#if defined(VULKAN)
#if defined(__ANDROID__)
// Android:
// Use shaderc to compile glsl to spirV
//@todo add support to macros!!
void vk_compileShader(
    Renderer* pRenderer, ShaderStage stage, uint32_t codeSize, const char* code, const tinystl::string& outFile, uint32_t macroCount,
    ShaderMacro* pMacros, tinystl::vector<char>* pByteCode, const char* pEntryPoint)
{
    // compile into spir-V shader
    shaderc_compiler_t           compiler = shaderc_compiler_initialize();
    shaderc_compilation_result_t spvShader =
        shaderc_compile_into_spv(compiler, code, codeSize, getShadercShaderType(stage), "shaderc_error", pEntryPoint ? pEntryPoint : "main", nullptr);
    if (shaderc_result_get_compilation_status(spvShader) != shaderc_compilation_status_success)
    {
        LOGERRORF("Shader compiling failed! with status");
        abort();
    }

    // Resize the byteCode block based on the compiled shader size
    pByteCode->resize(shaderc_result_get_length(spvShader));
    memcpy(pByteCode->data(), shaderc_result_get_bytes(spvShader), pByteCode->size());

    // Release resources
    shaderc_result_release(spvShader);
    shaderc_compiler_release(compiler);
}
#else
// PC:
// Vulkan has no builtin functions to compile source to spirv
// So we call the glslangValidator tool located inside VulkanSDK on user machine to compile the glsl code to spirv
// This code is not added to Vulkan.cpp since it calls no Vulkan specific functions
void vk_compileShader(
    Renderer* pRenderer, ShaderTarget target, const tinystl::string& fileName, const tinystl::string& outFile, uint32_t macroCount,
    ShaderMacro* pMacros, tinystl::vector<char>* pByteCode, const char* pEntryPoint)
{
    if (!FileSystem::DirExists(FileSystem::GetPath(outFile)))
        FileSystem::CreateDir(FileSystem::GetPath(outFile));

    tinystl::string                  commandLine;
    tinystl::vector<tinystl::string> args;
    tinystl::string                  configFileName;

    // If there is a config file located in the shader source directory use it to specify the limits
    if (FileSystem::FileExists(FileSystem::GetPath(fileName) + "/config.conf", FSRoot::FSR_Absolute))
    {
        configFileName = FileSystem::GetPath(fileName) + "/config.conf";
        // Add command to compile from Vulkan GLSL to Spirv
        commandLine += tinystl::string::format(
            "\"%s\" -V \"%s\" -o \"%s\"", configFileName.size() ? configFileName.c_str() : "", fileName.c_str(), outFile.c_str());
    }
    else
    {
        commandLine += tinystl::string::format("-V \"%s\" -o \"%s\"", fileName.c_str(), outFile.c_str());
    }

    if (target >= shader_target_6_0)
        commandLine += " --target-env vulkan1.1 ";
        //commandLine += " \"-D" + tinystl::string("VULKAN") + "=" + "1" + "\"";

    if (pEntryPoint != nullptr)
        commandLine += tinystl::string::format(" -e %s", pEntryPoint);

        // Add platform macro
#ifdef _WINDOWS
    commandLine += " \"-D WINDOWS\"";
#elif defined(__ANDROID__)
    commandLine += " \"-D ANDROID\"";
#elif defined(__linux__)
    commandLine += " \"-D LINUX\"";
#endif

    // Add user defined macros to the command line
    for (uint32_t i = 0; i < macroCount; ++i)
    {
        commandLine += " \"-D" + pMacros[i].definition + "=" + pMacros[i].value + "\"";
    }
    args.push_back(commandLine);

    tinystl::string glslangValidator = getenv("VULKAN_SDK");
    if (glslangValidator.size())
        glslangValidator += "/bin/glslangValidator";
    else
        glslangValidator = "/usr/bin/glslangValidator";
    if (FileSystem::SystemRun(glslangValidator, args, outFile + "_compile.log") == 0)
    {
        File file = {};
        file.Open(outFile, FileMode::FM_ReadBinary, FSRoot::FSR_Absolute);
        ASSERT(file.IsOpen());
        pByteCode->resize(file.GetSize());
        memcpy(pByteCode->data(), file.ReadText().c_str(), pByteCode->size());
        file.Close();
    }
    else
    {
        File errorFile = {};
        errorFile.Open(outFile + "_compile.log", FM_ReadBinary, FSR_Absolute);
        // If for some reason the error file could not be created just log error msg
        if (!errorFile.IsOpen())
        {
            ErrorMsg("Failed to compile shader %s", fileName.c_str());
        }
        else
        {
            tinystl::string errorLog = errorFile.ReadText();
            errorFile.Close();
            ErrorMsg("Failed to compile shader %s with error\n%s", fileName.c_str(), errorLog.c_str());
            errorFile.Close();
        }
    }
}
#endif
#endif
#if (defined(DIRECT3D12) || defined(DIRECT3D11)) && !defined(ENABLE_RENDERER_RUNTIME_SWITCH)
extern void compileShader(
    Renderer* pRenderer, ShaderTarget target, ShaderStage stage, const char* fileName, uint32_t codeSize, const char* code,
    uint32_t macroCount, ShaderMacro* pMacros, void* (*allocator)(size_t a), uint32_t* pByteCodeSize, char** ppByteCode, const char* pEntryPoint);
#endif
*/

}} // end namespace

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
