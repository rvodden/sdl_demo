#include <benchmark/benchmark.h>
#include <stdexcept>
#include <SDL3/SDL.h>
#include "sdl/window.h"

// =============================================================================
// WINDOW BENCHMARKS - SDL++ vs Pure SDL3
// =============================================================================

class WindowFixture : public benchmark::Fixture {
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

BENCHMARK_DEFINE_F(WindowFixture, WindowCreation_SDL3)(benchmark::State& state) {
    for (auto _ : state) {
        SDL_Window* window = SDL_CreateWindow("Test", 800, 600, 0);
        benchmark::DoNotOptimize(window);
        SDL_DestroyWindow(window);
    }
}

BENCHMARK_DEFINE_F(WindowFixture, WindowCreation_SDLpp)(benchmark::State& state) {
    for (auto _ : state) {
        sdl::Window window("Test", 800, 600, 0);
        benchmark::DoNotOptimize(window);
    }
}

BENCHMARK_DEFINE_F(WindowFixture, WindowGetSize_SDL3)(benchmark::State& state) {
    SDL_Window* window = SDL_CreateWindow("Test", 800, 600, 0);
    int w, h;
    
    for (auto _ : state) {
        SDL_GetWindowSize(window, &w, &h);
        benchmark::DoNotOptimize(w);
        benchmark::DoNotOptimize(h);
    }
    
    SDL_DestroyWindow(window);
}

BENCHMARK_DEFINE_F(WindowFixture, WindowGetSize_SDLpp)(benchmark::State& state) {
    sdl::Window window("Test", 800, 600, 0);
    
    for (auto _ : state) {
        auto size = window.getSize();
        benchmark::DoNotOptimize(size);
    }
}

BENCHMARK_DEFINE_F(WindowFixture, WindowSetTitle_SDL3)(benchmark::State& state) {
    SDL_Window* window = SDL_CreateWindow("Test", 800, 600, 0);
    const char* title = "Benchmark Title";
    
    for (auto _ : state) {
        SDL_SetWindowTitle(window, title);
    }
    
    SDL_DestroyWindow(window);
}

BENCHMARK_DEFINE_F(WindowFixture, WindowSetTitle_SDLpp)(benchmark::State& state) {
    sdl::Window window("Test", 800, 600, 0);
    std::string title = "Benchmark Title";
    
    for (auto _ : state) {
        window.setTitle(title);
    }
}

BENCHMARK_DEFINE_F(WindowFixture, WindowGetTitle_SDL3)(benchmark::State& state) {
    SDL_Window* window = SDL_CreateWindow("Test", 800, 600, 0);
    
    for (auto _ : state) {
        const char* title = SDL_GetWindowTitle(window);
        benchmark::DoNotOptimize(title);
    }
    
    SDL_DestroyWindow(window);
}

BENCHMARK_DEFINE_F(WindowFixture, WindowGetTitle_SDLpp)(benchmark::State& state) {
    sdl::Window window("Test", 800, 600, 0);
    
    for (auto _ : state) {
        auto title = window.getTitle();
        benchmark::DoNotOptimize(title);
    }
}

// Register benchmarks
BENCHMARK_REGISTER_F(WindowFixture, WindowCreation_SDL3);
BENCHMARK_REGISTER_F(WindowFixture, WindowCreation_SDLpp);
BENCHMARK_REGISTER_F(WindowFixture, WindowGetSize_SDL3);
BENCHMARK_REGISTER_F(WindowFixture, WindowGetSize_SDLpp);
BENCHMARK_REGISTER_F(WindowFixture, WindowSetTitle_SDL3);
BENCHMARK_REGISTER_F(WindowFixture, WindowSetTitle_SDLpp);
BENCHMARK_REGISTER_F(WindowFixture, WindowGetTitle_SDL3);
BENCHMARK_REGISTER_F(WindowFixture, WindowGetTitle_SDLpp);