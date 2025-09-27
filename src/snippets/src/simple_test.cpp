#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <stdio.h>

SDL_Window* window;
SDL_GPUDevice* device;

int main() {
    // Test if basic SDL3 GPU pipeline creation works at all
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        printf("SDL Init failed: %s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("Test", 800, 600, SDL_WINDOW_VULKAN);
    if (!window) {
        printf("Window creation failed: %s\n", SDL_GetError());
        return 1;
    }

    device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, NULL);
    if (!device) {
        printf("GPU device creation failed: %s\n", SDL_GetError());
        return 1;
    }

    if (!SDL_ClaimWindowForGPUDevice(device, window)) {
        printf("Window claim failed: %s\n", SDL_GetError());
        return 1;
    }

    printf("✓ SDL3 GPU setup successful\n");
    printf("Swapchain format: %d\n", SDL_GetGPUSwapchainTextureFormat(device, window));

    SDL_DestroyGPUDevice(device);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}