#pragma once
#ifndef WYRD_THEFORGE_RAYTRACER_H
#define WYRD_THEFORGE_RAYTRACER_H

#include "theforge/renderer.h"

typedef struct TheForge_AccelerationStructure TheForge_AccelerationStructure;
typedef struct TheForge_RaytracingShaderTable TheForge_RaytracingShaderTable;

//Supported by DXR. Metal ignores this.
typedef enum TheForge_AccelerationStructureBuildFlagBits {
  TheForge_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE = 0,
  TheForge_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE = 0x1,
  TheForge_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_COMPACTION = 0x2,
  TheForge_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE = 0x4,
  TheForge_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_BUILD = 0x8,
  TheForge_ACCELERATION_STRUCTURE_BUILD_FLAG_MINIMIZE_MEMORY = 0x10,
  TheForge_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE = 0x20,
} TheForge_AccelerationStructureBuildFlagBits;
typedef uint8_t TheForge_AccelerationStructureBuildFlags;

//Rustam: check if this can be mapped to Metal
typedef enum TheForge_AccelerationStructureGeometryFlagBits {
  TheForge_ACCELERATION_STRUCTURE_GEOMETRY_FLAG_NONE = 0,
  TheForge_ACCELERATION_STRUCTURE_GEOMETRY_FLAG_OPAQUE = 0x1,
  TheForge_ACCELERATION_STRUCTURE_GEOMETRY_FLAG_NO_DUPLICATE_ANYHIT_INVOCATION = 0x2
} TheForge_AccelerationStructureGeometryFlagBits;
typedef uint8_t TheForge_AccelerationStructureGeometryFlags;

//Rustam: check if this can be mapped to Metal
typedef enum TheForge_AccelerationStructureInstanceFlagBits {
  TheForge_ACCELERATION_STRUCTURE_INSTANCE_FLAG_NONE = 0,
  TheForge_ACCELERATION_STRUCTURE_INSTANCE_FLAG_TRIANGLE_CULL_DISABLE = 0x1,
  TheForge_ACCELERATION_STRUCTURE_INSTANCE_FLAG_TRIANGLE_FRONT_COUNTERCLOCKWISE = 0x2,
  TheForge_ACCELERATION_STRUCTURE_INSTANCE_FLAG_FORCE_OPAQUE = 0x4,
  TheForge_ACCELERATION_STRUCTURE_INSTANCE_FLAG_FORCE_NON_OPAQUE = 0x8
} TheForge_AccelerationStructureInstanceFlagBits;
typedef uint8_t TheForge_AccelerationStructureInstanceFlags;

typedef struct TheForge_AccelerationStructureInstanceDesc {
  uint32_t mAccelerationStructureIndex;
  /// Row major affine transform for transforming the vertices in the geometry stored in pAccelerationStructure
  float mTransform[12];
  /// User defined instanced ID which can be queried in the shader
  uint32_t mInstanceID;
  uint32_t mInstanceMask;
  uint32_t mInstanceContributionToHitGroupIndex;
  TheForge_AccelerationStructureInstanceFlags mFlags;
} TheForge_AccelerationStructureInstanceDesc;

typedef struct TheForge_AccelerationStructureGeometryDesc {
  TheForge_AccelerationStructureGeometryFlags mFlags;
  vec3_t *pVertexArray;
  uint32_t vertexCount;
  union {
    uint32_t *pIndices32;
    uint16_t *pIndices16;
  };
  uint32_t indicesCount;
  TheForge_IndexType indexType;
} TheForge_AccelerationStructureGeometryDesc;

/************************************************************************/
//	  Bottom Level Structures define the geometry data such as vertex buffers, index buffers
//	  Top Level Structures define the instance data for the geometry such as instance matrix, instance ID, ...
// #mDescCount - Number of geometries or instances in this structure
/************************************************************************/
typedef struct TheForge_AccelerationStructureDescBottom {
  TheForge_AccelerationStructureBuildFlags mFlags;
  /// Number of geometries / instances in thie acceleration structure
  uint32_t mDescCount;
  /// Array of geometries in the bottom level acceleration structure
  TheForge_AccelerationStructureGeometryDesc *pGeometryDescs;
} TheForge_AccelerationStructureDescBottom;

typedef struct TheForge_AccelerationStructureDescTop {
  TheForge_AccelerationStructureBuildFlags mFlags;
  uint32_t mInstancesDescCount;
  TheForge_AccelerationStructureInstanceDesc *pInstanceDescs;
  uint32_t mBottomASDescsCount;
  TheForge_AccelerationStructureDescBottom *mBottomASDescs;
  TheForge_IndexType mIndexType;
} TheForge_AccelerationStructureDescTop;

