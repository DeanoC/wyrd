#pragma once
#ifndef WYRD_THEFORGE_RENDERER_ENUMS_H
#define WYRD_THEFORGE_RENDERER_ENUMS_H

enum {
  TheForge_MAX_INSTANCE_EXTENSIONS = 64,
  TheForge_MAX_DEVICE_EXTENSIONS = 64,
  TheForge_MAX_GPUS = 10,
  TheForge_MAX_RENDER_TARGET_ATTACHMENTS = 8,
  TheForge_MAX_SUBMIT_CMDS = 20,    // max number of command lists / command buffers
  TheForge_MAX_SUBMIT_WAIT_SEMAPHORES = 8,
  TheForge_MAX_SUBMIT_SIGNAL_SEMAPHORES = 8,
  TheForge_MAX_PRESENT_WAIT_SEMAPHORES = 8,
  TheForge_MAX_VERTEX_BINDINGS = 15,
  TheForge_MAX_VERTEX_ATTRIBS = 15,
  TheForge_MAX_SEMANTIC_NAME_LENGTH = 128,
  TheForge_MAX_MIP_LEVELS = 0xFFFFFFFF,
  TheForge_MAX_BATCH_BARRIERS = 64,
  TheForge_MAX_GPU_VENDOR_STRING_LENGTH = 64,    //max size for GPUVendorPreset strings

  TheForge_RED = 0x1,
  TheForge_GREEN = 0x2,
  TheForge_BLUE = 0x4,
  TheForge_ALPHA = 0x8,
  TheForge_ALL = (TheForge_RED | TheForge_GREEN | TheForge_BLUE | TheForge_ALPHA),
  TheForge_NONE = 0,
  TheForge_BS_NONE = -1,
  TheForge_DS_NONE = -1,
  TheForge_RS_NONE = -1,
};

typedef enum TheForge_RendererApi {
  TheForge_RENDERER_API_D3D12 = 0,
  TheForge_RENDERER_API_VULKAN,
  TheForge_RENDERER_API_METAL,
  TheForge_RENDERER_API_XBOX_D3D12,
  TheForge_RENDERER_API_D3D11
} TheForge_RendererApi;

typedef enum TheForge_CmdPoolType {
  TheForge_CMD_POOL_DIRECT,
  TheForge_CMD_POOL_BUNDLE,
  TheForge_CMD_POOL_COPY,
  TheForge_CMD_POOL_COMPUTE,
  TheForge_MAX_CMD_TYPE
} TheForge_CmdPoolType;

typedef enum TheForge_QueueFlagBits {
  TheForge_QUEUE_FLAG_NONE = 0,
  TheForge_QUEUE_FLAG_DISABLE_GPU_TIMEOUT = 0x1,
  TheForge_MAX_QUEUE_FLAG
} TheForge_QueueFlagBits;
typedef uint8_t TheForge_QueueFlags;

typedef enum TheForge_QueuePriority {
  TheForge_QUEUE_PRIORITY_NORMAL = 0,
  TheForge_QUEUE_PRIORITY_HIGH,
  TheForge_QUEUE_PRIORITY_GLOBAL_REALTIME,
  TheForge_MAX_QUEUE_PRIORITY
} TheForge_QueuePriority;

typedef enum TheForge_LoadActionType {
  TheForge_LOAD_ACTION_DONTCARE = 0,
  TheForge_LOAD_ACTION_LOAD,
  TheForge_LOAD_ACTION_CLEAR,
  TheForge_MAX_LOAD_ACTION
} TheForge_LoadActionType;

typedef enum TheForge_ResourceStateFlagBits {
  TheForge_RESOURCE_STATE_UNDEFINED = 0,
  TheForge_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER = 0x1,
  TheForge_RESOURCE_STATE_INDEX_BUFFER = 0x2,
  TheForge_RESOURCE_STATE_RENDER_TARGET = 0x4,
  TheForge_RESOURCE_STATE_UNORDERED_ACCESS = 0x8,
  TheForge_RESOURCE_STATE_DEPTH_WRITE = 0x10,
  TheForge_RESOURCE_STATE_DEPTH_READ = 0x20,
  TheForge_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE = 0x40,
  TheForge_RESOURCE_STATE_SHADER_RESOURCE = 0x40 | 0x80,
  TheForge_RESOURCE_STATE_STREAM_OUT = 0x100,
  TheForge_RESOURCE_STATE_INDIRECT_ARGUMENT = 0x200,
  TheForge_RESOURCE_STATE_COPY_DEST = 0x400,
  TheForge_RESOURCE_STATE_COPY_SOURCE = 0x800,
  TheForge_RESOURCE_STATE_GENERIC_READ = (((((0x1 | 0x2) | 0x40) | 0x80) | 0x200) | 0x800),
  TheForge_RESOURCE_STATE_PRESENT = 0x4000,
  TheForge_RESOURCE_STATE_COMMON = 0x8000,
} TheForge_ResourceStateFlagBits;
typedef uint32_t TheForge_ResourceStateFlags;

