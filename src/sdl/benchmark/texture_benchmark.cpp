#include <benchmark/benchmark.h>
#include <vector>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "sdl/window.h"
#include "sdl/renderer.h"
#include "sdl/texture.h"
#include "sdl/rectangle.h"

// =============================================================================
// TEXTURE BENCHMARKS - SDL++ vs Pure SDL3
// =============================================================================

class TextureFixture : public benchmark::Fixture {
protected:
    SDL_Window* sdlWindow = nullptr;
    SDL_Renderer* sdlRenderer = nullptr;
    sdl::Window* sdlppWindow = nullptr;
    sdl::Renderer* sdlppRenderer = nullptr;

public:
    void SetUp(const ::benchmark::State& state) override {
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            throw std::runtime_error("SDL_Init failed");
        }
        
        // Setup pure SDL3
        sdlWindow = SDL_CreateWindow("Benchmark", 800, 600, 0);
        if (!sdlWindow) {
            throw std::runtime_error("SDL_CreateWindow failed");
        }
        sdlRenderer = SDL_CreateRenderer(sdlWindow, nullptr);
        if (!sdlRenderer) {
            throw std::runtime_error("SDL_CreateRenderer failed");
        }
        
        // Setup SDL++ wrapper
        sdlppWindow = new sdl::Window("Benchmark", 800, 600, 0);
        sdlppRenderer = new sdl::Renderer(*sdlppWindow);
    }
    
    void TearDown(const ::benchmark::State& state) override {
        delete sdlppRenderer;
        delete sdlppWindow;
        
        SDL_DestroyRenderer(sdlRenderer);
        SDL_DestroyWindow(sdlWindow);
        SDL_Quit();
    }
};

BENCHMARK_DEFINE_F(TextureFixture, TextureCreateFromPixels_SDL3)(benchmark::State& state) {
    std::vector<uint32_t> pixels(256 * 256, 0xFFFF0000); // Red pixels
    
    for (auto _ : state) {
        SDL_Texture* texture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_RGBA8888, 
                                                  SDL_TEXTUREACCESS_STATIC, 256, 256);
        if (texture) {
            SDL_UpdateTexture(texture, nullptr, pixels.data(), 256 * 4);
            benchmark::DoNotOptimize(texture);
            SDL_DestroyTexture(texture);
        }
    }
}

BENCHMARK_DEFINE_F(TextureFixture, TextureCreateFromPixels_SDLpp)(benchmark::State& state) {
    std::vector<uint32_t> pixels(256 * 256, 0xFFFF0000); // Red pixels
    
    for (auto _ : state) {
        sdl::Texture texture(*sdlppRenderer, 256, 256, pixels.data());
        benchmark::DoNotOptimize(texture);
    }
}

BENCHMARK_DEFINE_F(TextureFixture, TextureRender_SDL3)(benchmark::State& state) {
    std::vector<uint32_t> pixels(64 * 64, 0xFF00FF00); // Green pixels
    SDL_Texture* texture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_RGBA8888, 
                                              SDL_TEXTUREACCESS_STATIC, 64, 64);
    SDL_UpdateTexture(texture, nullptr, pixels.data(), 64 * 4);
    
    for (auto _ : state) {
        SDL_RenderTexture(sdlRenderer, texture, nullptr, nullptr);
    }
    
    SDL_DestroyTexture(texture);
}

BENCHMARK_DEFINE_F(TextureFixture, TextureRender_SDLpp)(benchmark::State& state) {
    std::vector<uint32_t> pixels(64 * 64, 0xFF00FF00); // Green pixels
    sdl::Texture texture(*sdlppRenderer, 64, 64, pixels.data());
    
    for (auto _ : state) {
        sdlppRenderer->copy(texture);
    }
}

