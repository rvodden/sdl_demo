#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3_shadercross/SDL_shadercross.h>
#include <stdio.h>

// Include generated SPIRV header files
#include "fullscreen_vert.h"
#include "scene_vert.h"
#include "scene_frag.h"
#include "bright_pass_frag.h"
#include "blur_frag.h"
#include "composite_frag.h"
#include "copy_frag.h"

// Simple vertex with position
typedef struct {
    float x, y;
} Vertex;

// Uniform buffer for blur parameters
typedef struct {
    float directionX, directionY;
    float strength;
    float padding; // Align to 16 bytes
} BlurParams;

// Window and graphics context
SDL_Window* window;
SDL_GPUDevice* device;

// Render targets
SDL_GPUTexture* sceneTexture;
SDL_GPUTexture* brightTexture;
SDL_GPUTexture* blurTexture1;
SDL_GPUTexture* blurTexture2;

// Shaders
SDL_GPUShader* fullscreenVertexShader;
SDL_GPUShader* sceneVertexShader;
SDL_GPUShader* sceneFragmentShader;
SDL_GPUShader* brightPassFragmentShader;
SDL_GPUShader* blurFragmentShader;
SDL_GPUShader* compositeFragmentShader;
SDL_GPUShader* copyFragmentShader;

// Pipelines
SDL_GPUGraphicsPipeline* scenePipeline;
SDL_GPUGraphicsPipeline* brightPassPipeline;
SDL_GPUGraphicsPipeline* blurPipeline;
SDL_GPUGraphicsPipeline* compositePipeline;

// Buffers
SDL_GPUBuffer* rectVertexBuffer;
SDL_GPUBuffer* fullscreenQuadBuffer;
SDL_GPUBuffer* blurParamsBuffer;

// Samplers
SDL_GPUSampler* linearSampler;

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

int initSDL() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        return 0;
    }

    window = SDL_CreateWindow("Bloom Shader Demo", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_VULKAN);
    if (!window) {
        printf("SDL_CreateWindow failed: %s\n", SDL_GetError());
        return 0;
    }

    device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, NULL);
    if (!device) {
        printf("SDL_CreateGPUDevice failed: %s\n", SDL_GetError());
        return 0;
    }

    if (!SDL_ClaimWindowForGPUDevice(device, window)) {
        printf("SDL_ClaimWindowForGPUDevice failed: %s\n", SDL_GetError());
        return 0;
    }

    return 1;
}

SDL_GPUShader* compileShader(const uint32_t* spirvData, size_t spirvSize, SDL_ShaderCross_ShaderStage stage, const char* name) {
    SDL_ShaderCross_SPIRV_Info spirvInfo = {
        .bytecode = reinterpret_cast<const Uint8*>(spirvData),
        .bytecode_size = spirvSize,
        .entrypoint = "main",
        .shader_stage = stage,
        .props = 0
    };

    SDL_ShaderCross_GraphicsShaderMetadata* metadata = SDL_ShaderCross_ReflectGraphicsSPIRV(
        spirvInfo.bytecode, spirvInfo.bytecode_size, 0);

    SDL_GPUShader* shader;
    if (!metadata) {
        printf("Failed to reflect %s shader metadata, using minimal resource info\n", name);
        SDL_ShaderCross_GraphicsShaderResourceInfo minimalResourceInfo = {};
        shader = SDL_ShaderCross_CompileGraphicsShaderFromSPIRV(
            device, &spirvInfo, &minimalResourceInfo, 0);
    } else {
        shader = SDL_ShaderCross_CompileGraphicsShaderFromSPIRV(
            device, &spirvInfo, &metadata->resource_info, 0);
        SDL_free(metadata);
    }

    if (!shader) {
        printf("Failed to compile %s shader! Error: %s\n", name, SDL_GetError());
        return NULL;
    }
    printf("✅ %s shader compiled successfully\n", name);
    return shader;
}

