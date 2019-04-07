#pragma once
#ifndef WYRD_THEFORGE_METAL_DESCRIPTOR_MANAGER_HPP
#define WYRD_THEFORGE_METAL_DESCRIPTOR_MANAGER_HPP

namespace TheForge { namespace Metal {

struct DescriptorManager {
  /// The root signature associated with this descriptor manager.
  RootSignature *pRootSignature;
  /// The descriptor data bound to the current rootSignature;
  DescriptorData *pDescriptorDataArray;
  /// Array of flags to check whether a descriptor has already been bound.
  bool *pBoundDescriptors;
  bool mBoundStaticSamplers;

  // renderer for access to defaults only!
  Renderer* pRenderer;

  /// Map that holds all the argument buffers bound by this descriptor manager for each root signature.
  typedef tinystl::unordered_map<size_t, tinystl::pair<Buffer *, bool>> ArgumentBuffers;
  ArgumentBuffers mArgumentBuffers;
};

void AddDescriptorManager(Renderer *pRenderer, RootSignature *pRootSignature, DescriptorManager **ppManager);
void RemoveDescriptorManager(Renderer *pRenderer, RootSignature *pRootSignature, DescriptorManager *pManager);
DescriptorManager *GetDescriptorManager(Renderer *pRenderer, RootSignature *pRootSignature);
void ResetBoundResources(DescriptorManager *pManager);


} } // end namespace TheForge::Metal

#endif //WYRD_THEFORGE_METAL_DESCRIPTOR_MANAGER_HPP
