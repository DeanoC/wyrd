#pragma once
#ifndef WYRD_THEFORGE_SHADERREFLECTION_HPP
#define WYRD_THEFORGE_SHADERREFLECTION_HPP

#include "theforge/renderer.hpp"
#include "theforge_shaderreflection/theforge_shaderreflection.hpp"
namespace TheForge { namespace ShaderReflection {

void GeneratePipeline(
    TheForge::BinaryShaderDesc *pBinaryShaderDesc,
    Pipeline *pInOutPipeline);

} }; // end namespace

#endif //WYRD_THEFORGE_SHADERREFLECTION_HPP