typedef enum TheForge_ResourceMemoryUsage {
  /// No intended memory usage specified
      TheForge_RESOURCE_MEMORY_USAGE_UNKNOWN = 0,
  /// Memory will be used on device only, no need to be mapped on host.
      TheForge_RESOURCE_MEMORY_USAGE_GPU_ONLY = 1,
  /// Memory will be mapped on host. Could be used for transfer to device.
      TheForge_RESOURCE_MEMORY_USAGE_CPU_ONLY = 2,
  /// Memory will be used for frequent (dynamic) updates from host and reads on device.
      TheForge_RESOURCE_MEMORY_USAGE_CPU_TO_GPU = 3,
  /// Memory will be used for writing on device and readback on host.
      TheForge_RESOURCE_MEMORY_USAGE_GPU_TO_CPU = 4,
  TheForge_RESOURCE_MEMORY_USAGE_MAX_ENUM = 0x7FFFFFFF
} TheForge_ResourceMemoryUsage;

typedef enum TheForge_IndirectArgumentType {
  TheForge_INDIRECT_DRAW,
  TheForge_INDIRECT_DRAW_INDEX,
  TheForge_INDIRECT_DISPATCH,
  TheForge_INDIRECT_VERTEX_BUFFER,
  TheForge_INDIRECT_INDEX_BUFFER,
  TheForge_INDIRECT_CONSTANT,
  TheForge_INDIRECT_DESCRIPTOR_TABLE,        // only for vulkan
  TheForge_INDIRECT_PIPELINE,                // only for vulkan now, probally will add to dx when it comes to xbox
  TheForge_INDIRECT_CONSTANT_BUFFER_VIEW,    // only for dx
  TheForge_INDIRECT_SHADER_RESOURCE_VIEW,    // only for dx
  TheForge_INDIRECT_UNORDERED_ACCESS_VIEW    // only for dx
} TheForge_IndirectArgumentType;

