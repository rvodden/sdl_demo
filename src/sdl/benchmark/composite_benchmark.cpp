#include <benchmark/benchmark.h>
#include <vector>
#include <SDL3/SDL.h>
#include "sdl/window.h"
#include "sdl/renderer.h"
#include "sdl/texture.h"
#include "sdl/color.h"

// =============================================================================
// COMPOSITE OPERATION BENCHMARKS - SDL++ vs Pure SDL3
// Tests realistic usage patterns combining multiple operations
// =============================================================================

class CompositeFixture : public benchmark::Fixture {
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

BENCHMARK_DEFINE_F(CompositeFixture, FullRenderLoop_SDL3)(benchmark::State& state) {
    std::vector<uint32_t> pixels(64 * 64, 0xFF00FF00);
    SDL_Texture* texture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_RGBA8888, 
                                              SDL_TEXTUREACCESS_STATIC, 64, 64);
    SDL_UpdateTexture(texture, nullptr, pixels.data(), 64 * 4);
    
    for (auto _ : state) {
        SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
        SDL_RenderClear(sdlRenderer);
        SDL_RenderTexture(sdlRenderer, texture, nullptr, nullptr);
        SDL_RenderPresent(sdlRenderer);
    }
    
    SDL_DestroyTexture(texture);
}

BENCHMARK_DEFINE_F(CompositeFixture, FullRenderLoop_SDLpp)(benchmark::State& state) {
    std::vector<uint32_t> pixels(64 * 64, 0xFF00FF00);
    sdl::Texture texture(*sdlppRenderer, 64, 64, pixels.data());
    sdl::Color black(0, 0, 0, 255);
    
    for (auto _ : state) {
        sdlppRenderer->setDrawColour(black);
        sdlppRenderer->clear();
        sdlppRenderer->copy(texture);
        sdlppRenderer->present();
    }
}

BENCHMARK_DEFINE_F(CompositeFixture, MultipleTextures_SDL3)(benchmark::State& state) {
    const int numTextures = 10;
    std::vector<SDL_Texture*> textures;
    std::vector<uint32_t> pixels(32 * 32, 0xFF8080FF);
    
    // Create textures
    for (int i = 0; i < numTextures; ++i) {
        SDL_Texture* texture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_RGBA8888, 
                                                  SDL_TEXTUREACCESS_STATIC, 32, 32);
        SDL_UpdateTexture(texture, nullptr, pixels.data(), 32 * 4);
        textures.push_back(texture);
    }
    
    for (auto _ : state) {
        SDL_SetRenderDrawColor(sdlRenderer, 64, 64, 64, 255);
        SDL_RenderClear(sdlRenderer);
        
        for (int i = 0; i < numTextures; ++i) {
            SDL_FRect dstRect = {static_cast<float>(i * 40), static_cast<float>(i * 20), 32, 32};
            SDL_RenderTexture(sdlRenderer, textures[i], nullptr, &dstRect);
        }
        
        SDL_RenderPresent(sdlRenderer);
    }
    
    // Cleanup textures
    for (auto* texture : textures) {
        SDL_DestroyTexture(texture);
    }
}

BENCHMARK_DEFINE_F(CompositeFixture, MultipleTextures_SDLpp)(benchmark::State& state) {
    const int numTextures = 10;
    std::vector<std::unique_ptr<sdl::Texture>> textures;
    std::vector<uint32_t> pixels(32 * 32, 0xFF8080FF);
    
    // Create textures
    for (int i = 0; i < numTextures; ++i) {
        textures.push_back(std::make_unique<sdl::Texture>(*sdlppRenderer, 32, 32, pixels.data()));
    }
    
    sdl::Color gray(64, 64, 64, 255);
    
    for (auto _ : state) {
        sdlppRenderer->setDrawColour(gray);
        sdlppRenderer->clear();
        
        for (int i = 0; i < numTextures; ++i) {
            sdl::Rectangle<float> dstRect(static_cast<float>(i * 40), static_cast<float>(i * 20), 32, 32);
            sdlppRenderer->copy(*textures[i], dstRect);
        }
        
        sdlppRenderer->present();
    }
}