/************************************************************************/
// Defines which shaders will be used by this hit group
// #pIntersectionShaderName - Name of shader used to test intersection with ray
//	  This will be NULL as long as user does not specify ACCELERATION_STRUCTURE_GEOMETRY_TYPE_PROCEDURAL_PRIMITIVE_AABBS
// #pAnyHitShaderName - Name of shader executed when a ray hits
//	  This will be NULL if user specifies ACCELERATION_STRUCTURE_GEOMETRY_FLAG_OPAQUE in geometry creation
//	  This shader is usually used for work like alpha testing geometry to see if an intersection is valid
// #pClosestHitShaderName - Name of shader executed for the intersection closest to the ray
//	  This shader will do most of the work like shading
// #pHitGroupName - User defined name of the hit group. Use the same name when creating the shader table
/************************************************************************/
typedef struct TheForge_RaytracingHitGroup {
  TheForge_RootSignature *pRootSignature;
  TheForge_Shader *pIntersectionShader;
  TheForge_Shader *pAnyHitShader;
  TheForge_Shader *pClosestHitShader;
  const char *pHitGroupName;
} TheForge_RaytracingHitGroup;

typedef struct TheForge_RaytracingShaderTableRecordDesc {
  const char *pName;
  TheForge_RootSignature *pRootSignature;
  TheForge_DescriptorData *pRootData;
  uint32_t mRootDataCount;
  bool mInvokeTraceRay;
  uint32_t mHitShaderIndex;
  uint32_t mMissShaderIndex;
} TheForge_RaytracingShaderTableRecordDesc;

typedef struct TheForge_RaytracingShaderTableDesc {
  TheForge_Pipeline *pPipeline;
  TheForge_RootSignature *pEmptyRootSignature;
  TheForge_RaytracingShaderTableRecordDesc *pRayGenShader;
  TheForge_RaytracingShaderTableRecordDesc *pMissShaders;
  TheForge_RaytracingShaderTableRecordDesc *pHitGroups;
  uint32_t mMissShaderCount;
  uint32_t mHitGroupCount;
} TheForge_RaytracingShaderTableDesc;

typedef struct TheForge_RaytracingDispatchDesc {
  uint32_t mWidth;
  uint32_t mHeight;
  uint32_t mRootSignatureDescriptorsCount;
  TheForge_AccelerationStructure *pTopLevelAccelerationStructure;
  TheForge_RaytracingShaderTable *pShaderTable;
  TheForge_DescriptorData *pRootSignatureDescriptorData;
  TheForge_RootSignature *pRootSignature;
  TheForge_Pipeline *pPipeline;
} TheForge_RaytracingDispatchDesc;

typedef struct TheForge_RaytracingBuildASDesc {
  TheForge_AccelerationStructure *pAccelerationStructure;
  uint32_t mBottomASIndicesCount;
  uint32_t *pBottomASIndices;
} TheForge_RaytracingBuildASDesc;

typedef struct TheForge_Raytracing {
  TheForge_Renderer *pRenderer;
} TheForge_Raytracing;

const static char *TheForge_RaytracingShaderSettingsBufferName = "gSettings";
const static uint32_t TheForge_RaytracingUserdataStartBufferRegister = 10;

EXTERN_C bool TheForge_IsRaytracingSupported(TheForge_Renderer *pRenderer);
EXTERN_C bool TheForge_InitRaytracing(TheForge_Renderer *pRenderer, TheForge_Raytracing **ppRaytracing);
EXTERN_C void TheForge_RemoveRaytracing(TheForge_Renderer *pRenderer, TheForge_Raytracing *pRaytracing);

EXTERN_C void TheForge_AddAccelerationStructure(TheForge_Raytracing *pRaytracing,
                                                const TheForge_AccelerationStructureDescTop *pDesc,
                                                TheForge_AccelerationStructure **ppAccelerationStructure);
EXTERN_C void TheForge_RemoveAccelerationStructure(TheForge_Raytracing *pRaytracing,
                                                   TheForge_AccelerationStructure *pAccelerationStructure);

EXTERN_C void TheForge_AddRaytracingShaderTable(TheForge_Raytracing *pRaytracing,
                                                const TheForge_RaytracingShaderTableDesc *pDesc,
                                                TheForge_RaytracingShaderTable **ppTable);
EXTERN_C void TheForge_RemoveRaytracingShaderTable(TheForge_Raytracing *pRaytracing,
                                                   TheForge_RaytracingShaderTable *pTable);
EXTERN_C void TheForge_CmdBuildAccelerationStructure(TheForge_Cmd *pCmd,
                                                     TheForge_Raytracing *pRaytracing,
                                                     TheForge_RaytracingBuildASDesc *pDesc);
EXTERN_C void TheForge_CmdDispatchRays(TheForge_Cmd *pCmd,
                                       TheForge_Raytracing *pRaytracing,
                                       const TheForge_RaytracingDispatchDesc *pDesc);

#endif //WYRD_THEFORGE_RAYTRACER_H