typedef enum TheForge_DescriptorTypeFlagBits {
  TheForge_DESCRIPTOR_TYPE_UNDEFINED = 0,
  TheForge_DESCRIPTOR_TYPE_SAMPLER = 0x01,
  TheForge_DESCRIPTOR_TYPE_TEXTURE = (TheForge_DESCRIPTOR_TYPE_SAMPLER << 1),
  TheForge_DESCRIPTOR_TYPE_RW_TEXTURE = (TheForge_DESCRIPTOR_TYPE_TEXTURE << 1),
  TheForge_DESCRIPTOR_TYPE_BUFFER = (TheForge_DESCRIPTOR_TYPE_RW_TEXTURE << 1),
  TheForge_DESCRIPTOR_TYPE_BUFFER_RAW = (TheForge_DESCRIPTOR_TYPE_BUFFER | (TheForge_DESCRIPTOR_TYPE_BUFFER << 1)),
  TheForge_DESCRIPTOR_TYPE_RW_BUFFER = (TheForge_DESCRIPTOR_TYPE_BUFFER << 2),
  TheForge_DESCRIPTOR_TYPE_RW_BUFFER_RAW =
  (TheForge_DESCRIPTOR_TYPE_RW_BUFFER | (TheForge_DESCRIPTOR_TYPE_RW_BUFFER << 1)),
  TheForge_DESCRIPTOR_TYPE_UNIFORM_BUFFER = (TheForge_DESCRIPTOR_TYPE_RW_BUFFER << 2),
  TheForge_DESCRIPTOR_TYPE_VERTEX_BUFFER = (TheForge_DESCRIPTOR_TYPE_UNIFORM_BUFFER << 1),
  TheForge_DESCRIPTOR_TYPE_INDEX_BUFFER = (TheForge_DESCRIPTOR_TYPE_VERTEX_BUFFER << 1),
  TheForge_DESCRIPTOR_TYPE_INDIRECT_BUFFER = (TheForge_DESCRIPTOR_TYPE_INDEX_BUFFER << 1),
  TheForge_DESCRIPTOR_TYPE_ROOT_CONSTANT = (TheForge_DESCRIPTOR_TYPE_INDIRECT_BUFFER << 1),
  TheForge_DESCRIPTOR_TYPE_TEXTURE_CUBE =
  (TheForge_DESCRIPTOR_TYPE_TEXTURE | (TheForge_DESCRIPTOR_TYPE_ROOT_CONSTANT << 1)),
  TheForge_DESCRIPTOR_TYPE_RENDER_TARGET_ARRAY_SLICES = (TheForge_DESCRIPTOR_TYPE_ROOT_CONSTANT << 2),
  TheForge_DESCRIPTOR_TYPE_RENDER_TARGET_DEPTH_SLICES = (TheForge_DESCRIPTOR_TYPE_RENDER_TARGET_ARRAY_SLICES << 1),

  // vulkan only at the moment
      TheForge_DESCRIPTOR_TYPE_INPUT_ATTACHMENT = (TheForge_DESCRIPTOR_TYPE_RENDER_TARGET_DEPTH_SLICES << 1),
  TheForge_DESCRIPTOR_TYPE_TEXEL_BUFFER = (TheForge_DESCRIPTOR_TYPE_INPUT_ATTACHMENT << 1),
  TheForge_DESCRIPTOR_TYPE_RW_TEXEL_BUFFER = (TheForge_DESCRIPTOR_TYPE_TEXEL_BUFFER << 1),
} TheForge_DescriptorTypeFlagBits;
typedef uint32_t TheForge_DescriptorTypeFlags;

typedef enum TheForge_TextureDimension {
  TheForge_TEXTURE_DIM_UNDEFINED = 0,
  TheForge_TEXTURE_DIM_1D,
  TheForge_TEXTURE_DIM_2D,
  TheForge_TEXTURE_DIM_3D,
  TheForge_TEXTURE_DIM_1D_ARRAY,
  TheForge_TEXTURE_DIM_2D_ARRAY,
  TheForge_TEXTURE_DIM_CUBE,
} TheForge_TextureDimension;

typedef enum TheForge_SampleCount {
  TheForge_SAMPLE_COUNT_1 = 1,
  TheForge_SAMPLE_COUNT_2 = 2,
  TheForge_SAMPLE_COUNT_4 = 4,
  TheForge_SAMPLE_COUNT_8 = 8,
  TheForge_SAMPLE_COUNT_16 = 16,
} TheForge_SampleCount;

typedef enum TheForge_ShaderStageFlagBits {
  TheForge_SHADER_STAGE_NONE = 0,
  TheForge_SHADER_STAGE_VERT = 0X00000001,
  TheForge_SHADER_STAGE_TESC = 0X00000002,
  TheForge_SHADER_STAGE_TESE = 0X00000004,
  TheForge_SHADER_STAGE_GEOM = 0X00000008,
  TheForge_SHADER_STAGE_FRAG = 0X00000010,
  TheForge_SHADER_STAGE_ALL_GRAPHICS = 0X0000001F,
  TheForge_SHADER_STAGE_COMP = 0X00000020,
  TheForge_SHADER_STAGE_LIB = 0X000000040,
  TheForge_SHADER_STAGE_HULL = TheForge_SHADER_STAGE_TESC,
  TheForge_SHADER_STAGE_DOMN = TheForge_SHADER_STAGE_TESE,
  TheForge_SHADER_STAGE_COUNT = 7,
} TheForge_ShaderStageFlagBits;
// its used as both flags and and enum. Type makes it clear which is intended
typedef uint8_t TheForge_ShaderStage;
typedef uint8_t TheForge_ShaderStageFlags;