BENCHMARK_DEFINE_F(CompositeFixture, WindowResizing_SDL3)(benchmark::State& state) {
    for (auto _ : state) {
        SDL_SetWindowSize(sdlWindow, 640, 480);
        int w, h;
        SDL_GetWindowSize(sdlWindow, &w, &h);
        benchmark::DoNotOptimize(w);
        benchmark::DoNotOptimize(h);
        
        SDL_SetWindowSize(sdlWindow, 800, 600);
        SDL_GetWindowSize(sdlWindow, &w, &h);
        benchmark::DoNotOptimize(w);
        benchmark::DoNotOptimize(h);
    }
}

// Note: SDL++ doesn't currently have a setSize method, so we'll test what's available
BENCHMARK_DEFINE_F(CompositeFixture, WindowOperations_SDLpp)(benchmark::State& state) {
    for (auto _ : state) {
        sdlppWindow->setTitle("Benchmark 1");
        auto title1 = sdlppWindow->getTitle();
        benchmark::DoNotOptimize(title1);
        
        sdlppWindow->setTitle("Benchmark 2");  
        auto title2 = sdlppWindow->getTitle();
        benchmark::DoNotOptimize(title2);
        
        auto size = sdlppWindow->getSize();
        benchmark::DoNotOptimize(size);
    }
}

BENCHMARK_DEFINE_F(CompositeFixture, TextureBlendModes_SDL3)(benchmark::State& state) {
    std::vector<uint32_t> pixels(64 * 64, 0x80FF8080); // Semi-transparent green
    SDL_Texture* texture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_RGBA8888, 
                                              SDL_TEXTUREACCESS_STATIC, 64, 64);
    SDL_UpdateTexture(texture, nullptr, pixels.data(), 64 * 4);
    
    SDL_BlendMode modes[] = {SDL_BLENDMODE_NONE, SDL_BLENDMODE_BLEND, SDL_BLENDMODE_ADD, SDL_BLENDMODE_MOD};
    
    for (auto _ : state) {
        SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
        SDL_RenderClear(sdlRenderer);
        
        for (auto mode : modes) {
            SDL_SetTextureBlendMode(texture, mode);
            SDL_RenderTexture(sdlRenderer, texture, nullptr, nullptr);
        }
        
        SDL_RenderPresent(sdlRenderer);
    }
    
    SDL_DestroyTexture(texture);
}

BENCHMARK_DEFINE_F(CompositeFixture, TextureBlendModes_SDLpp)(benchmark::State& state) {
    std::vector<uint32_t> pixels(64 * 64, 0x80FF8080); // Semi-transparent green
    sdl::Texture texture(*sdlppRenderer, 64, 64, pixels.data());
    
    sdl::Texture::BlendMode modes[] = {sdl::Texture::kNone, sdl::Texture::kBlend, sdl::Texture::kAdd, sdl::Texture::kMod};
    
    sdl::Color black(0, 0, 0, 255);
    
    for (auto _ : state) {
        sdlppRenderer->setDrawColour(black);
        sdlppRenderer->clear();
        
        for (auto mode : modes) {
            texture.setTextureBlendMode(mode);
            sdlppRenderer->copy(texture);
        }
        
        sdlppRenderer->present();
    }
}

// Register benchmarks
BENCHMARK_REGISTER_F(CompositeFixture, FullRenderLoop_SDL3);
BENCHMARK_REGISTER_F(CompositeFixture, FullRenderLoop_SDLpp);
BENCHMARK_REGISTER_F(CompositeFixture, MultipleTextures_SDL3);
BENCHMARK_REGISTER_F(CompositeFixture, MultipleTextures_SDLpp);
BENCHMARK_REGISTER_F(CompositeFixture, WindowResizing_SDL3);
BENCHMARK_REGISTER_F(CompositeFixture, WindowOperations_SDLpp);
BENCHMARK_REGISTER_F(CompositeFixture, TextureBlendModes_SDL3);
BENCHMARK_REGISTER_F(CompositeFixture, TextureBlendModes_SDLpp);