#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3_shadercross/SDL_shadercross.h>
#include <stdio.h>

// Include generated SPIRV header files (compiled via CMake)
#include "triangle_vert.h"
#include "triangle_frag.h"

// Simple vertex with just position
typedef struct {
    float x, y;
} SimpleVertex;

// Window and graphics context
SDL_Window* window;
SDL_GPUDevice* device;

// Shaders
SDL_GPUShader* vertexShader;
SDL_GPUShader* fragmentShader;

// Graphics pipeline
SDL_GPUGraphicsPipeline* pipeline;

// Buffer
SDL_GPUBuffer* vertexBuffer;

// SPIRV bytecode is now loaded from generated header files:
// - triangleVertexSpirv[] from shaders/triangle_vert.h
// - triangleFragmentSpirv[] from shaders/triangle_frag.h
//
// Generated from GLSL source files:
// - shaders/triangle.vert (vertex shader)
// - shaders/triangle.frag (fragment shader)
//
// To recompile: cd shaders && ./compile_shaders.sh

int initSDL() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        return 0;
    }

    window = SDL_CreateWindow("Working SDL_shadercross Demo", 800, 600, SDL_WINDOW_VULKAN);
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

int compileShaders() {
    printf("🚀 Working SDL_shadercross Demo\n");
    printf("================================\n");
    printf("Compiling properly formatted SPIRV shaders...\n");

    // Vertex shader
    SDL_ShaderCross_SPIRV_Info vertexSpirvInfo = {
        .bytecode = reinterpret_cast<const Uint8*>(triangleVertexSpirv),
        .bytecode_size = sizeof(triangleVertexSpirv),
        .entrypoint = "main",
        .shader_stage = SDL_SHADERCROSS_SHADERSTAGE_VERTEX,
        .props = 0
    };

    SDL_ShaderCross_GraphicsShaderMetadata* vertexMetadata = SDL_ShaderCross_ReflectGraphicsSPIRV(
        vertexSpirvInfo.bytecode, vertexSpirvInfo.bytecode_size, 0);

    if (!vertexMetadata) {
        printf("Failed to reflect vertex shader metadata, using minimal resource info\n");
        SDL_ShaderCross_GraphicsShaderResourceInfo minimalResourceInfo = {};
        vertexShader = SDL_ShaderCross_CompileGraphicsShaderFromSPIRV(
            device, &vertexSpirvInfo, &minimalResourceInfo, 0);
    } else {
        vertexShader = SDL_ShaderCross_CompileGraphicsShaderFromSPIRV(
            device, &vertexSpirvInfo, &vertexMetadata->resource_info, 0);
        SDL_free(vertexMetadata);
    }

    if (!vertexShader) {
        printf("Failed to compile vertex shader! Error: %s\n", SDL_GetError());
        return 0;
    }
    printf("✅ Vertex shader compiled successfully\n");

    // Fragment shader
    SDL_ShaderCross_SPIRV_Info fragmentSpirvInfo = {
        .bytecode = reinterpret_cast<const Uint8*>(triangleFragmentSpirv),
        .bytecode_size = sizeof(triangleFragmentSpirv),
        .entrypoint = "main",
        .shader_stage = SDL_SHADERCROSS_SHADERSTAGE_FRAGMENT,
        .props = 0
    };

    SDL_ShaderCross_GraphicsShaderMetadata* fragmentMetadata = SDL_ShaderCross_ReflectGraphicsSPIRV(
        fragmentSpirvInfo.bytecode, fragmentSpirvInfo.bytecode_size, 0);

    if (!fragmentMetadata) {
        printf("Failed to reflect fragment shader metadata, using minimal resource info\n");
        SDL_ShaderCross_GraphicsShaderResourceInfo minimalResourceInfo = {};
        fragmentShader = SDL_ShaderCross_CompileGraphicsShaderFromSPIRV(
            device, &fragmentSpirvInfo, &minimalResourceInfo, 0);
    } else {
        fragmentShader = SDL_ShaderCross_CompileGraphicsShaderFromSPIRV(
            device, &fragmentSpirvInfo, &fragmentMetadata->resource_info, 0);
        SDL_free(fragmentMetadata);
    }

    if (!fragmentShader) {
        printf("Failed to compile fragment shader! Error: %s\n", SDL_GetError());
        return 0;
    }
    printf("✅ Fragment shader compiled successfully\n");

    printf("✅ SDL_shadercross compilation complete!\n");
    return 1;
}