typedef enum TheForge_PrimitiveTopology {
  TheForge_PRIMITIVE_TOPO_POINT_LIST = 0,
  TheForge_PRIMITIVE_TOPO_LINE_LIST,
  TheForge_PRIMITIVE_TOPO_LINE_STRIP,
  TheForge_PRIMITIVE_TOPO_TRI_LIST,
  TheForge_PRIMITIVE_TOPO_TRI_STRIP,
  TheForge_PRIMITIVE_TOPO_PATCH_LIST,
  TheForge_PRIMITIVE_TOPO_COUNT,
} TheForge_PrimitiveTopology;

typedef enum TheForge_IndexType {
  TheForge_INDEX_TYPE_UINT32 = 0,
  TheForge_INDEX_TYPE_UINT16,
} TheForge_IndexType;

typedef enum TheForge_ShaderSemantic {
  TheForge_SEMANTIC_UNDEFINED = 0,
  TheForge_SEMANTIC_POSITION,
  TheForge_SEMANTIC_NORMAL,
  TheForge_SEMANTIC_COLOR,
  TheForge_SEMANTIC_TANGENT,
  TheForge_SEMANTIC_BITANGENT,
  TheForge_SEMANTIC_TEXCOORD0,
  TheForge_SEMANTIC_TEXCOORD1,
  TheForge_SEMANTIC_TEXCOORD2,
  TheForge_SEMANTIC_TEXCOORD3,
  TheForge_SEMANTIC_TEXCOORD4,
  TheForge_SEMANTIC_TEXCOORD5,
  TheForge_SEMANTIC_TEXCOORD6,
  TheForge_SEMANTIC_TEXCOORD7,
  TheForge_SEMANTIC_TEXCOORD8,
  TheForge_SEMANTIC_TEXCOORD9,
} TheForge_ShaderSemantic;

typedef enum TheForge_BlendConstant {
  TheForge_BC_ZERO = 0,
  TheForge_BC_ONE,
  TheForge_BC_SRC_COLOR,
  TheForge_BC_ONE_MINUS_SRC_COLOR,
  TheForge_BC_DST_COLOR,
  TheForge_BC_ONE_MINUS_DST_COLOR,
  TheForge_BC_SRC_ALPHA,
  TheForge_BC_ONE_MINUS_SRC_ALPHA,
  TheForge_BC_DST_ALPHA,
  TheForge_BC_ONE_MINUS_DST_ALPHA,
  TheForge_BC_SRC_ALPHA_SATURATE,
  TheForge_BC_BLEND_FACTOR,
  TheForge_BC_INV_BLEND_FACTOR,
  TheForge_MAX_BLEND_CONSTANTS
} TheForge_BlendConstant;

typedef enum TheForge_BlendMode {
  TheForge_BM_ADD,
  TheForge_BM_SUBTRACT,
  TheForge_BM_REVERSE_SUBTRACT,
  TheForge_BM_MIN,
  TheForge_BM_MAX,
  TheForge_MAX_BLEND_MODES,
} TheForge_BlendMode;

typedef enum TheForge_CompareMode {
  TheForge_CMP_NEVER,
  TheForge_CMP_LESS,
  TheForge_CMP_EQUAL,
  TheForge_CMP_LEQUAL,
  TheForge_CMP_GREATER,
  TheForge_CMP_NOTEQUAL,
  TheForge_CMP_GEQUAL,
  TheForge_CMP_ALWAYS,
  TheForge_MAX_COMPARE_MODES,
} TheForge_CompareMode;

typedef enum TheForge_StencilOp {
  TheForge_STENCIL_OP_KEEP,
  TheForge_STENCIL_OP_SET_ZERO,
  TheForge_STENCIL_OP_REPLACE,
  TheForge_STENCIL_OP_INVERT,
  TheForge_STENCIL_OP_INCR,
  TheForge_STENCIL_OP_DECR,
  TheForge_STENCIL_OP_INCR_SAT,
  TheForge_STENCIL_OP_DECR_SAT,
  TheForge_MAX_STENCIL_OPS,
} TheForge_StencilOp;

// Blend states are always attached to one of the eight or more render targets that
// are in a MRT
// Mask constants
typedef enum TheForge_BlendStateTargetsFlagBits {
  TheForge_BLEND_STATE_TARGET_0 = 0x1,
  TheForge_BLEND_STATE_TARGET_1 = 0x2,
  TheForge_BLEND_STATE_TARGET_2 = 0x4,
  TheForge_BLEND_STATE_TARGET_3 = 0x8,
  TheForge_BLEND_STATE_TARGET_4 = 0x10,
  TheForge_BLEND_STATE_TARGET_5 = 0x20,
  TheForge_BLEND_STATE_TARGET_6 = 0x40,
  TheForge_BLEND_STATE_TARGET_7 = 0x80,
  TheForge_BLEND_STATE_TARGET_ALL = 0xFF,
} TheForge_BlendStateTargetsFlagBits;
typedef uint8_t TheForge_BlendStateTargetsFlags;