int compileShaders() {
    printf("🚀 Bloom Shader Demo\n");
    printf("====================\n");
    printf("Compiling shaders...\n");

    fullscreenVertexShader = compileShader(fullscreenVertexSpirv, sizeof(fullscreenVertexSpirv),
        SDL_SHADERCROSS_SHADERSTAGE_VERTEX, "Fullscreen Vertex");
    if (!fullscreenVertexShader) return 0;

    sceneVertexShader = compileShader(sceneVertexSpirv, sizeof(sceneVertexSpirv),
        SDL_SHADERCROSS_SHADERSTAGE_VERTEX, "Scene Vertex");
    if (!sceneVertexShader) return 0;

    sceneFragmentShader = compileShader(sceneFragmentSpirv, sizeof(sceneFragmentSpirv),
        SDL_SHADERCROSS_SHADERSTAGE_FRAGMENT, "Scene Fragment");
    if (!sceneFragmentShader) return 0;

    brightPassFragmentShader = compileShader(brightPassFragmentSpirv, sizeof(brightPassFragmentSpirv),
        SDL_SHADERCROSS_SHADERSTAGE_FRAGMENT, "Bright Pass Fragment");
    if (!brightPassFragmentShader) return 0;

    blurFragmentShader = compileShader(blurFragmentSpirv, sizeof(blurFragmentSpirv),
        SDL_SHADERCROSS_SHADERSTAGE_FRAGMENT, "Blur Fragment");
    if (!blurFragmentShader) return 0;

    compositeFragmentShader = compileShader(compositeFragmentSpirv, sizeof(compositeFragmentSpirv),
        SDL_SHADERCROSS_SHADERSTAGE_FRAGMENT, "Composite Fragment");
    if (!compositeFragmentShader) return 0;

    copyFragmentShader = compileShader(copyFragmentSpirv, sizeof(copyFragmentSpirv),
        SDL_SHADERCROSS_SHADERSTAGE_FRAGMENT, "Copy Fragment");
    if (!copyFragmentShader) return 0;

    printf("✅ All shaders compiled successfully!\n");
    return 1;
}

int createRenderTargets() {
    SDL_GPUTextureCreateInfo textureCreateInfo = {
        .type = SDL_GPU_TEXTURETYPE_2D,
        .format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
        .usage = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET | SDL_GPU_TEXTUREUSAGE_SAMPLER,
        .width = WINDOW_WIDTH,
        .height = WINDOW_HEIGHT,
        .layer_count_or_depth = 1,
        .num_levels = 1,
        .sample_count = SDL_GPU_SAMPLECOUNT_1,
        .props = 0
    };

    sceneTexture = SDL_CreateGPUTexture(device, &textureCreateInfo);
    brightTexture = SDL_CreateGPUTexture(device, &textureCreateInfo);
    blurTexture1 = SDL_CreateGPUTexture(device, &textureCreateInfo);
    blurTexture2 = SDL_CreateGPUTexture(device, &textureCreateInfo);

    if (!sceneTexture || !brightTexture || !blurTexture1 || !blurTexture2) {
        printf("Failed to create render targets: %s\n", SDL_GetError());
        return 0;
    }

    printf("✅ Render targets created successfully\n");
    return 1;
}

int createSampler() {
    SDL_GPUSamplerCreateInfo samplerCreateInfo = {};
    samplerCreateInfo.min_filter = SDL_GPU_FILTER_LINEAR;
    samplerCreateInfo.mag_filter = SDL_GPU_FILTER_LINEAR;
    samplerCreateInfo.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
    samplerCreateInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerCreateInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerCreateInfo.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;

    linearSampler = SDL_CreateGPUSampler(device, &samplerCreateInfo);
    if (!linearSampler) {
        printf("Failed to create sampler: %s\n", SDL_GetError());
        return 0;
    }

    printf("✅ Sampler created successfully\n");
    return 1;
}

