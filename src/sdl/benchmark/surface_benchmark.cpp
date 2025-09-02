#include <benchmark/benchmark.h>
#include <vector>
#include <SDL3/SDL.h>
#include "sdl/surface.h"
#include "sdl/color.h"

// =============================================================================
// SURFACE BENCHMARKS - SDL++ vs Pure SDL3
// =============================================================================

class SurfaceFixture : public benchmark::Fixture {
public:
    void SetUp(const ::benchmark::State& state) override {
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            throw std::runtime_error("SDL_Init failed");
        }
    }
    
    void TearDown(const ::benchmark::State& state) override {
        SDL_Quit();
    }
};

BENCHMARK_DEFINE_F(SurfaceFixture, SurfaceCreate_SDL3)(benchmark::State& state) {
    for (auto _ : state) {
        SDL_Surface* surface = SDL_CreateSurface(256, 256, SDL_PIXELFORMAT_RGBA8888);
        benchmark::DoNotOptimize(surface);
        SDL_DestroySurface(surface);
    }
}

BENCHMARK_DEFINE_F(SurfaceFixture, SurfaceCreate_SDLpp)(benchmark::State& state) {
    for (auto _ : state) {
        sdl::Surface surface(256, 256);
        benchmark::DoNotOptimize(surface);
    }
}

BENCHMARK_DEFINE_F(SurfaceFixture, SurfacePixelAccess_SDL3)(benchmark::State& state) {
    SDL_Surface* surface = SDL_CreateSurface(256, 256, SDL_PIXELFORMAT_RGBA8888);
    
    for (auto _ : state) {
        SDL_LockSurface(surface);
        uint32_t* pixels = static_cast<uint32_t*>(surface->pixels);
        for (int i = 0; i < 1000; ++i) {
            pixels[i] = 0xFFFF0000;
        }
        SDL_UnlockSurface(surface);
    }
    
    SDL_DestroySurface(surface);
}

BENCHMARK_DEFINE_F(SurfaceFixture, SurfacePixelAccess_SDLpp)(benchmark::State& state) {
    sdl::Surface surface(256, 256);
    sdl::Color red(255, 0, 0, 255);
    
    for (auto _ : state) {
        for (int i = 0; i < 1000; ++i) {
            int x = i % 256;
            int y = i / 256;
            if (y < 256) { // Bounds check
                surface.setPixel(x, y, red);
            }
        }
    }
}

BENCHMARK_DEFINE_F(SurfaceFixture, SurfaceFill_SDL3)(benchmark::State& state) {
    SDL_Surface* surface = SDL_CreateSurface(256, 256, SDL_PIXELFORMAT_RGBA8888);
    
    for (auto _ : state) {
        SDL_FillSurfaceRect(surface, nullptr, SDL_MapSurfaceRGBA(surface, 255, 0, 0, 255));
    }
    
    SDL_DestroySurface(surface);
}

BENCHMARK_DEFINE_F(SurfaceFixture, SurfaceFill_SDLpp)(benchmark::State& state) {
    sdl::Surface surface(256, 256);
    sdl::Color red(255, 0, 0, 255);
    
    for (auto _ : state) {
        surface.fill(red);
    }
}

BENCHMARK_DEFINE_F(SurfaceFixture, SurfaceFillRect_SDL3)(benchmark::State& state) {
    SDL_Surface* surface = SDL_CreateSurface(256, 256, SDL_PIXELFORMAT_RGBA8888);
    SDL_Rect rect = {50, 50, 100, 100};
    
    for (auto _ : state) {
        SDL_FillSurfaceRect(surface, &rect, SDL_MapSurfaceRGBA(surface, 0, 255, 0, 255));
    }
    
    SDL_DestroySurface(surface);
}

BENCHMARK_DEFINE_F(SurfaceFixture, SurfaceFillRect_SDLpp)(benchmark::State& state) {
    sdl::Surface surface(256, 256);
    sdl::Color green(0, 255, 0, 255);
    
    for (auto _ : state) {
        surface.fillRect(50, 50, 100, 100, green);
    }
}

BENCHMARK_DEFINE_F(SurfaceFixture, SurfaceBlit_SDL3)(benchmark::State& state) {
    SDL_Surface* src = SDL_CreateSurface(128, 128, SDL_PIXELFORMAT_RGBA8888);
    SDL_Surface* dst = SDL_CreateSurface(256, 256, SDL_PIXELFORMAT_RGBA8888);
    
    SDL_FillSurfaceRect(src, nullptr, SDL_MapSurfaceRGBA(src, 255, 255, 0, 255));
    
    for (auto _ : state) {
        SDL_BlitSurface(src, nullptr, dst, nullptr);
    }
    
    SDL_DestroySurface(src);
    SDL_DestroySurface(dst);
}

BENCHMARK_DEFINE_F(SurfaceFixture, SurfaceBlit_SDLpp)(benchmark::State& state) {
    sdl::Surface src(128, 128);
    sdl::Surface dst(256, 256);
    sdl::Color yellow(255, 255, 0, 255);
    
    src.fill(yellow);
    
    for (auto _ : state) {
        dst.blit(src);
    }
}

BENCHMARK_DEFINE_F(SurfaceFixture, SurfaceGetPixels_SDL3)(benchmark::State& state) {
    SDL_Surface* surface = SDL_CreateSurface(256, 256, SDL_PIXELFORMAT_RGBA8888);
    
    for (auto _ : state) {
        SDL_LockSurface(surface);
        void* pixels = surface->pixels;
        benchmark::DoNotOptimize(pixels);
        SDL_UnlockSurface(surface);
    }
    
    SDL_DestroySurface(surface);
}

BENCHMARK_DEFINE_F(SurfaceFixture, SurfaceGetPixels_SDLpp)(benchmark::State& state) {
    sdl::Surface surface(256, 256);
    
    for (auto _ : state) {
        auto pixels = surface.getPixels();
        benchmark::DoNotOptimize(pixels);
    }
}

// Register benchmarks
BENCHMARK_REGISTER_F(SurfaceFixture, SurfaceCreate_SDL3);
BENCHMARK_REGISTER_F(SurfaceFixture, SurfaceCreate_SDLpp);
BENCHMARK_REGISTER_F(SurfaceFixture, SurfacePixelAccess_SDL3);
BENCHMARK_REGISTER_F(SurfaceFixture, SurfacePixelAccess_SDLpp);
BENCHMARK_REGISTER_F(SurfaceFixture, SurfaceFill_SDL3);
BENCHMARK_REGISTER_F(SurfaceFixture, SurfaceFill_SDLpp);
BENCHMARK_REGISTER_F(SurfaceFixture, SurfaceFillRect_SDL3);
BENCHMARK_REGISTER_F(SurfaceFixture, SurfaceFillRect_SDLpp);
BENCHMARK_REGISTER_F(SurfaceFixture, SurfaceBlit_SDL3);
BENCHMARK_REGISTER_F(SurfaceFixture, SurfaceBlit_SDLpp);
BENCHMARK_REGISTER_F(SurfaceFixture, SurfaceGetPixels_SDL3);
BENCHMARK_REGISTER_F(SurfaceFixture, SurfaceGetPixels_SDLpp);