typedef enum TheForge_CullMode {
  TheForge_CULL_MODE_NONE = 0,
  TheForge_CULL_MODE_BACK,
  TheForge_CULL_MODE_FRONT,
  TheForge_CULL_MODE_BOTH,
  TheForge_MAX_CULL_MODES
} TheForge_CullMode;

typedef enum TheForge_FrontFace {
  TheForge_FRONT_FACE_CCW = 0,
  TheForge_FRONT_FACE_CW
} TheForge_FrontFace;

typedef enum TheForge_FillMode {
  TheForge_FILL_MODE_SOLID,
  TheForge_FILL_MODE_WIREFRAME,
  TheForge_MAX_FILL_MODES
} TheForge_FillMode;

typedef enum TheForge_PipelineType {
  TheForge_PIPELINE_TYPE_UNDEFINED = 0,
  TheForge_PIPELINE_TYPE_COMPUTE,
  TheForge_PIPELINE_TYPE_GRAPHICS,
  TheForge_PIPELINE_TYPE_RAYTRACING,
  TheForge_PIPELINE_TYPE_COUNT,
} TheForge_PipelineType;

typedef enum TheForge_FilterType {
  TheForge_FILTER_NEAREST = 0,
  TheForge_FILTER_LINEAR,
} TheForge_FilterType;

typedef enum TheForge_AddressMode {
  TheForge_ADDRESS_MODE_MIRROR,
  TheForge_ADDRESS_MODE_REPEAT,
  TheForge_ADDRESS_MODE_CLAMP_TO_EDGE,
  TheForge_ADDRESS_MODE_CLAMP_TO_BORDER
} TheForge_AddressMode;

typedef enum TheForge_MipMapMode {
  TheForge_MIPMAP_MODE_NEAREST = 0,
  TheForge_MIPMAP_MODE_LINEAR
} TheForge_MipMapMode;

typedef enum TheForge_DepthStencilClearFlagBits {
  TheForge_CLEAR_DEPTH = 0x01,
  TheForge_CLEAR_STENCIL = 0x02
} TheForge_DepthStencilClearFlagBits;
typedef uint8_t TheForge_DepthStencilClearFlags;

typedef enum TheForge_BufferCreationFlagBits {
  /// Default flag (Buffer will use aliased memory, buffer will not be cpu accessible until mapBuffer is called)
      TheForge_BUFFER_CREATION_FLAG_NONE = 0x01,
  /// Buffer will allocate its own memory (COMMITTED resource)
      TheForge_BUFFER_CREATION_FLAG_OWN_MEMORY_BIT = 0x02,
  /// Buffer will be persistently mapped
      TheForge_BUFFER_CREATION_FLAG_PERSISTENT_MAP_BIT = 0x04,
  /// Use ESRAM to store this buffer
      TheForge_BUFFER_CREATION_FLAG_ESRAM = 0x08,
  /// Flag to specify not to allocate descriptors for the resource
      TheForge_BUFFER_CREATION_FLAG_NO_DESCRIPTOR_VIEW_CREATION = 0x10,
} TheForge_BufferCreationFlagBits;
typedef uint8_t TheForge_BufferCreationFlags;

typedef enum TheForge_TextureCreationFlagBits {
  /// Default flag (Texture will use default allocation strategy decided by the api specific allocator)
      TheForge_TEXTURE_CREATION_FLAG_NONE = 0,
  /// Texture will allocate its own memory (COMMITTED resource)
      TheForge_TEXTURE_CREATION_FLAG_OWN_MEMORY_BIT = 0x01,
  /// Texture will be allocated in memory which can be shared among multiple processes
      TheForge_TEXTURE_CREATION_FLAG_EXPORT_BIT = 0x02,
  /// Texture will be allocated in memory which can be shared among multiple gpus
      TheForge_TEXTURE_CREATION_FLAG_EXPORT_ADAPTER_BIT = 0x04,
  /// Texture will be imported from a handle created in another process
      TheForge_TEXTURE_CREATION_FLAG_IMPORT_BIT = 0x08,
  /// Use ESRAM to store this texture
      TheForge_TEXTURE_CREATION_FLAG_ESRAM = 0x10,
  /// Use on-tile memory to store this texture
      TheForge_TEXTURE_CREATION_FLAG_ON_TILE = 0x20,
  /// Prevent compression meta data from generating (XBox)
      TheForge_TEXTURE_CREATION_FLAG_NO_COMPRESSION = 0x40,
} TheForge_TextureCreationFlagBits;
typedef uint32_t TheForge_TextureCreationFlags;

