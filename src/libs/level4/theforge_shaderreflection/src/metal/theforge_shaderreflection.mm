#include "core/core.h"
#include "tinystl/vector.h"
#include "tinystl/unordered_map.h"
#include "theforge/renderer.hpp"
#include "theforge_shaderreflection/theforge_shaderreflection.hpp"
#include "theforge/metal/structs.hpp"
#include "os/filesystem.hpp"

#define MAX_REFLECT_STRING_LENGTH 128
#define MAX_BUFFER_BINDINGS 31

namespace TheForge { namespace ShaderReflection {
namespace Metal {

struct BufferStructMember {
  char name[MAX_REFLECT_STRING_LENGTH];
  MTLDataType dataType;
  int bufferIndex;
  unsigned long offset;    //byte offset within the uniform block
  uint32_t sizeInBytes;

  BufferStructMember() : sizeInBytes(0), offset(0), bufferIndex(0), dataType(MTLDataTypeNone) { name[0] = '\0'; }
};

struct BufferInfo {
  char name[MAX_REFLECT_STRING_LENGTH];
  uint32_t bufferIndex;
  uint32_t sizeInBytes;
  uint32_t currentOffset;
  bool isUAV;
  bool isArgBuffer;
  DescriptorTypeFlags argBufferType;

  BufferInfo() :
      sizeInBytes(0),
      bufferIndex(~0),
      currentOffset(0),
      isUAV(false),
      isArgBuffer(false),
      argBufferType(DESCRIPTOR_TYPE_UNDEFINED) {
  }
};

struct SamplerInfo {
  char name[MAX_REFLECT_STRING_LENGTH];
  int slotIndex;
};

struct TextureInfo {
  char name[MAX_REFLECT_STRING_LENGTH];
  MTLTextureType type;
  int slotIndex;
  bool isUAV;
};

struct ShaderReflectionInfo {
  tinystl::vector<BufferStructMember> variableMembers;
  tinystl::vector<BufferInfo> buffers;
  tinystl::vector<SamplerInfo> samplers;
  tinystl::vector<TextureInfo> textures;
};

int GetSizeFromDataType(MTLDataType dataType) {
  const int HALF_SIZE = 4;
  const int FLOAT_SIZE = 4;
  const int INT_SIZE = 4;
  switch (dataType) {
    case MTLDataTypeFloat: return FLOAT_SIZE;
    case MTLDataTypeFloat2: return FLOAT_SIZE * 2;
    case MTLDataTypeFloat3: return FLOAT_SIZE * 3;
    case MTLDataTypeFloat4: return FLOAT_SIZE * 4;
    case MTLDataTypeFloat2x2: return FLOAT_SIZE * 2 * 2;
    case MTLDataTypeFloat2x3: return FLOAT_SIZE * 2 * 3;
    case MTLDataTypeFloat2x4: return FLOAT_SIZE * 2 * 4;
    case MTLDataTypeFloat3x2: return FLOAT_SIZE * 3 * 2;
    case MTLDataTypeFloat3x3: return FLOAT_SIZE * 3 * 3;
    case MTLDataTypeFloat3x4: return FLOAT_SIZE * 3 * 4;
    case MTLDataTypeFloat4x2: return FLOAT_SIZE * 4 * 2;
    case MTLDataTypeFloat4x3: return FLOAT_SIZE * 4 * 3;
    case MTLDataTypeFloat4x4: return FLOAT_SIZE * 4 * 4;
    case MTLDataTypeHalf: return HALF_SIZE;
    case MTLDataTypeHalf2: return HALF_SIZE * 2;
    case MTLDataTypeHalf3: return HALF_SIZE * 3;
    case MTLDataTypeHalf4: return HALF_SIZE * 4;
    case MTLDataTypeHalf2x2: return HALF_SIZE * 2 * 2;
    case MTLDataTypeHalf2x3: return HALF_SIZE * 2 * 3;
    case MTLDataTypeHalf2x4: return HALF_SIZE * 2 * 4;
    case MTLDataTypeHalf3x2: return HALF_SIZE * 3 * 2;
    case MTLDataTypeHalf3x3: return HALF_SIZE * 3 * 3;
    case MTLDataTypeHalf3x4: return HALF_SIZE * 3 * 4;
    case MTLDataTypeHalf4x2: return HALF_SIZE * 4 * 2;
    case MTLDataTypeHalf4x3: return HALF_SIZE * 4 * 3;
    case MTLDataTypeHalf4x4: return HALF_SIZE * 4 * 4;
    case MTLDataTypeInt: return INT_SIZE;
    case MTLDataTypeInt2: return INT_SIZE * 2;
    case MTLDataTypeInt3: return INT_SIZE * 3;
    case MTLDataTypeInt4: return INT_SIZE * 4;
    case MTLDataTypeUInt: return INT_SIZE;
    case MTLDataTypeUInt2: return INT_SIZE * 2;
    case MTLDataTypeUInt3: return INT_SIZE * 3;
    case MTLDataTypeUInt4: return INT_SIZE * 4;
    case MTLDataTypeShort: return HALF_SIZE;
    case MTLDataTypeShort2: return HALF_SIZE * 2;
    case MTLDataTypeShort3: return HALF_SIZE * 3;
    case MTLDataTypeShort4: return HALF_SIZE * 4;
    case MTLDataTypeUShort: return HALF_SIZE;
    case MTLDataTypeUShort2: return HALF_SIZE * 2;
    case MTLDataTypeUShort3: return HALF_SIZE * 3;
    case MTLDataTypeUShort4: return HALF_SIZE * 4;
    case MTLDataTypeChar: return 1;
    case MTLDataTypeChar2: return 2;
    case MTLDataTypeChar3: return 3;
    case MTLDataTypeChar4: return 4;
    case MTLDataTypeUChar: return 1;
    case MTLDataTypeUChar2: return 2;
    case MTLDataTypeUChar3: return 3;
    case MTLDataTypeUChar4: return 4;
    case MTLDataTypeBool: return 1;
    case MTLDataTypeBool2: return 2;
    case MTLDataTypeBool3: return 3;
    case MTLDataTypeBool4: return 4;
    case MTLDataTypeTexture: return 8;
    default: break;
  }
  assert(0 && "Unknown metal type");
  return -1;
}

// Returns the total size of the struct
uint32_t ReflectShaderStruct(ShaderReflectionInfo *info,
                             unsigned int bufferIndex,
                             unsigned long parentOffset,
                             MTLStructType *structObj) {
  uint32_t totalSize = 0;
  for (MTLStructMember *member in structObj.members) {
    BufferStructMember bufferMember;
    strlcpy(bufferMember.name, [member.name UTF8String], MAX_REFLECT_STRING_LENGTH);
    bufferMember.bufferIndex = bufferIndex;
    bufferMember.offset = member.offset + parentOffset;
    bufferMember.dataType = member.dataType;

    //  process each MTLStructMember
    if (member.dataType == MTLDataTypeStruct) {
      MTLStructType *nestedStruct = member.structType;
      if (nestedStruct != nil) {
        bufferMember.sizeInBytes = ReflectShaderStruct(info, bufferIndex, bufferMember.offset, nestedStruct);
      } else {
        bufferMember.sizeInBytes = GetSizeFromDataType(bufferMember.dataType);
      }
    } else if (member.dataType == MTLDataTypeArray) {
      assert(member.arrayType != nil);
      int arrayLength = (int) member.arrayType.arrayLength;
      if (member.arrayType.elementType == MTLDataTypeStruct) {
        MTLStructType *nestedStruct = member.arrayType.elementStructType;
        bufferMember.sizeInBytes =
            ReflectShaderStruct(info, bufferIndex, bufferMember.offset, nestedStruct) * arrayLength;
      } else {
        bufferMember.sizeInBytes = GetSizeFromDataType(member.arrayType.elementType) * arrayLength;
      }
    } else {
      // member is neither struct nor array
      // analyze it; no need to drill down further
      bufferMember.sizeInBytes = GetSizeFromDataType(bufferMember.dataType);
    }

    info->variableMembers.push_back(bufferMember);
    totalSize += bufferMember.sizeInBytes;
  }
  return totalSize;
}

void ReflectShaderBufferArgument(ShaderReflectionInfo *info, MTLArgument *arg) {
  if (arg.bufferDataType == MTLDataTypeStruct) {
    // We do this for constant buffer initialization. Constant buffers are always defined in structs,
    // so we only care about structs here.
    MTLStructType *theStruct = arg.bufferStructType;
    ReflectShaderStruct(info, (uint32_t) arg.index, 0, theStruct);
  } else if (arg.bufferDataType == MTLDataTypeArray) {
    assert(!"TODO: Implement");
  }

  // Reflect buffer info
  BufferInfo bufferInfo;
  strlcpy(bufferInfo.name, [arg.name UTF8String], MAX_REFLECT_STRING_LENGTH);
  bufferInfo.bufferIndex = (uint32_t) arg.index;
  bufferInfo.sizeInBytes = (uint32_t) arg.bufferDataSize;
  bufferInfo.isUAV = (arg.access == MTLArgumentAccessReadWrite || arg.access == MTLArgumentAccessWriteOnly);
  bufferInfo.isArgBuffer = arg.bufferPointerType.elementIsArgumentBuffer;
  if (bufferInfo.isArgBuffer) {
    switch (arg.bufferStructType.members[0].arrayType.elementType) {
      case MTLDataTypeSampler: bufferInfo.argBufferType = DESCRIPTOR_TYPE_SAMPLER;
        break;
      case MTLDataTypeTexture: bufferInfo.argBufferType = DESCRIPTOR_TYPE_TEXTURE;
        break;
      default: bufferInfo.argBufferType = DESCRIPTOR_TYPE_BUFFER;
        break;
    }
  }
  info->buffers.push_back(bufferInfo);
}

void ReflectShaderSamplerArgument(ShaderReflectionInfo *info, MTLArgument *arg) {
  SamplerInfo samplerInfo;
  strlcpy(samplerInfo.name, [arg.name UTF8String], MAX_REFLECT_STRING_LENGTH);
  samplerInfo.slotIndex = (uint32_t) arg.index;
  info->samplers.push_back(samplerInfo);
}

void ReflectShaderTextureArgument(ShaderReflectionInfo *info, MTLArgument *arg) {
  TextureInfo textureInfo;
  strlcpy(textureInfo.name, [arg.name UTF8String], MAX_REFLECT_STRING_LENGTH);
  textureInfo.slotIndex = (uint32_t) arg.index;
  textureInfo.type = arg.textureType;
  textureInfo.isUAV = (arg.access == MTLArgumentAccessReadWrite || arg.access == MTLArgumentAccessWriteOnly);
  info->textures.push_back(textureInfo);
}

void ReflectShader(ShaderReflectionInfo *info, NSArray<MTLArgument *> *shaderArgs) {
  for (MTLArgument *arg in shaderArgs) {
    // Ignore arguments not used by the shader
    if (arg.isActive == false) {
      continue;
    }

    if (arg.type == MTLArgumentTypeBuffer) {
      ReflectShaderBufferArgument(info, arg);
    } else if (arg.type == MTLArgumentTypeSampler) {
      ReflectShaderSamplerArgument(info, arg);
    } else if (arg.type == MTLArgumentTypeTexture) {
      ReflectShaderTextureArgument(info, arg);
    }
  }
}

uint32_t CalculateNamePoolSize(const ShaderReflectionInfo *shaderReflectionInfo) {
  uint32_t namePoolSize = 0;
  for (uint32_t i = 0; i < shaderReflectionInfo->variableMembers.size(); i++) {
    const BufferStructMember& varMember = shaderReflectionInfo->variableMembers[i];
    namePoolSize += (uint32_t) strlen(varMember.name) + 1;
  }
  for (uint32_t i = 0; i < shaderReflectionInfo->buffers.size(); i++) {
    const BufferInfo& buffer = shaderReflectionInfo->buffers[i];
    namePoolSize += (uint32_t) strlen(buffer.name) + 1;
  }
  for (uint32_t i = 0; i < shaderReflectionInfo->textures.size(); i++) {
    const TextureInfo& tex = shaderReflectionInfo->textures[i];
    namePoolSize += (uint32_t) strlen(tex.name) + 1;
  }
  for (uint32_t i = 0; i < shaderReflectionInfo->samplers.size(); i++) {
    const SamplerInfo& sampler = shaderReflectionInfo->samplers[i];
    namePoolSize += (uint32_t) strlen(sampler.name) + 1;
  }
  return namePoolSize;
}

bool StartsWith(const char *str, const char *prefix) { return strncmp(prefix, str, strlen(prefix)) == 0; }

bool IsInputVertexBuffer(const BufferInfo& bufferInfo, ShaderStage shaderStage) {
  return (StartsWith(bufferInfo.name, "vertexBuffer.") && shaderStage == SHADER_STAGE_VERT);
}

void AddShaderResource(
    ShaderResource *pResources,
    uint32_t idx,
    DescriptorTypeFlags type,
    uint32_t bindingPoint,
    uint32_t sizeInBytes,
    ShaderStage shaderStage,
    char **ppCurrentName,
    char *name) {
  pResources[idx].type = type;
  pResources[idx].set = DESCRIPTOR_UPDATE_FREQ_NONE;
  pResources[idx].reg = bindingPoint;
  pResources[idx].size = sizeInBytes;
  pResources[idx].used_stages = shaderStage;
  pResources[idx].name = *ppCurrentName;
  pResources[idx].name_size = (uint32_t) strlen(name);
  // we dont own the names memory we need to copy it to the name pool
  memcpy(*ppCurrentName, name, pResources[idx].name_size);
  *ppCurrentName += pResources[idx].name_size + 1;
}

void CreateComputeShaderReflection(
    TheForge::Metal::Renderer *pRenderer,
    id <MTLFunction> mtlComputeShader,
    char const *shaderCode,
    ShaderReflectionInfo **ppOutReflectionInfo) {
  using namespace TheForge::Metal;

  if (ppOutReflectionInfo == NULL) {
    ASSERT(false);
    return;    // TODO: error msg
  }

  NSError *error = nil;
  ShaderReflectionInfo *pReflectionInfo = nil;

  MTLComputePipelineDescriptor *computePipelineDesc = [[MTLComputePipelineDescriptor alloc] init];
  computePipelineDesc.computeFunction = mtlComputeShader;

  MTLComputePipelineReflection *ref;
  id <MTLComputePipelineState> pipelineState =
      [pRenderer->pDevice newComputePipelineStateWithDescriptor:computePipelineDesc
                                                        options:MTLPipelineOptionBufferTypeInfo
                                                     reflection:&ref
                                                          error:&error];

  if (!pipelineState) {
    NSLog(@ "Error generation compute pipeline object: %@", error);
    LOGERROR("Compute pipeline object failed to be created.");
    return;
  }

  *ppOutReflectionInfo = (ShaderReflectionInfo *) malloc(sizeof(ShaderReflectionInfo));
  memset(*ppOutReflectionInfo, 0, sizeof(ShaderReflectionInfo));
  ReflectShader(*ppOutReflectionInfo, ref.arguments);

//  shader->mNumThreadsPerGroup[0] = pOutReflection->mNumThreadsPerGroup[0];
//  shader->mNumThreadsPerGroup[1] = pOutReflection->mNumThreadsPerGroup[1];
//  shader->mNumThreadsPerGroup[2] = pOutReflection->mNumThreadsPerGroup[2];
}
void CreateGfxShaderReflection(
    TheForge::Metal::Renderer *pRenderer,
    id <MTLFunction> mtlVertexShader,
    id <MTLFunction> mtlFragmentShader,
    char const *vertexShaderCode,
    char const *fragmentShaderCode,

    tinystl::unordered_map<uint32_t, MTLVertexFormat> *vertexAttributeFormats,
    ShaderReflectionInfo *pOutVertexReflectionInfo,
    ShaderReflectionInfo *pOutFragmentReflectionInfo) {
  using namespace TheForge::Metal;

  if (pOutVertexReflectionInfo == nullptr || pOutFragmentReflectionInfo == nullptr) {
    ASSERT(false);
    return;    // TODO: error msg
  }
  NSError *error = nil;

  MTLRenderPipelineDescriptor *renderPipelineDesc = [[MTLRenderPipelineDescriptor alloc] init];

  renderPipelineDesc.vertexFunction = mtlVertexShader;
  renderPipelineDesc.fragmentFunction = mtlFragmentShader;

  uint maxColorAttachments = MAX_RENDER_TARGET_ATTACHMENTS;
#ifdef TARGET_IOS
  if (![pRenderer->pDevice supportsFeatureSet:MTLFeatureSet_iOS_GPUFamily2_v1])
        maxColorAttachments = 4;
#endif
  for (uint i = 0; i < maxColorAttachments; i++) {
    renderPipelineDesc.colorAttachments[i].pixelFormat = MTLPixelFormatBGRA8Unorm;
  }
  renderPipelineDesc.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float;

  // We need to create a vertex descriptor if needed to obtain reflection information
  // We are forced to initialize the vertex descriptor with dummy information just to get
  // the reflection information.
  MTLVertexDescriptor *vertexDesc = [[MTLVertexDescriptor alloc] init];

  // read line by line and find vertex attribute definitions
  char *p, *temp;
  p = strtok_r((char *) vertexShaderCode, "\n", &temp);
  do {
    const char *pattern = "attribute(";
    const char *start = strstr(p, pattern);
    if (start != nil) {
      // vertex attribute definitino found: create a vertex descriptor for this
      int attrNumber = atoi(start + strlen(pattern));
      MTLVertexFormat vf = (strstr((const char *) p, "uint") ? MTLVertexFormatUInt : MTLVertexFormatFloat);
      (*vertexAttributeFormats)[attrNumber] = vf;
    }
  } while ((p = strtok_r(NULL, "\n", &temp)) != NULL);

  for (uint32_t i = 0; i < MAX_VERTEX_ATTRIBS; i++) {
    vertexDesc.attributes[i].offset = 0;
    vertexDesc.attributes[i].bufferIndex = 0;

    MTLVertexFormat vf = MTLVertexFormatFloat;

    tinystl::unordered_map<uint32_t, MTLVertexFormat>::iterator it = vertexAttributeFormats->find(i);
    if (it.node != nil) {
      vf = it.node->second;
    }

    vertexDesc.attributes[i].format = vf;
  }
  vertexDesc.layouts[0].stride = MAX_VERTEX_ATTRIBS * sizeof(float);
  vertexDesc.layouts[0].stepRate = 1;
  vertexDesc.layouts[0].stepFunction = mtlVertexShader.patchType != MTLPatchTypeNone
                                       ? MTLVertexStepFunctionPerPatchControlPoint
                                       : MTLVertexStepFunctionPerVertex;

  renderPipelineDesc.vertexDescriptor = vertexDesc;

  MTLRenderPipelineReflection *ref;
  id <MTLRenderPipelineState>
      pipelineState = [pRenderer->pDevice
      newRenderPipelineStateWithDescriptor:renderPipelineDesc
                                   options:MTLPipelineOptionBufferTypeInfo
                                reflection:&ref
                                     error:&error];
  if (!pipelineState) {
    NSLog(@ "Error generation render pipeline object: %@", error);
    LOGERROR("Render pipeline object failed to create.");
    return;
  }

  pOutVertexReflectionInfo = (ShaderReflectionInfo *) calloc(1, sizeof(ShaderReflectionInfo));
  pOutFragmentReflectionInfo = (ShaderReflectionInfo *) calloc(1, sizeof(ShaderReflectionInfo));

  ReflectShader(pOutVertexReflectionInfo, ref.vertexArguments);
  ReflectShader(pOutFragmentReflectionInfo, ref.fragmentArguments);
}

void CreateShaderReflection(
    ShaderReflectionInfo *pReflectionInfo,
    ShaderStage shaderStage,
    tinystl::unordered_map<uint32_t, MTLVertexFormat> *vertexAttributeFormats,
    Reflection *pOutReflection) {

  if (pOutReflection == NULL) {
    assert(0);
    return;    // TODO: error msg
  }
  using namespace TheForge::Metal;

  NSError *error = nil;

  assert(pReflectionInfo != nil);

  // lets find out the size of the name pool we need
  // also get number of resources while we are at it
  uint32_t namePoolSize = CalculateNamePoolSize(pReflectionInfo);
  uint32_t resourceCount = 0;
  uint32_t variablesCount = (uint32_t) pReflectionInfo->variableMembers.size();

  tinystl::vector<BufferInfo> vertexBuffers;

  for (uint32_t i = 0; i < pReflectionInfo->buffers.size(); ++i) {
    const BufferInfo& bufferInfo = pReflectionInfo->buffers[i];
    // The name of the vertex buffers declared as stage_in are internally named by Metal starting with preffix "vertexBuffer."
    if (IsInputVertexBuffer(bufferInfo, shaderStage)) {
      vertexBuffers.push_back(bufferInfo);
    } else {
      ++resourceCount;
    }
  }

  resourceCount += pReflectionInfo->textures.size();
  resourceCount += pReflectionInfo->samplers.size();

  // we now have the size of the memory pool and number of resources
  char *namePool = (char *) calloc(namePoolSize, 1);
  char *pCurrentName = namePool;

  /*
  // start with the vertex input
  VertexInput *pVertexInputs = NULL;
  const uint32_t vertexInputCount = (uint32_t) vertexBuffers.size();
  if (shaderStage == SHADER_STAGE_VERT && vertexInputCount > 0) {
    pVertexInputs = (VertexInput *) malloc(sizeof(VertexInput) * vertexInputCount);

    for (uint32_t i = 0; i < vertexBuffers.size(); ++i) {
      const BufferInfo& vertexBufferInfo = vertexBuffers[i];
      pVertexInputs[i].size = vertexBufferInfo.sizeInBytes;
      pVertexInputs[i].name = pCurrentName;
      pVertexInputs[i].name_size = (uint32_t) strlen(vertexBufferInfo.name);
      // we don't own the names memory we need to copy it to the name pool
      memcpy(pCurrentName, vertexBufferInfo.name, pVertexInputs[i].name_size);
      pCurrentName += pVertexInputs[i].name_size + 1;
    }
  }
*/
  uint32_t resourceIdxByBufferIdx[MAX_BUFFER_BINDINGS];

  // continue with resources
  ShaderResource *pResources = NULL;
  if (resourceCount > 0) {
    pResources = (ShaderResource *) malloc(sizeof(ShaderResource) * resourceCount);
    uint32_t resourceIdx = 0;
    for (uint32_t i = 0; i < pReflectionInfo->buffers.size(); ++i) {
      const BufferInfo& bufferInfo = pReflectionInfo->buffers[i];
      if (!IsInputVertexBuffer(bufferInfo, shaderStage)) {
        DescriptorTypeFlags descriptorType =
            (tinystl::string(bufferInfo.name).to_lower().find("rootconstant", 0) != tinystl::string::npos
             ? DESCRIPTOR_TYPE_ROOT_CONSTANT
             : DESCRIPTOR_TYPE_BUFFER);
        AddShaderResource(
            pResources,
            resourceIdx,
            descriptorType,
            bufferInfo.bufferIndex,
            bufferInfo.sizeInBytes,
            shaderStage,
            &pCurrentName,
            (char *) bufferInfo.name);

        if (bufferInfo.isArgBuffer) {
          pResources[resourceIdx].backend.mtlArgumentBufferType = bufferInfo.argBufferType;
        } else {
          pResources[resourceIdx].backend.mtlArgumentBufferType = RESOURCE_STATE_UNDEFINED;
        }

        resourceIdxByBufferIdx[bufferInfo.bufferIndex] = resourceIdx++;
      }
    }
    for (uint32_t i = 0; i < pReflectionInfo->textures.size(); ++i, ++resourceIdx) {
      const TextureInfo& texInfo = pReflectionInfo->textures[i];
      AddShaderResource(
          pResources,
          resourceIdx,
          texInfo.isUAV ? DESCRIPTOR_TYPE_RW_TEXTURE : DESCRIPTOR_TYPE_TEXTURE,
          texInfo.slotIndex,
          0 /*size*/,
          shaderStage,
          &pCurrentName,
          (char *) texInfo.name);

      pResources[resourceIdx].backend.mtlTextureType = texInfo.type;
      pResources[resourceIdx].backend.mtlArgumentBufferType = RESOURCE_STATE_UNDEFINED;
    }
    for (uint32_t i = 0; i < pReflectionInfo->samplers.size(); ++i, ++resourceIdx) {
      const SamplerInfo& samplerInfo = pReflectionInfo->samplers[i];
      AddShaderResource(
          pResources,
          resourceIdx,
          DESCRIPTOR_TYPE_SAMPLER,
          samplerInfo.slotIndex,
          0 /*samplerInfo.sizeInBytes*/,
          shaderStage,
          &pCurrentName,
          (char *) samplerInfo.name);
      pResources[resourceIdx].backend.mtlArgumentBufferType = RESOURCE_STATE_UNDEFINED;
    }
  }

  Variable *pVariables = NULL;
  // now do variables
  if (variablesCount > 0) {
    pVariables = (Variable *) malloc(sizeof(Variable) * variablesCount);
    for (uint32_t i = 0; i < variablesCount; ++i) {
      const BufferStructMember& variable = pReflectionInfo->variableMembers[i];

      pVariables[i].offset = (uint32_t) variable.offset;
      pVariables[i].size = variable.sizeInBytes;
      pVariables[i].parent_index = resourceIdxByBufferIdx
      [variable.bufferIndex];    // parent_index is an index into the resources list, not just to the buffers list

      pVariables[i].name = pCurrentName;
      pVariables[i].name_size = (uint32_t) strlen(variable.name);
      // we dont own the names memory we need to copy it to the name pool
      memcpy(pCurrentName, variable.name, pVariables[i].name_size);
      pCurrentName += pVariables[i].name_size + 1;
    }
  }

  // all refection structs should be built now
  pOutReflection->mShaderStage = shaderStage;

  pOutReflection->pNamePool = namePool;
  pOutReflection->mNamePoolSize = namePoolSize;

//  pOutReflection->pVertexInputs = pVertexInputs;
//  pOutReflection->mVertexInputsCount = vertexInputCount;

  pOutReflection->pShaderResources = pResources;
  pOutReflection->mShaderResourceCount = resourceCount;

  pOutReflection->pVariables = pVariables;
  pOutReflection->mVariableCount = variablesCount;

  pReflectionInfo->variableMembers.~vector();
  pReflectionInfo->buffers.~vector();
  pReflectionInfo->samplers.~vector();
  pReflectionInfo->textures.~vector();
  free(pReflectionInfo);
}

id <MTLFunction> CreateMetalFunction(
    TheForge::Metal::Renderer *pRenderer,
    const char *shader_name,
    const char *entry_point,
    const char *source,
    NSDictionary *macroDictionary) {

  NSString *shaderSource = [[NSString alloc] initWithUTF8String:source];
  NSError *error = nil;

  MTLCompileOptions *options = [[MTLCompileOptions alloc] init];
  options.preprocessorMacros = macroDictionary;
  id <MTLLibrary> lib = [pRenderer->pDevice newLibraryWithSource:shaderSource options:options error:&error];

  // Warning
  if (error) {
    if (lib) {
      LOGWARNINGF(
          "Loaded shader %s with the following warnings:\n %s",
          shader_name,
          [[error localizedDescription] UTF8String]);
      error = nil;    //  error string is an autorelease object.
    }
      // Error
    else {
      LOGERRORF(
          "Couldn't load shader %s with the following error:\n %s",
          shader_name,
          [[error localizedDescription] UTF8String]);
      error = nil;    //  error string is an autorelease object.
    }
  }

  // Compile the code
  if (lib) {
    NSString *entryPointNStr = [[NSString alloc] initWithUTF8String:entry_point];
    id <MTLFunction> function = [lib newFunctionWithName:entryPointNStr];
    assert(function != nil && "Entry point not found in shader.");
    return function;
  } else {
    return nil;
  }
}

NSDictionary *CreateMacroDictionary(
    size_t shader_macros_count,
    Macro const *shader_macros) {
  // Create a NSDictionary for all the shader macros.
  NSNumberFormatter *numberFormatter =
      [[NSNumberFormatter alloc] init];    // Used for reading NSNumbers macro values from strings.
  numberFormatter.numberStyle = NSNumberFormatterDecimalStyle;

  NSArray *defArray = [[NSArray alloc] init];
  NSArray *valArray = [[NSArray alloc] init];
  for (uint i = 0; i < shader_macros_count; i++) {
    defArray = [defArray arrayByAddingObject:[[NSString alloc] initWithUTF8String:shader_macros[i].definition]];

    // Try reading the macro value as a NSNumber. If failed, use it as an NSString.
    NSString *valueString = [[NSString alloc] initWithUTF8String:shader_macros[i].value];
    NSNumber *valueNumber = [numberFormatter numberFromString:valueString];
    if (valueNumber) {
      valArray = [valArray arrayByAddingObject:valueNumber];
    } else {
      valArray = [valArray arrayByAddingObject:valueString];
    }
  }
  return [[NSDictionary alloc] initWithObjects:valArray forKeys:defArray];
}

void CompileBinaryShader(
    TheForge::Metal::Renderer *pRenderer,
    SourceDesc *pSourceDesc,
    TheForge::BinaryShaderDesc **ppBinaryShaderDesc,
    Pipeline **ppOutReflection) {

  using namespace TheForge::Metal;

  ASSERT(pRenderer);
  ASSERT(pSourceDesc);
  ASSERT(pRenderer->pDevice != nil);
  ASSERT(ppBinaryShaderDesc);

  *ppBinaryShaderDesc = nullptr;

  TheForge::BinaryShaderDesc *pBinaryShaderDesc = (TheForge::BinaryShaderDesc *) calloc(1, sizeof(*pBinaryShaderDesc));

  pBinaryShaderDesc->mStages = pSourceDesc->mStages;

  // split graphics shaders from compute
  if (pSourceDesc->mStages & SHADER_STAGE_ALL_GRAPHICS) {
    tinystl::unordered_map<uint32_t, MTLVertexFormat> vertexAttributeFormats;

    NSDictionary *vertexMacroDict = CreateMacroDictionary(pSourceDesc->mVert.mNumMacros, pSourceDesc->mVert.mMacros);
    NSDictionary *fragMacroDict = CreateMacroDictionary(pSourceDesc->mFrag.mNumMacros, pSourceDesc->mFrag.mMacros);
    __strong id <MTLFunction> vertexFunc = CreateMetalFunction(pRenderer,
                                                               pSourceDesc->mVert.mName,
                                                               pSourceDesc->mVert.mEntryPoint,
                                                               pSourceDesc->mVert.mCode,
                                                               vertexMacroDict);
    __strong id <MTLFunction> fragFunc = CreateMetalFunction(pRenderer,
                                                             pSourceDesc->mFrag.mName,
                                                             pSourceDesc->mFrag.mEntryPoint,
                                                             pSourceDesc->mFrag.mCode,
                                                             fragMacroDict);

      ShaderReflectionInfo pReflectionInfo[2];

      CreateGfxShaderReflection(pRenderer, vertexFunc, fragFunc,
                                pSourceDesc->mVert.mCode,
                                pSourceDesc->mFrag.mCode,
                                &vertexAttributeFormats,
                                &pReflectionInfo[0],
                                &pReflectionInfo[1]);
    if (ppOutReflection != nullptr) {
      ShaderReflection::Pipeline *pOutReflection = (ShaderReflection::Pipeline *) calloc(1, sizeof(*pOutReflection));
      CreateShaderReflection(&pReflectionInfo[0],
                             SHADER_STAGE_VERT,
                             &vertexAttributeFormats, &pOutReflection->mStageReflections[0]);
      CreateShaderReflection(&pReflectionInfo[1],
                             SHADER_STAGE_FRAG,
                             nullptr, &pOutReflection->mStageReflections[1]);

      pOutReflection->mShaderStages = SHADER_STAGE_VERT | SHADER_STAGE_FRAG;
      pOutReflection->mStageReflectionCount = 2;
      FinishShaderPipeline(pBinaryShaderDesc, pOutReflection);
      *ppOutReflection = pOutReflection;
    }
  } else {
    NSDictionary *macroDict = CreateMacroDictionary(pSourceDesc->mComp.mNumMacros, pSourceDesc->mComp.mMacros);
    __strong id <MTLFunction> function = CreateMetalFunction(pRenderer,
                                                             pSourceDesc->mComp.mName,
                                                             pSourceDesc->mComp.mEntryPoint,
                                                             pSourceDesc->mComp.mCode,
                                                             macroDict);

    if (ppOutReflection != nullptr) {
      ShaderReflectionInfo *pReflectionInfo;
      ShaderReflection::Pipeline *pOutReflection = (ShaderReflection::Pipeline *) calloc(1, sizeof(*pOutReflection));
      CreateComputeShaderReflection(pRenderer, function,
                                    pSourceDesc->mComp.mCode,
                                    &pReflectionInfo);

      CreateShaderReflection(&pReflectionInfo[0],
                             SHADER_STAGE_COMP,
                             nullptr, &pOutReflection->mStageReflections[0]);
      pOutReflection->mShaderStages = SHADER_STAGE_COMP;
      pOutReflection->mStageReflectionCount = 1;
      FinishShaderPipeline(pBinaryShaderDesc, pOutReflection);
      *ppOutReflection = pOutReflection;

      // Note: Metal compute shaders don't specify the number of threads per group in the shader code.
      // Instead this must be specified from the client API.
      // To fix this, the parser expects the metal shader code to specify it like other languages do in the following way:
      // Ex: // [numthreads(256, 1, 1)]
      // Notice that it is a commented out line, since we don't want this line to be taken into account by the Metal shader compiler.
      const char *numThreadsStart = strstr((char const *) pSourceDesc->mComp.mCode, "[numthreads(");
      if (numThreadsStart == NULL) {
        assert(!"Compute shaders require: [numthreads(x,y,z)]");
        return;
      }
      numThreadsStart += strlen("[numthreads(");
      const char *numThreadsEnd = strstr(numThreadsStart, ")");
      if (numThreadsEnd == NULL) {
        assert(!"Malformed[numthreads(x,y,z)]");
        return;
      }

      char buff[128] = "";
      size_t len = numThreadsEnd - numThreadsStart;
      memcpy(buff, numThreadsStart, len);
      int count = sscanf(
          buff, "%d,%d,%d",
          &pBinaryShaderDesc->mComp.mNumThreadsPerGroup[0],
          &pBinaryShaderDesc->mComp.mNumThreadsPerGroup[1],
          &pBinaryShaderDesc->mComp.mNumThreadsPerGroup[2]);
      if (count != 3) {
        assert(!"Malformed[numthreads(x,y,z)]");
        return;
      }
    }
  }
  *ppBinaryShaderDesc = pBinaryShaderDesc;
  return;
}

} // end namespace Metal

// Get renderer shader macros
RendererDefinesDesc const *GetRendererShaderDefines(Renderer *pRenderer) {
  static RendererDefinesDesc defineDesc = {NULL, 0};
  return &defineDesc;
}

// On Metal, on the other hand, we can compile from code into a MTLLibrary, but cannot save this
// object's bytecode to disk. We instead use the xcbuild bash tool to compile the shaders.
void CompileShader(
    Renderer *pRenderer,
    const tinystl::string& fileName,
    uint32_t macroCount,
    Macro *pMacros,
    tinystl::vector<char> *pByteCode,
    const char *pEntryPoint) {

  tinystl::string xcrun = "/usr/bin/xcrun";
  tinystl::string intermediateFile = fileName + ".air";
  tinystl::string libFile = fileName + ".metallib";
  tinystl::vector<tinystl::string> args;
  tinystl::string tmpArg = "";

  // Compile the source into a temporary .air file.
  args.push_back("-sdk");
  args.push_back("macosx");
  args.push_back("metal");
  args.push_back("-c");
  tmpArg = tinystl::string::format(
      ""
      "%s"
      "",
      fileName.c_str());
  args.push_back(tmpArg);
  args.push_back("-o");
  args.push_back(intermediateFile.c_str());

  //enable the 2 below for shader debugging on xcode 10.0
  //args.push_back("-MO");
  //args.push_back("-gline-tables-only");
  args.push_back("-D");
  args.push_back("MTL_SHADER=1");    // Add MTL_SHADER macro to differentiate structs in headers shared by app/shader code.
  // Add user defined macros to the command line
  for (uint32_t i = 0; i < macroCount; ++i) {
    args.push_back("-D");
    args.push_back(tinystl::string::format("%s = %i", pMacros[i].definition, pMacros[i].value));
  }
  if (Os::FileSystem::SystemRun(xcrun, args) == 0) {
    // Create a .metallib file from the .air file.
    args.clear();
    tmpArg = "";
    args.push_back("-sdk");
    args.push_back("macosx");
    args.push_back("metallib");
    args.push_back(intermediateFile.c_str());
    args.push_back("-o");
    tmpArg = tinystl::string::format(
        ""
        "%s"
        "",
        libFile.c_str());
    args.push_back(tmpArg);
    if (Os::FileSystem::SystemRun(xcrun, args) == 0) {
      // Remove the temp .air file.
      args.clear();
      args.push_back(intermediateFile.c_str());
      Os::FileSystem::SystemRun("rm", args);

      // Store the compiled bytecode.
      VFile::ScopedFile file(VFile::File::FromFile(libFile, Os_FM_ReadBinary));
      ASSERT(file);
      pByteCode->resize(file->Size());
      memcpy(pByteCode->data(), file->ReadString().c_str(), pByteCode->size());

      // Remove the temp .mtl file.
      args.clear();
      args.push_back(libFile.c_str());
      Os::FileSystem::SystemRun("rm", args);

    } else
      LOGERRORF("Failed to assemble shader's %s .metallib file", fileName.c_str());
  } else {
    LOGERRORF("Failed to compile shader %s", fileName.c_str());
  }
}

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