SDL_GPUGraphicsPipeline* createRenderTargetPipeline(SDL_GPUShader* vertex, SDL_GPUShader* fragment,
                                                   SDL_GPUVertexBufferDescription* vertexDesc,
                                                   SDL_GPUVertexAttribute* vertexAttr,
                                                   Uint32 numVertexBuffers, Uint32 numVertexAttrs) {
    SDL_GPUColorTargetDescription colorTargetDesc = {
        .format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
        .blend_state = {
            .src_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE,
            .dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ZERO,
            .color_blend_op = SDL_GPU_BLENDOP_ADD,
            .src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE,
            .dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ZERO,
            .alpha_blend_op = SDL_GPU_BLENDOP_ADD,
            .color_write_mask = 0xF,
            .enable_blend = false,
            .enable_color_write_mask = false,
            .padding1 = 0,
            .padding2 = 0
        }
    };

    SDL_GPUVertexInputState vertexInputState = {};
    if (numVertexBuffers > 0 && vertexDesc != nullptr) {
        vertexInputState.vertex_buffer_descriptions = vertexDesc;
        vertexInputState.num_vertex_buffers = numVertexBuffers;
    }
    if (numVertexAttrs > 0 && vertexAttr != nullptr) {
        vertexInputState.vertex_attributes = vertexAttr;
        vertexInputState.num_vertex_attributes = numVertexAttrs;
    }

    SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo = {
        .vertex_shader = vertex,
        .fragment_shader = fragment,
        .vertex_input_state = vertexInputState,
        .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        .rasterizer_state = {
            .fill_mode = SDL_GPU_FILLMODE_FILL,
            .cull_mode = SDL_GPU_CULLMODE_NONE,
            .front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE,
            .depth_bias_constant_factor = 0.0f,
            .depth_bias_clamp = 0.0f,
            .depth_bias_slope_factor = 0.0f,
            .enable_depth_bias = false,
            .enable_depth_clip = false,
            .padding1 = 0,
            .padding2 = 0
        },
        .multisample_state = {
            .sample_count = SDL_GPU_SAMPLECOUNT_1,
            .sample_mask = 0,
            .enable_mask = false,
            .padding1 = 0,
            .padding2 = 0,
            .padding3 = 0
        },
        .depth_stencil_state = {
            .compare_op = SDL_GPU_COMPAREOP_ALWAYS,
            .back_stencil_state = {},
            .front_stencil_state = {},
            .compare_mask = 0,
            .write_mask = 0,
            .enable_depth_test = false,
            .enable_depth_write = false,
            .enable_stencil_test = false,
            .padding1 = 0,
            .padding2 = 0,
            .padding3 = 0
        },
        .target_info = {
            .color_target_descriptions = &colorTargetDesc,
            .num_color_targets = 1,
            .depth_stencil_format = SDL_GPU_TEXTUREFORMAT_INVALID,
            .has_depth_stencil_target = false,
            .padding1 = 0,
            .padding2 = 0,
            .padding3 = 0
        },
        .props = 0
    };

    return SDL_CreateGPUGraphicsPipeline(device, &pipelineCreateInfo);
}

