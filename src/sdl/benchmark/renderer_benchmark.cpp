#include <benchmark/benchmark.h>
#include <SDL3/SDL.h>
#include "sdl/window.h"
#include "sdl/renderer.h"
#include "sdl/color.h"

// =============================================================================
// RENDERER BENCHMARKS - SDL++ vs Pure SDL3
// =============================================================================

class RendererFixture : public benchmark::Fixture {
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

BENCHMARK_DEFINE_F(RendererFixture, RendererCreation_SDL3)(benchmark::State& state) {
    for (auto _ : state) {
        SDL_Window* tempWindow = SDL_CreateWindow("Temp", 400, 300, 0);
        SDL_Renderer* renderer = SDL_CreateRenderer(tempWindow, nullptr);
        benchmark::DoNotOptimize(renderer);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(tempWindow);
    }
}

BENCHMARK_DEFINE_F(RendererFixture, RendererCreation_SDLpp)(benchmark::State& state) {
    for (auto _ : state) {
        sdl::Window tempWindow("Temp", 400, 300, 0);
        sdl::Renderer renderer(tempWindow);
        benchmark::DoNotOptimize(renderer);
    }
}

BENCHMARK_DEFINE_F(RendererFixture, RendererSetDrawColor_SDL3)(benchmark::State& state) {
    for (auto _ : state) {
        SDL_SetRenderDrawColor(sdlRenderer, 255, 128, 64, 255);
    }
}

BENCHMARK_DEFINE_F(RendererFixture, RendererSetDrawColor_SDLpp)(benchmark::State& state) {
    sdl::Color color(255, 128, 64, 255);
    for (auto _ : state) {
        sdlppRenderer->setDrawColour(color);
    }
}

BENCHMARK_DEFINE_F(RendererFixture, RendererClear_SDL3)(benchmark::State& state) {
    for (auto _ : state) {
        SDL_RenderClear(sdlRenderer);
    }
}

BENCHMARK_DEFINE_F(RendererFixture, RendererClear_SDLpp)(benchmark::State& state) {
    for (auto _ : state) {
        sdlppRenderer->clear();
    }
}

BENCHMARK_DEFINE_F(RendererFixture, RendererPresent_SDL3)(benchmark::State& state) {
    for (auto _ : state) {
        SDL_RenderPresent(sdlRenderer);
    }
}

BENCHMARK_DEFINE_F(RendererFixture, RendererPresent_SDLpp)(benchmark::State& state) {
    for (auto _ : state) {
        sdlppRenderer->present();
    }
}

BENCHMARK_DEFINE_F(RendererFixture, RendererGetOutputSize_SDL3)(benchmark::State& state) {
    int width, height;
    for (auto _ : state) {
        SDL_GetRenderOutputSize(sdlRenderer, &width, &height);
        benchmark::DoNotOptimize(width);
        benchmark::DoNotOptimize(height);
    }
}

BENCHMARK_DEFINE_F(RendererFixture, RendererGetOutputSize_SDLpp)(benchmark::State& state) {
    for (auto _ : state) {
        auto size = sdlppRenderer->getOutputSize();
        benchmark::DoNotOptimize(size);
    }
}

BENCHMARK_DEFINE_F(RendererFixture, RendererSetScale_SDL3)(benchmark::State& state) {
    for (auto _ : state) {
        SDL_SetRenderScale(sdlRenderer, 2.0f, 2.0f);
    }
}

BENCHMARK_DEFINE_F(RendererFixture, RendererSetScale_SDLpp)(benchmark::State& state) {
    for (auto _ : state) {
        sdlppRenderer->setScale(2.0f, 2.0f);
    }
}

BENCHMARK_DEFINE_F(RendererFixture, RendererDrawLine_SDL3)(benchmark::State& state) {
    for (auto _ : state) {
        SDL_RenderLine(sdlRenderer, 0, 0, 100, 100);
    }
}

BENCHMARK_DEFINE_F(RendererFixture, RendererDrawLine_SDLpp)(benchmark::State& state) {
    for (auto _ : state) {
        sdlppRenderer->drawLine(0, 0, 100, 100);
    }
}

// Register benchmarks
BENCHMARK_REGISTER_F(RendererFixture, RendererCreation_SDL3);
BENCHMARK_REGISTER_F(RendererFixture, RendererCreation_SDLpp);
BENCHMARK_REGISTER_F(RendererFixture, RendererSetDrawColor_SDL3);
BENCHMARK_REGISTER_F(RendererFixture, RendererSetDrawColor_SDLpp);
BENCHMARK_REGISTER_F(RendererFixture, RendererClear_SDL3);
BENCHMARK_REGISTER_F(RendererFixture, RendererClear_SDLpp);
BENCHMARK_REGISTER_F(RendererFixture, RendererPresent_SDL3);
BENCHMARK_REGISTER_F(RendererFixture, RendererPresent_SDLpp);
BENCHMARK_REGISTER_F(RendererFixture, RendererGetOutputSize_SDL3);
BENCHMARK_REGISTER_F(RendererFixture, RendererGetOutputSize_SDLpp);
BENCHMARK_REGISTER_F(RendererFixture, RendererSetScale_SDL3);
BENCHMARK_REGISTER_F(RendererFixture, RendererSetScale_SDLpp);
BENCHMARK_REGISTER_F(RendererFixture, RendererDrawLine_SDL3);
BENCHMARK_REGISTER_F(RendererFixture, RendererDrawLine_SDLpp);