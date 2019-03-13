#pragma once
#ifndef WYRD_THEFORGE_RAYTRACER_HPP
#define WYRD_THEFORGE_RAYTRACER_HPP

#include "core/core.h"
#include "theforge/raytracer.h"

namespace TheForge {

static const auto ACCELERATION_STRUCTURE_BUILD_FLAG_NONE = TheForge_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE;
static const auto ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE = TheForge_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;
static const auto ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_COMPACTION = TheForge_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_COMPACTION;
static const auto ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE = TheForge_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
static const auto ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_BUILD = TheForge_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_BUILD;
static const auto ACCELERATION_STRUCTURE_BUILD_FLAG_MINIMIZE_MEMORY = TheForge_ACCELERATION_STRUCTURE_BUILD_FLAG_MINIMIZE_MEMORY;
static const auto ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE = TheForge_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE;
static const auto ACCELERATION_STRUCTURE_GEOMETRY_FLAG_NONE = TheForge_ACCELERATION_STRUCTURE_GEOMETRY_FLAG_NONE;
static const auto ACCELERATION_STRUCTURE_GEOMETRY_FLAG_OPAQUE = TheForge_ACCELERATION_STRUCTURE_GEOMETRY_FLAG_OPAQUE;
static const auto ACCELERATION_STRUCTURE_GEOMETRY_FLAG_NO_DUPLICATE_ANYHIT_INVOCATION = TheForge_ACCELERATION_STRUCTURE_GEOMETRY_FLAG_NO_DUPLICATE_ANYHIT_INVOCATION;
static const auto ACCELERATION_STRUCTURE_INSTANCE_FLAG_NONE = TheForge_ACCELERATION_STRUCTURE_INSTANCE_FLAG_NONE;
static const auto ACCELERATION_STRUCTURE_INSTANCE_FLAG_TRIANGLE_CULL_DISABLE = TheForge_ACCELERATION_STRUCTURE_INSTANCE_FLAG_TRIANGLE_CULL_DISABLE;
static const auto ACCELERATION_STRUCTURE_INSTANCE_FLAG_TRIANGLE_FRONT_COUNTERCLOCKWISE = TheForge_ACCELERATION_STRUCTURE_INSTANCE_FLAG_TRIANGLE_FRONT_COUNTERCLOCKWISE;
static const auto ACCELERATION_STRUCTURE_INSTANCE_FLAG_FORCE_OPAQUE = TheForge_ACCELERATION_STRUCTURE_INSTANCE_FLAG_FORCE_OPAQUE;
static const auto ACCELERATION_STRUCTURE_INSTANCE_FLAG_FORCE_NON_OPAQUE = TheForge_ACCELERATION_STRUCTURE_INSTANCE_FLAG_FORCE_NON_OPAQUE;

using AccelerationStructureBuildFlagBits = TheForge_AccelerationStructureBuildFlagBits;
using AccelerationStructureBuildFlags = TheForge_AccelerationStructureBuildFlags;
using AccelerationStructureGeometryFlagBits = TheForge_AccelerationStructureGeometryFlagBits;
using AccelerationStructureGeometryFlags = TheForge_AccelerationStructureGeometryFlags;
using AccelerationStructureInstanceFlagBits = TheForge_AccelerationStructureInstanceFlagBits;
using AccelerationStructureInstanceFlags = TheForge_AccelerationStructureInstanceFlags;

using AccelerationStructureInstanceDesc = TheForge_AccelerationStructureInstanceDesc;
using AccelerationStructureGeometryDesc = TheForge_AccelerationStructureGeometryDesc;
using AccelerationStructureDescBottom = TheForge_AccelerationStructureDescBottom;
using AccelerationStructureDescTop = TheForge_AccelerationStructureDescTop;
using RaytracingHitGroup = TheForge_RaytracingHitGroup;
using RaytracingShaderTableRecordDesc = TheForge_RaytracingShaderTableRecordDesc;
using RaytracingShaderTableDesc = TheForge_RaytracingShaderTableDesc;
using RaytracingDispatchDesc = TheForge_RaytracingDispatchDesc;
using RaytracingBuildASDesc = TheForge_RaytracingBuildASDesc;
using Raytracing = TheForge_Raytracing;
using AccelerationStructure = TheForge_AccelerationStructure;
using RaytracingShaderTable = TheForge_RaytracingShaderTable;

bool IsRaytracingSupported(Renderer *pRenderer);
bool InitRaytracing(Renderer *pRenderer, Raytracing **ppRaytracing);
void RemoveRaytracing(Renderer *pRenderer, Raytracing *pRaytracing);
void AddAccelerationStructure(Raytracing *pRaytracing,
                              const AccelerationStructureDescTop *pDesc,
                              AccelerationStructure **ppAccelerationStructure);
void RemoveAccelerationStructure(Raytracing *pRaytracing, AccelerationStructure *pAccelerationStructure);

void AddRaytracingShaderTable(Raytracing *pRaytracing,
                              const RaytracingShaderTableDesc *pDesc,
                              RaytracingShaderTable **ppTable);
void RemoveRaytracingShaderTable(Raytracing *pRaytracing, RaytracingShaderTable *pTable);

void CmdBuildAccelerationStructure(Cmd *pCmd, Raytracing *pRaytracing, RaytracingBuildASDesc *pDesc);
void CmdDispatchRays(Cmd *pCmd, Raytracing *pRaytracing, const RaytracingDispatchDesc *pDesc);

} // end namespace TheForge

#endif //WYRD_THEFORGE_RAYTRACER_HPP