SDL_GPUGraphicsPipeline* createSwapchainPipeline(SDL_GPUShader* vertex, SDL_GPUShader* fragment,
                                                SDL_GPUVertexBufferDescription* vertexDesc,
                                                SDL_GPUVertexAttribute* vertexAttr,
                                                Uint32 numVertexBuffers, Uint32 numVertexAttrs) {
    SDL_GPUTextureFormat swapchainFormat = SDL_GetGPUSwapchainTextureFormat(device, window);
    printf("Swapchain format: %d\n", swapchainFormat);

    SDL_GPUColorTargetDescription colorTargetDesc = {
        .format = swapchainFormat,
        .blend_state = {
            .src_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE,
            .dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ZERO,
            .color_blend_op = SDL_GPU_BLENDOP_ADD,
            .src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE,
            .dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ZERO,
            .alpha_blend_op = SDL_GPU_BLENDOP_ADD,
            .color_write_mask = 0xF,
            .enable_blend = false,
            .enable_color_write_mask = false,
            .padding1 = 0,
            .padding2 = 0
        }
    };

    SDL_GPUVertexInputState vertexInputState = {};
    if (numVertexBuffers > 0 && vertexDesc != nullptr) {
        vertexInputState.vertex_buffer_descriptions = vertexDesc;
        vertexInputState.num_vertex_buffers = numVertexBuffers;
    }
    if (numVertexAttrs > 0 && vertexAttr != nullptr) {
        vertexInputState.vertex_attributes = vertexAttr;
        vertexInputState.num_vertex_attributes = numVertexAttrs;
    }

    SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo = {
        .vertex_shader = vertex,
        .fragment_shader = fragment,
        .vertex_input_state = vertexInputState,
        .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        .rasterizer_state = {
            .fill_mode = SDL_GPU_FILLMODE_FILL,
            .cull_mode = SDL_GPU_CULLMODE_NONE,
            .front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE,
            .depth_bias_constant_factor = 0.0f,
            .depth_bias_clamp = 0.0f,
            .depth_bias_slope_factor = 0.0f,
            .enable_depth_bias = false,
            .enable_depth_clip = false,
            .padding1 = 0,
            .padding2 = 0
        },
        .multisample_state = {
            .sample_count = SDL_GPU_SAMPLECOUNT_1,
            .sample_mask = 0,
            .enable_mask = false,
            .padding1 = 0,
            .padding2 = 0,
            .padding3 = 0
        },
        .depth_stencil_state = {
            .compare_op = SDL_GPU_COMPAREOP_ALWAYS,
            .back_stencil_state = {},
            .front_stencil_state = {},
            .compare_mask = 0,
            .write_mask = 0,
            .enable_depth_test = false,
            .enable_depth_write = false,
            .enable_stencil_test = false,
            .padding1 = 0,
            .padding2 = 0,
            .padding3 = 0
        },
        .target_info = {
            .color_target_descriptions = &colorTargetDesc,
            .num_color_targets = 1,
            .depth_stencil_format = SDL_GPU_TEXTUREFORMAT_INVALID,
            .has_depth_stencil_target = false,
            .padding1 = 0,
            .padding2 = 0,
            .padding3 = 0
        },
        .props = 0
    };

    return SDL_CreateGPUGraphicsPipeline(device, &pipelineCreateInfo);
}

int createPipelines() {
    // Scene pipeline (renders the white rectangle)
    SDL_GPUVertexAttribute sceneVertexAttribute = {
        .location = 0,
        .buffer_slot = 0,
        .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
        .offset = 0
    };

    SDL_GPUVertexBufferDescription sceneVertexBufferDesc = {
        .slot = 0,
        .pitch = sizeof(Vertex),
        .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
        .instance_step_rate = 0
    };

    // Create scene pipeline for rendering to texture
    scenePipeline = createRenderTargetPipeline(sceneVertexShader, sceneFragmentShader,
                                               &sceneVertexBufferDesc, &sceneVertexAttribute, 1, 1);
    if (!scenePipeline) {
        printf("Failed to create scene pipeline: %s\n", SDL_GetError());
        return 0;
    }
    printf("✅ Scene pipeline created successfully\n");

    // Create composite pipeline for copying scene texture to screen
    SDL_GPUVertexAttribute fullscreenVertexAttribute = {
        .location = 0,
        .buffer_slot = 0,
        .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
        .offset = 0
    };

    SDL_GPUVertexBufferDescription fullscreenVertexBufferDesc = {
        .slot = 0,
        .pitch = sizeof(Vertex),
        .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
        .instance_step_rate = 0
    };

    // Remove unused variable suppression since we're using them now

    // Create bright pass pipeline (using fullscreen vertex shader)
    brightPassPipeline = createRenderTargetPipeline(fullscreenVertexShader, brightPassFragmentShader,
                                                   &fullscreenVertexBufferDesc, &fullscreenVertexAttribute, 1, 1);
    if (!brightPassPipeline) {
        printf("Failed to create bright pass pipeline: %s\n", SDL_GetError());
        return 0;
    }

    // Create blur pipeline
    blurPipeline = createRenderTargetPipeline(fullscreenVertexShader, blurFragmentShader,
                                            &fullscreenVertexBufferDesc, &fullscreenVertexAttribute, 1, 1);
    if (!blurPipeline) {
        printf("Failed to create blur pipeline: %s\n", SDL_GetError());
        return 0;
    }

    // Create composite pipeline for final output to swapchain
    compositePipeline = createSwapchainPipeline(fullscreenVertexShader, compositeFragmentShader,
                                               &fullscreenVertexBufferDesc, &fullscreenVertexAttribute, 1, 1);
    if (!compositePipeline) {
        printf("Failed to create composite pipeline: %s\n", SDL_GetError());
        return 0;
    }

    printf("✅ All pipelines created successfully\n");
    return 1;
}

