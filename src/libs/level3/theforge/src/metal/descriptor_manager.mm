
#include "core/core.h"
#include "theforge/metal/structs.hpp"
#include "theforge/metal/renderer.hpp"
#include "theforge/metal/descriptor_manager.hpp"
#include "os/thread.hpp"

namespace TheForge { namespace Metal {

// Since there are no descriptor tables in Metal, we just hold a map of all descriptors.
using DescriptorMap = tinystl::unordered_map<uint64_t, DescriptorInfo>;
using ConstDescriptorMapIterator = tinystl::unordered_map<uint64_t, DescriptorInfo>::const_iterator;
using DescriptorMapNode = tinystl::unordered_hash_node<uint64_t, DescriptorInfo>;
using DescriptorNameToIndexMap = tinystl::unordered_map<uint32_t, uint32_t>;

Os::Mutex gDescriptorMutex;

void AddDescriptorManager(Renderer *pRenderer, RootSignature *pRootSignature, DescriptorManager **ppManager) {

  DescriptorManager *pManager = (DescriptorManager *) calloc(1, sizeof(*pManager));
  pManager->pRootSignature = pRootSignature;
  pManager->pRenderer = pRenderer;

  // Allocate enough memory to hold all the necessary data for all the descriptors of this rootSignature.

  size_t const sizeDescriptorDataArray = pRootSignature->mDescriptorCount * sizeof(DescriptorData);
  size_t const sizeBoundDescriptors = pRootSignature->mDescriptorCount * sizeof(bool);

  pManager->pDescriptorDataArray = (DescriptorData *) malloc(sizeDescriptorDataArray);
  pManager->pBoundDescriptors = (bool *) malloc(sizeBoundDescriptors);
  memset(pManager->pDescriptorDataArray, 0, sizeDescriptorDataArray);
  memset(pManager->pDescriptorDataArray, 0, sizeBoundDescriptors);

  // Fill all the descriptors in the rootSignature with their default values.
  for (uint32_t i = 0; i < pRootSignature->mDescriptorCount; ++i) {
    DescriptorInfo *descriptorInfo = &pRootSignature->pDescriptors[i];

    // Create a DescriptorData structure for a default resource.
    pManager->pDescriptorDataArray[i].pName = "";
    pManager->pDescriptorDataArray[i].mCount = 1;
    pManager->pDescriptorDataArray[i].pOffsets = NULL;

    // Metal requires that the bound textures match the texture type present in the shader.
    Texture **ppDefaultTexture = nil;
    if (descriptorInfo->mDesc.type == DESCRIPTOR_TYPE_RW_TEXTURE
        || descriptorInfo->mDesc.type == DESCRIPTOR_TYPE_TEXTURE) {

      switch ((MTLTextureType) descriptorInfo->mDesc.backend.mtlTextureType) {
        case MTLTextureType1D: ppDefaultTexture = &pRenderer->pDefault1DTexture;
          break;
        case MTLTextureType1DArray: ppDefaultTexture = &pRenderer->pDefault1DTextureArray;
          break;
        case MTLTextureType2D: ppDefaultTexture = &pRenderer->pDefault2DTexture;
          break;
        case MTLTextureType2DArray: ppDefaultTexture = &pRenderer->pDefault2DTextureArray;
          break;
        case MTLTextureType3D: ppDefaultTexture = &pRenderer->pDefault3DTexture;
          break;
        case MTLTextureTypeCube: ppDefaultTexture = &pRenderer->pDefaultCubeTexture;
          break;
        case MTLTextureTypeCubeArray: ppDefaultTexture = &pRenderer->pDefaultCubeTextureArray;
          break;
        default: break;
      }
    }

    // Point to the appropiate default resource depending of the type of descriptor.
    switch (descriptorInfo->mDesc.type) {
      case DESCRIPTOR_TYPE_RW_TEXTURE:
      case DESCRIPTOR_TYPE_TEXTURE:
        pManager->pDescriptorDataArray[i].ppTextures = (TheForge::Texture **) ppDefaultTexture;
        break;
      case DESCRIPTOR_TYPE_SAMPLER:
        pManager->pDescriptorDataArray[i].ppSamplers = (TheForge::Sampler **) &pRenderer->pDefaultSampler;
        break;
      case DESCRIPTOR_TYPE_ROOT_CONSTANT:
        // Default root constants can be bound the same way buffers are.
        pManager->pDescriptorDataArray[i].pRootConstant = &pRenderer->pDefaultBuffer;
        break;
      case DESCRIPTOR_TYPE_UNIFORM_BUFFER:
      case DESCRIPTOR_TYPE_RW_BUFFER:
      case DESCRIPTOR_TYPE_BUFFER: {
        pManager->pDescriptorDataArray[i].ppBuffers = (TheForge::Buffer **) &pRenderer->pDefaultBuffer;
        break;
        default: break;
      }
    }
  }

  *ppManager = pManager;
}

void RemoveDescriptorManager(Renderer *pRenderer, RootSignature *pRootSignature, DescriptorManager *pManager) {
  pManager->mArgumentBuffers.clear();
  free(pManager->pDescriptorDataArray);
  free(pManager->pBoundDescriptors);
  free(pManager);
}

// This function returns the descriptor manager belonging to this thread
// If a descriptor manager does not exist for this thread, a new one is created
// With this approach we make sure that descriptor binding is thread safe and lock conf_free at the same time
DescriptorManager *GetDescriptorManager(Renderer *pRenderer, RootSignature *pRootSignature) {

  void *pNode = stb_ptrmap_get(&pRootSignature->pDescriptorManagerMap, Os::Thread::GetCurrentThreadID());
  if (pNode == NULL) {
    // Only need a lock when creating a new descriptor manager for this thread
    Os::MutexLock lock(gDescriptorMutex);
    DescriptorManager *pManager = NULL;
    AddDescriptorManager(pRenderer, pRootSignature, &pManager);
    stb_ptrmap_add(&pRootSignature->pDescriptorManagerMap, Os::Thread::GetCurrentThreadID(), pManager);
    return pManager;
  } else {
    return (DescriptorManager *) pNode;
  }
}

void ResetBoundResources(DescriptorManager *pManager) {
  pManager->mBoundStaticSamplers = false;
  for (uint32_t i = 0; i < pManager->pRootSignature->mDescriptorCount; ++i) {
    DescriptorInfo *descInfo = &pManager->pRootSignature->pDescriptors[i];

    pManager->pDescriptorDataArray[i].mCount = 1;
    pManager->pDescriptorDataArray[i].pOffsets = NULL;
    pManager->pDescriptorDataArray[i].pSizes = NULL;

    // Metal requires that the bound textures match the texture type present in the shader.
    Texture **ppDefaultTexture = nil;
    if (descInfo->mDesc.type == DESCRIPTOR_TYPE_RW_TEXTURE ||
        descInfo->mDesc.type == DESCRIPTOR_TYPE_TEXTURE) {
      switch ((MTLTextureType) descInfo->mDesc.backend.mtlTextureType) {
        case MTLTextureType1D: ppDefaultTexture = &pManager->pRenderer->pDefault1DTexture;
          break;
        case MTLTextureType1DArray: ppDefaultTexture = &pManager->pRenderer->pDefault1DTextureArray;
          break;
        case MTLTextureType2D: ppDefaultTexture = &pManager->pRenderer->pDefault2DTexture;
          break;
        case MTLTextureType2DArray: ppDefaultTexture = &pManager->pRenderer->pDefault2DTextureArray;
          break;
        case MTLTextureType3D: ppDefaultTexture = &pManager->pRenderer->pDefault3DTexture;
          break;
        case MTLTextureTypeCube: ppDefaultTexture = &pManager->pRenderer->pDefaultCubeTexture;
          break;
        case MTLTextureTypeCubeArray: ppDefaultTexture = &pManager->pRenderer->pDefaultCubeTextureArray;
          break;
        default: break;
      }
    }

    switch (descInfo->mDesc.type) {
      case DESCRIPTOR_TYPE_RW_TEXTURE:
      case DESCRIPTOR_TYPE_TEXTURE:
        pManager->pDescriptorDataArray[i].ppTextures = (TheForge::Texture **) ppDefaultTexture;
        break;
      case DESCRIPTOR_TYPE_SAMPLER:
        pManager->pDescriptorDataArray[i].ppSamplers = (TheForge::Sampler **) &pManager->pRenderer->pDefaultSampler;
        break;
      case DESCRIPTOR_TYPE_ROOT_CONSTANT:pManager->pDescriptorDataArray[i].pRootConstant = &pManager->pRenderer->pDefaultBuffer;
        break;
      case DESCRIPTOR_TYPE_UNIFORM_BUFFER:
      case DESCRIPTOR_TYPE_RW_BUFFER:
      case DESCRIPTOR_TYPE_BUFFER: {
        pManager->pDescriptorDataArray[i].ppBuffers = (TheForge::Buffer **) &pManager->pRenderer->pDefaultBuffer;
        break;
        default: break;
      }
    }
    pManager->pBoundDescriptors[i] = false;
  }
}


}} // end namespace TheForge::Metal