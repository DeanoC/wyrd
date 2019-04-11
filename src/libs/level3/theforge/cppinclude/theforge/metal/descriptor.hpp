#pragma once
#ifndef WYRD_THEFORGE_METAL_DESCRIPTOR_BINDER_HPP
#define WYRD_THEFORGE_METAL_DESCRIPTOR_BINDER_HPP

#include "theforge/metal/structs.hpp"

namespace TheForge { namespace Metal {

void ResetBoundResources(Renderer *pRenderer, DescriptorBinder* pDescriptorBinder, RootSignature* pRootSignature);
void BindArgumentBuffer(Cmd *pCmd,
                        DescriptorBinderNode& pManager,
                        const DescriptorInfo *descInfo,
                        const DescriptorData *descData);

} } // end namespace TheForge::Metal

#endif //WYRD_THEFORGE_METAL_DESCRIPTOR_BINDER_HPP