int createBuffers() {
    // Create rectangle vertices (white rectangle in the center)
    Vertex rectVertices[] = {
        { -0.3f, -0.3f }, // Bottom-left
        {  0.3f, -0.3f }, // Bottom-right
        { -0.3f,  0.3f }, // Top-left
        {  0.3f, -0.3f }, // Bottom-right
        {  0.3f,  0.3f }, // Top-right
        { -0.3f,  0.3f }  // Top-left
    };

    // Create fullscreen quad vertices (for post-processing)
    Vertex fullscreenVertices[] = {
        { -1.0f, -1.0f }, // Bottom-left
        {  1.0f, -1.0f }, // Bottom-right
        { -1.0f,  1.0f }, // Top-left
        {  1.0f, -1.0f }, // Bottom-right
        {  1.0f,  1.0f }, // Top-right
        { -1.0f,  1.0f }  // Top-left
    };

    SDL_GPUBufferCreateInfo bufferCreateInfo = {
        .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
        .size = sizeof(rectVertices),
        .props = 0
    };

    rectVertexBuffer = SDL_CreateGPUBuffer(device, &bufferCreateInfo);
    if (!rectVertexBuffer) {
        printf("Failed to create vertex buffer: %s\n", SDL_GetError());
        return 0;
    }

    // Upload vertex data
    SDL_GPUTransferBufferCreateInfo transferBufferCreateInfo = {
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = sizeof(rectVertices),
        .props = 0
    };
    SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(device, &transferBufferCreateInfo);

    void* data = SDL_MapGPUTransferBuffer(device, transferBuffer, false);
    SDL_memcpy(data, rectVertices, sizeof(rectVertices));
    SDL_UnmapGPUTransferBuffer(device, transferBuffer);

    SDL_GPUCommandBuffer* uploadCmdBuf = SDL_AcquireGPUCommandBuffer(device);
    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(uploadCmdBuf);

    SDL_GPUTransferBufferLocation src = { .transfer_buffer = transferBuffer, .offset = 0 };
    SDL_GPUBufferRegion dst = { .buffer = rectVertexBuffer, .offset = 0, .size = sizeof(rectVertices) };

    SDL_UploadToGPUBuffer(copyPass, &src, &dst, false);
    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(uploadCmdBuf);
    SDL_ReleaseGPUTransferBuffer(device, transferBuffer);

    // Create fullscreen quad buffer
    bufferCreateInfo.size = sizeof(fullscreenVertices);
    fullscreenQuadBuffer = SDL_CreateGPUBuffer(device, &bufferCreateInfo);
    if (!fullscreenQuadBuffer) {
        printf("Failed to create fullscreen quad buffer: %s\n", SDL_GetError());
        return 0;
    }

    // Upload fullscreen quad data
    transferBufferCreateInfo.size = sizeof(fullscreenVertices);
    transferBuffer = SDL_CreateGPUTransferBuffer(device, &transferBufferCreateInfo);

    data = SDL_MapGPUTransferBuffer(device, transferBuffer, false);
    SDL_memcpy(data, fullscreenVertices, sizeof(fullscreenVertices));
    SDL_UnmapGPUTransferBuffer(device, transferBuffer);

    uploadCmdBuf = SDL_AcquireGPUCommandBuffer(device);
    copyPass = SDL_BeginGPUCopyPass(uploadCmdBuf);

    src.transfer_buffer = transferBuffer;
    dst.buffer = fullscreenQuadBuffer;
    dst.size = sizeof(fullscreenVertices);

    SDL_UploadToGPUBuffer(copyPass, &src, &dst, false);
    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(uploadCmdBuf);
    SDL_ReleaseGPUTransferBuffer(device, transferBuffer);

    // Create blur parameters buffer
    bufferCreateInfo.usage = SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ;
    bufferCreateInfo.size = sizeof(BlurParams);
    blurParamsBuffer = SDL_CreateGPUBuffer(device, &bufferCreateInfo);

    if (!blurParamsBuffer) {
        printf("Failed to create blur params buffer: %s\n", SDL_GetError());
        return 0;
    }

    printf("✅ Buffers created and uploaded\n");
    return 1;
}