typedef enum TheForge_GPUPresetLevel {
  TheForge_GPU_PRESET_NONE = 0,
  TheForge_GPU_PRESET_OFFICE,    //This means unsupported
  TheForge_GPU_PRESET_LOW,
  TheForge_GPU_PRESET_MEDIUM,
  TheForge_GPU_PRESET_HIGH,
  TheForge_GPU_PRESET_ULTRA,
  TheForge_GPU_PRESET_COUNT
} TheForge_GPUPresetLevel;

typedef enum TheForge_QueryType {
  TheForge_QUERY_TYPE_TIMESTAMP = 0,
  TheForge_QUERY_TYPE_PIPELINE_STATISTICS,
  TheForge_QUERY_TYPE_OCCLUSION,
  TheForge_QUERY_TYPE_COUNT,
} TheForge_QueryType;

typedef enum TheForge_DescriptorUpdateFrequency {
  TheForge_DESCRIPTOR_UPDATE_FREQ_NONE = 0,
  TheForge_DESCRIPTOR_UPDATE_FREQ_PER_FRAME,
  TheForge_DESCRIPTOR_UPDATE_FREQ_PER_BATCH,
  TheForge_DESCRIPTOR_UPDATE_FREQ_PER_DRAW,
  TheForge_DESCRIPTOR_UPDATE_FREQ_COUNT,
} TheForge_DescriptorUpdateFrequency;

typedef enum TheForge_FenceStatus {
  TheForge_FENCE_STATUS_COMPLETE = 0,
  TheForge_FENCE_STATUS_INCOMPLETE,
  TheForge_FENCE_STATUS_NOTSUBMITTED,
} TheForge_FenceStatus;

typedef enum TheForge_VertexAttribRate {
  TheForge_VERTEX_ATTRIB_RATE_VERTEX = 0,
  TheForge_VERTEX_ATTRIB_RATE_INSTANCE = 1,
  TheForge_VERTEX_ATTRIB_RATE_COUNT,
} TheForge_VertexAttribRate;

typedef enum TheForge_ShaderTarget {
  TheForge_shader_target_5_0,
  TheForge_shader_target_5_1,
  TheForge_shader_target_6_0,
} TheForge_ShaderTarget;

typedef enum TheForge_GpuMode {
  TheForge_GPU_MODE_SINGLE = 0,
  TheForge_GPU_MODE_LINKED,
  // #TODO GPU_MODE_UNLINKED,
} TheForge_GpuMode;

typedef enum TheForge_D3D_FEATURE_LEVEL {
  TheForge_D3D_FEATURE_LEVEL_9_1,
  TheForge_D3D_FEATURE_LEVEL_9_2,
  TheForge_D3D_FEATURE_LEVEL_9_3,
  TheForge_D3D_FEATURE_LEVEL_10_0,
  TheForge_D3D_FEATURE_LEVEL_10_1,
  TheForge_D3D_FEATURE_LEVEL_11_0,
  TheForge_D3D_FEATURE_LEVEL_11_1,
  TheForge_D3D_FEATURE_LEVEL_12_0,
  TheForge_D3D_FEATURE_LEVEL_12_1
} TheForge_D3D_FEATURE_LEVEL;

typedef enum TheForge_RootSignatureType {
  TheForge_ROOT_SIGNATURE_GRAPHICS_COMPUTE,
  TheForge_ROOT_SIGNATURE_RAYTRACING_LOCAL,
  TheForge_ROOT_SIGNATURE_RAYTRACING_GLOBAL,
} TheForge_RootSignatureType;

#endif //WYRD_THEFORGE_RENDERER_ENUMS_H