BENCHMARK_DEFINE_F(TextureFixture, TextureRenderWithRects_SDL3)(benchmark::State& state) {
    std::vector<uint32_t> pixels(64 * 64, 0xFF0000FF); // Blue pixels
    SDL_Texture* texture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_RGBA8888, 
                                              SDL_TEXTUREACCESS_STATIC, 64, 64);
    SDL_UpdateTexture(texture, nullptr, pixels.data(), 64 * 4);
    
    SDL_FRect srcRect = {0, 0, 32, 32};
    SDL_FRect dstRect = {100, 100, 64, 64};
    
    for (auto _ : state) {
        SDL_RenderTexture(sdlRenderer, texture, &srcRect, &dstRect);
    }
    
    SDL_DestroyTexture(texture);
}

BENCHMARK_DEFINE_F(TextureFixture, TextureRenderWithRects_SDLpp)(benchmark::State& state) {
    std::vector<uint32_t> pixels(64 * 64, 0xFF0000FF); // Blue pixels
    sdl::Texture texture(*sdlppRenderer, 64, 64, pixels.data());
    
    sdl::Rectangle<float> srcRect(0, 0, 32, 32);
    sdl::Rectangle<float> dstRect(100, 100, 64, 64);
    
    for (auto _ : state) {
        sdlppRenderer->copy(texture, srcRect, dstRect);
    }
}

BENCHMARK_DEFINE_F(TextureFixture, TextureSetBlendMode_SDL3)(benchmark::State& state) {
    std::vector<uint32_t> pixels(32 * 32, 0xFF808080);
    SDL_Texture* texture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_RGBA8888, 
                                              SDL_TEXTUREACCESS_STATIC, 32, 32);
    SDL_UpdateTexture(texture, nullptr, pixels.data(), 32 * 4);
    
    for (auto _ : state) {
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    }
    
    SDL_DestroyTexture(texture);
}

BENCHMARK_DEFINE_F(TextureFixture, TextureSetBlendMode_SDLpp)(benchmark::State& state) {
    std::vector<uint32_t> pixels(32 * 32, 0xFF808080);
    sdl::Texture texture(*sdlppRenderer, 32, 32, pixels.data());
    
    for (auto _ : state) {
        texture.setTextureBlendMode(sdl::Texture::kBlend);
    }
}

BENCHMARK_DEFINE_F(TextureFixture, TextureGetSize_SDL3)(benchmark::State& state) {
    std::vector<uint32_t> pixels(64 * 64, 0xFF888888);
    SDL_Texture* texture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_RGBA8888, 
                                              SDL_TEXTUREACCESS_STATIC, 64, 64);
    SDL_UpdateTexture(texture, nullptr, pixels.data(), 64 * 4);
    
    float w, h;
    for (auto _ : state) {
        SDL_GetTextureSize(texture, &w, &h);
        benchmark::DoNotOptimize(w);
        benchmark::DoNotOptimize(h);
    }
    
    SDL_DestroyTexture(texture);
}

BENCHMARK_DEFINE_F(TextureFixture, TextureGetSize_SDLpp)(benchmark::State& state) {
    std::vector<uint32_t> pixels(64 * 64, 0xFF888888);
    sdl::Texture texture(*sdlppRenderer, 64, 64, pixels.data());
    
    for (auto _ : state) {
        auto size = texture.getSize();
        benchmark::DoNotOptimize(size);
    }
}

// Register benchmarks
BENCHMARK_REGISTER_F(TextureFixture, TextureCreateFromPixels_SDL3);
BENCHMARK_REGISTER_F(TextureFixture, TextureCreateFromPixels_SDLpp);
BENCHMARK_REGISTER_F(TextureFixture, TextureRender_SDL3);
BENCHMARK_REGISTER_F(TextureFixture, TextureRender_SDLpp);
BENCHMARK_REGISTER_F(TextureFixture, TextureRenderWithRects_SDL3);
BENCHMARK_REGISTER_F(TextureFixture, TextureRenderWithRects_SDLpp);
BENCHMARK_REGISTER_F(TextureFixture, TextureSetBlendMode_SDL3);
BENCHMARK_REGISTER_F(TextureFixture, TextureSetBlendMode_SDLpp);
BENCHMARK_REGISTER_F(TextureFixture, TextureGetSize_SDL3);
BENCHMARK_REGISTER_F(TextureFixture, TextureGetSize_SDLpp);