void updateBlurParams(float dirX, float dirY, float strength) {
    BlurParams params = { dirX, dirY, strength, 0.0f };

    SDL_GPUTransferBufferCreateInfo transferBufferCreateInfo = {
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = sizeof(BlurParams),
        .props = 0
    };
    SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(device, &transferBufferCreateInfo);

    void* data = SDL_MapGPUTransferBuffer(device, transferBuffer, false);
    SDL_memcpy(data, &params, sizeof(BlurParams));
    SDL_UnmapGPUTransferBuffer(device, transferBuffer);

    SDL_GPUCommandBuffer* uploadCmdBuf = SDL_AcquireGPUCommandBuffer(device);
    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(uploadCmdBuf);

    SDL_GPUTransferBufferLocation src = { .transfer_buffer = transferBuffer, .offset = 0 };
    SDL_GPUBufferRegion dst = { .buffer = blurParamsBuffer, .offset = 0, .size = sizeof(BlurParams) };

    SDL_UploadToGPUBuffer(copyPass, &src, &dst, false);
    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(uploadCmdBuf);
    SDL_ReleaseGPUTransferBuffer(device, transferBuffer);
}

void render() {
    SDL_GPUCommandBuffer* cmdBuf = SDL_AcquireGPUCommandBuffer(device);
    if (!cmdBuf) {
        printf("Failed to acquire command buffer\n");
        return;
    }

    // STEP 1: Render scene to sceneTexture
    SDL_GPUColorTargetInfo sceneTarget = {
        .texture = sceneTexture,
        .mip_level = 0,
        .layer_or_depth_plane = 0,
        .clear_color = { 0.0f, 0.0f, 0.0f, 1.0f },
        .load_op = SDL_GPU_LOADOP_CLEAR,
        .store_op = SDL_GPU_STOREOP_STORE,
        .resolve_texture = NULL,
        .resolve_mip_level = 0,
        .resolve_layer = 0,
        .cycle = false,
        .cycle_resolve_texture = false,
        .padding1 = 0,
        .padding2 = 0
    };

    SDL_GPURenderPass* scenePass = SDL_BeginGPURenderPass(cmdBuf, &sceneTarget, 1, NULL);
    SDL_BindGPUGraphicsPipeline(scenePass, scenePipeline);
    SDL_GPUBufferBinding rectBinding = { .buffer = rectVertexBuffer, .offset = 0 };
    SDL_BindGPUVertexBuffers(scenePass, 0, &rectBinding, 1);
    SDL_DrawGPUPrimitives(scenePass, 6, 1, 0, 0);
    SDL_EndGPURenderPass(scenePass);

    // STEP 2: Extract bright areas (sceneTexture -> brightTexture)
    SDL_GPUColorTargetInfo brightTarget = {
        .texture = brightTexture,
        .mip_level = 0,
        .layer_or_depth_plane = 0,
        .clear_color = { 0.0f, 0.0f, 0.0f, 1.0f },
        .load_op = SDL_GPU_LOADOP_CLEAR,
        .store_op = SDL_GPU_STOREOP_STORE,
        .resolve_texture = NULL,
        .resolve_mip_level = 0,
        .resolve_layer = 0,
        .cycle = false,
        .cycle_resolve_texture = false,
        .padding1 = 0,
        .padding2 = 0
    };

    SDL_GPURenderPass* brightPass = SDL_BeginGPURenderPass(cmdBuf, &brightTarget, 1, NULL);
    SDL_BindGPUGraphicsPipeline(brightPass, brightPassPipeline);
    SDL_GPUTextureSamplerBinding sceneBinding = { .texture = sceneTexture, .sampler = linearSampler };
    SDL_BindGPUFragmentSamplers(brightPass, 0, &sceneBinding, 1);
    SDL_GPUBufferBinding quadBinding = { .buffer = fullscreenQuadBuffer, .offset = 0 };
    SDL_BindGPUVertexBuffers(brightPass, 0, &quadBinding, 1);
    SDL_DrawGPUPrimitives(brightPass, 6, 1, 0, 0);
    SDL_EndGPURenderPass(brightPass);

    // STEP 3 & 4: Blur passes (horizontal then vertical)
    updateBlurParams(1.0f, 0.0f, 2.0f); // Horizontal blur
    SDL_GPUColorTargetInfo blur1Target = {
        .texture = blurTexture1,
        .mip_level = 0,
        .layer_or_depth_plane = 0,
        .clear_color = { 0.0f, 0.0f, 0.0f, 1.0f },
        .load_op = SDL_GPU_LOADOP_CLEAR,
        .store_op = SDL_GPU_STOREOP_STORE,
        .resolve_texture = NULL,
        .resolve_mip_level = 0,
        .resolve_layer = 0,
        .cycle = false,
        .cycle_resolve_texture = false,
        .padding1 = 0,
        .padding2 = 0
    };

    SDL_GPURenderPass* blur1Pass = SDL_BeginGPURenderPass(cmdBuf, &blur1Target, 1, NULL);
    SDL_BindGPUGraphicsPipeline(blur1Pass, blurPipeline);
    SDL_GPUTextureSamplerBinding brightBinding = { .texture = brightTexture, .sampler = linearSampler };
    SDL_BindGPUFragmentSamplers(blur1Pass, 0, &brightBinding, 1);
    SDL_BindGPUFragmentStorageBuffers(blur1Pass, 1, &blurParamsBuffer, 1);
    SDL_BindGPUVertexBuffers(blur1Pass, 0, &quadBinding, 1);
    SDL_DrawGPUPrimitives(blur1Pass, 6, 1, 0, 0);
    SDL_EndGPURenderPass(blur1Pass);

    updateBlurParams(0.0f, 1.0f, 2.0f); // Vertical blur
    SDL_GPUColorTargetInfo blur2Target = {
        .texture = blurTexture2,
        .mip_level = 0,
        .layer_or_depth_plane = 0,
        .clear_color = { 0.0f, 0.0f, 0.0f, 1.0f },
        .load_op = SDL_GPU_LOADOP_CLEAR,
        .store_op = SDL_GPU_STOREOP_STORE,
        .resolve_texture = NULL,
        .resolve_mip_level = 0,
        .resolve_layer = 0,
        .cycle = false,
        .cycle_resolve_texture = false,
        .padding1 = 0,
        .padding2 = 0
    };

    SDL_GPURenderPass* blur2Pass = SDL_BeginGPURenderPass(cmdBuf, &blur2Target, 1, NULL);
    SDL_BindGPUGraphicsPipeline(blur2Pass, blurPipeline);
    SDL_GPUTextureSamplerBinding blur1Binding = { .texture = blurTexture1, .sampler = linearSampler };
    SDL_BindGPUFragmentSamplers(blur2Pass, 0, &blur1Binding, 1);
    SDL_BindGPUFragmentStorageBuffers(blur2Pass, 1, &blurParamsBuffer, 1);
    SDL_BindGPUVertexBuffers(blur2Pass, 0, &quadBinding, 1);
    SDL_DrawGPUPrimitives(blur2Pass, 6, 1, 0, 0);
    SDL_EndGPURenderPass(blur2Pass);

    // STEP 5: Final composite (sceneTexture + blurTexture2 -> swapchain)
    SDL_GPUTexture* swapchainTexture;
    if (!SDL_AcquireGPUSwapchainTexture(cmdBuf, window, &swapchainTexture, NULL, NULL)) {
        printf("Failed to acquire swapchain texture\n");
        return;
    }

    if (swapchainTexture) {
        SDL_GPUColorTargetInfo finalTarget = {
            .texture = swapchainTexture,
            .mip_level = 0,
            .layer_or_depth_plane = 0,
            .clear_color = { 0.0f, 0.0f, 0.0f, 1.0f },
            .load_op = SDL_GPU_LOADOP_CLEAR,
            .store_op = SDL_GPU_STOREOP_STORE,
            .resolve_texture = NULL,
            .resolve_mip_level = 0,
            .resolve_layer = 0,
            .cycle = false,
            .cycle_resolve_texture = false,
            .padding1 = 0,
            .padding2 = 0
        };

        SDL_GPURenderPass* finalPass = SDL_BeginGPURenderPass(cmdBuf, &finalTarget, 1, NULL);
        SDL_BindGPUGraphicsPipeline(finalPass, compositePipeline);
        SDL_GPUTextureSamplerBinding compositeBindings[] = {
            { .texture = sceneTexture, .sampler = linearSampler },
            { .texture = blurTexture2, .sampler = linearSampler }
        };
        SDL_BindGPUFragmentSamplers(finalPass, 0, compositeBindings, 2);
        SDL_BindGPUVertexBuffers(finalPass, 0, &quadBinding, 1);
        SDL_DrawGPUPrimitives(finalPass, 6, 1, 0, 0);
        SDL_EndGPURenderPass(finalPass);
    }

    SDL_SubmitGPUCommandBuffer(cmdBuf);
}