int createPipeline() {
    // Single vertex attribute - just position
    SDL_GPUVertexAttribute vertexAttribute = {
        .location = 0,
        .buffer_slot = 0,
        .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
        .offset = 0
    };

    SDL_GPUVertexBufferDescription vertexBufferDesc = {
        .slot = 0,
        .pitch = sizeof(SimpleVertex),
        .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
        .instance_step_rate = 0
    };

    // Simple color target description
    SDL_GPUColorTargetDescription colorTargetDesc = {
        .format = SDL_GetGPUSwapchainTextureFormat(device, window),
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

    SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo = {
        .vertex_shader = vertexShader,
        .fragment_shader = fragmentShader,
        .vertex_input_state = {
            .vertex_buffer_descriptions = &vertexBufferDesc,
            .num_vertex_buffers = 1,
            .vertex_attributes = &vertexAttribute,
            .num_vertex_attributes = 1
        },
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

    pipeline = SDL_CreateGPUGraphicsPipeline(device, &pipelineCreateInfo);
    if (!pipeline) {
        printf("Failed to create graphics pipeline: %s\n", SDL_GetError());
        return 0;
    }

    printf("✅ Graphics pipeline created successfully!\n");
    return 1;
}

int createBuffer() {
    // Simple triangle vertices - just positions
    SimpleVertex vertices[] = {
        { -0.5f, -0.5f },  // Bottom-left
        {  0.5f, -0.5f },  // Bottom-right
        {  0.0f,  0.5f }   // Top-center
    };

    SDL_GPUBufferCreateInfo bufferCreateInfo = {
        .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
        .size = sizeof(vertices),
        .props = 0
    };

    vertexBuffer = SDL_CreateGPUBuffer(device, &bufferCreateInfo);
    if (!vertexBuffer) {
        printf("Failed to create vertex buffer: %s\n", SDL_GetError());
        return 0;
    }

    // Upload vertex data
    SDL_GPUTransferBufferCreateInfo transferBufferCreateInfo = {
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = sizeof(vertices),
        .props = 0
    };
    SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(device, &transferBufferCreateInfo);

    if (!transferBuffer) {
        printf("Failed to create transfer buffer: %s\n", SDL_GetError());
        return 0;
    }

    void* data = SDL_MapGPUTransferBuffer(device, transferBuffer, false);
    if (!data) {
        printf("Failed to map transfer buffer: %s\n", SDL_GetError());
        return 0;
    }

    SDL_memcpy(data, vertices, sizeof(vertices));
    SDL_UnmapGPUTransferBuffer(device, transferBuffer);

    SDL_GPUCommandBuffer* uploadCmdBuf = SDL_AcquireGPUCommandBuffer(device);
    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(uploadCmdBuf);

    SDL_GPUTransferBufferLocation src = {
        .transfer_buffer = transferBuffer,
        .offset = 0
    };

    SDL_GPUBufferRegion dst = {
        .buffer = vertexBuffer,
        .offset = 0,
        .size = sizeof(vertices)
    };

    SDL_UploadToGPUBuffer(copyPass, &src, &dst, false);
    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(uploadCmdBuf);

    SDL_ReleaseGPUTransferBuffer(device, transferBuffer);

    printf("✅ Vertex buffer created and uploaded\n");
    return 1;
}

void render() {
    SDL_GPUCommandBuffer* cmdBuf = SDL_AcquireGPUCommandBuffer(device);
    if (!cmdBuf) {
        printf("Failed to acquire command buffer: %s\n", SDL_GetError());
        return;
    }

    SDL_GPUTexture* swapchainTexture;
    if (!SDL_AcquireGPUSwapchainTexture(cmdBuf, window, &swapchainTexture, NULL, NULL)) {
        printf("Failed to acquire swapchain texture: %s\n", SDL_GetError());
        return;
    }

    if (swapchainTexture) {
        SDL_GPUColorTargetInfo colorTargetInfo = {
            .texture = swapchainTexture,
            .mip_level = 0,
            .layer_or_depth_plane = 0,
            .clear_color = { 0.1f, 0.1f, 0.2f, 1.0f }, // Dark blue background
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

        SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(cmdBuf, &colorTargetInfo, 1, NULL);

        SDL_BindGPUGraphicsPipeline(renderPass, pipeline);

        SDL_GPUBufferBinding vertexBinding = {
            .buffer = vertexBuffer,
            .offset = 0
        };
        SDL_BindGPUVertexBuffers(renderPass, 0, &vertexBinding, 1);

        SDL_DrawGPUPrimitives(renderPass, 3, 1, 0, 0);

        SDL_EndGPURenderPass(renderPass);
    }

    SDL_SubmitGPUCommandBuffer(cmdBuf);
}

void cleanup() {
    if (vertexBuffer) SDL_ReleaseGPUBuffer(device, vertexBuffer);
    if (pipeline) SDL_ReleaseGPUGraphicsPipeline(device, pipeline);
    if (fragmentShader) SDL_ReleaseGPUShader(device, fragmentShader);
    if (vertexShader) SDL_ReleaseGPUShader(device, vertexShader);
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

    if (!createPipeline()) {
        cleanup();
        return 1;
    }

    if (!createBuffer()) {
        cleanup();
        return 1;
    }

    printf("✅ Starting render loop - you should see a red triangle!\n");
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