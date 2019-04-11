#include "core/core.h"
#include "theforge/renderer.hpp"
#include "theforge_shaderreflection/theforge_shaderreflection.hpp"

EXTERN_C void TheForge_ShaderReflection_DestroyReflection(TheForge_ShaderReflection_Reflection *pReflection) {
  TheForge::ShaderReflection::DestroyReflection(pReflection);
}

EXTERN_C void TheForge_ShaderReflection_DestroyPipeline(TheForge_ShaderReflection_Pipeline *pReflection) {
  TheForge::ShaderReflection::DestroyPipeline(pReflection);
}

EXTERN_C bool TheForge_ShaderReflection_GenerateSingleSourceFile(
    VFile_Handle handle,
    TheForge_ShaderReflection_IncludeFunc callback,
    char **outCode) {
  ASSERT(outCode);
  tinystl::string str;
  VFile::File* file = VFile::File::FromHandle(handle);
  bool ret = TheForge::ShaderReflection::GenerateSingleSourceFile(file, callback, str);
  if(ret) {
    *outCode = (char*) malloc(str.size()+1);
    strcpy(*outCode, str.c_str());
    (*outCode)[str.size()] = 0;
  }
  return ret;
}
EXTERN_C void TheForge_ShaderReflection_CompileBinaryShader(
    TheForge_Renderer *pRenderer,
    TheForge_ShaderReflection_SourceDesc* pSourceDesc,
    TheForge_BinaryShaderDesc** ppBinaryShaderDesc,
    TheForge_ShaderReflection_Pipeline **ppOutReflection) {
  TheForge::ShaderReflection::CompileBinaryShader(pRenderer, pSourceDesc, ppBinaryShaderDesc, ppOutReflection);
}

EXTERN_C TheForge_ShaderReflection_RendererDefinesDesc const* TheForge_ShaderReflection_GetRendererShaderDefines(TheForge_Renderer *pRenderer) {
  return TheForge::ShaderReflection::GetRendererShaderDefines(pRenderer);
}