void cleanup() {
    if (rectVertexBuffer) SDL_ReleaseGPUBuffer(device, rectVertexBuffer);
    if (fullscreenQuadBuffer) SDL_ReleaseGPUBuffer(device, fullscreenQuadBuffer);
    if (blurParamsBuffer) SDL_ReleaseGPUBuffer(device, blurParamsBuffer);

    if (linearSampler) SDL_ReleaseGPUSampler(device, linearSampler);

    if (sceneTexture) SDL_ReleaseGPUTexture(device, sceneTexture);
    if (brightTexture) SDL_ReleaseGPUTexture(device, brightTexture);
    if (blurTexture1) SDL_ReleaseGPUTexture(device, blurTexture1);
    if (blurTexture2) SDL_ReleaseGPUTexture(device, blurTexture2);

    if (scenePipeline) SDL_ReleaseGPUGraphicsPipeline(device, scenePipeline);
    if (brightPassPipeline) SDL_ReleaseGPUGraphicsPipeline(device, brightPassPipeline);
    if (blurPipeline) SDL_ReleaseGPUGraphicsPipeline(device, blurPipeline);
    if (compositePipeline) SDL_ReleaseGPUGraphicsPipeline(device, compositePipeline);

    if (fullscreenVertexShader) SDL_ReleaseGPUShader(device, fullscreenVertexShader);
    if (sceneVertexShader) SDL_ReleaseGPUShader(device, sceneVertexShader);
    if (sceneFragmentShader) SDL_ReleaseGPUShader(device, sceneFragmentShader);
    if (brightPassFragmentShader) SDL_ReleaseGPUShader(device, brightPassFragmentShader);
    if (blurFragmentShader) SDL_ReleaseGPUShader(device, blurFragmentShader);
    if (compositeFragmentShader) SDL_ReleaseGPUShader(device, compositeFragmentShader);
    if (copyFragmentShader) SDL_ReleaseGPUShader(device, copyFragmentShader);

    if (device) SDL_DestroyGPUDevice(device);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

int main() {
    if (!initSDL()) {
        cleanup();
        return 1;
    }

    if (!compileShaders()) {
        cleanup();
        return 1;
    }

    if (!createRenderTargets()) {
        cleanup();
        return 1;
    }

    if (!createSampler()) {
        cleanup();
        return 1;
    }

    if (!createPipelines()) {
        cleanup();
        return 1;
    }

    if (!createBuffers()) {
        cleanup();
        return 1;
    }

    printf("✅ Starting bloom demo - you should see a white rectangle with yellow bloom!\n");
    printf("Press ESC or close window to exit.\n");

    // Render loop
    SDL_Event event;
    bool running = true;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
            if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE) {
                running = false;
            }
        }

        render();
        SDL_Delay(16); // ~60 FPS
    }

    cleanup();
    return 0;
}