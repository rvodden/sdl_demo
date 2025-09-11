#include "sdl/random.h"

#include "random_impl.h"
#include <SDL3/SDL.h>

namespace sdl {

// Template explicit instantiations
template class Random<int32_t>;
template class Random<float>;

// Constructor
template<RandomType T>
Random<T>::Random(uint64_t seed) : _impl(std::make_unique<RandomImpl<T>>(seed)) {
}

// Destructor
template<RandomType T>
Random<T>::~Random() = default;

// Range method
template<RandomType T>
auto Random<T>::range(T min, T max) -> T {
    return _impl->range(min, max);
}

// RandomImpl<int32_t> implementation
RandomImpl<int32_t>::RandomImpl(uint64_t seed) {
    if (seed == 0) {
        _state = SDL_GetPerformanceCounter();
    } else {
        _state = seed;
    }
}

auto RandomImpl<int32_t>::range(int32_t min, int32_t max) -> int32_t {
    if (max <= min) {
        return min;
    }
    int32_t range_size = max - min;
    return min + SDL_rand_r(&_state, range_size);
}

// RandomImpl<float> implementation
RandomImpl<float>::RandomImpl(uint64_t seed) {
    if (seed == 0) {
        _state = SDL_GetPerformanceCounter();
    } else {
        _state = seed;
    }
}

auto RandomImpl<float>::range(float min, float max) -> float {
    if (max <= min) {
        return min;
    }
    // Use SDL_rand_bits to get full 32-bit precision
    uint32_t random_bits = SDL_rand_bits();
    // Convert to [0.0, 1.0) using full precision
    float normalized = static_cast<float>(random_bits) / static_cast<float>(UINT32_MAX + 1ULL);
    return min + normalized * (max - min);
}

// Global functions
namespace random {
    void srand(uint64_t seed) {
        SDL_srand(seed);
    }
    
    auto rand(int32_t n) -> int32_t {
        return SDL_rand(n);
    }
    
    auto randf() -> float {
        return SDL_randf();
    }
    
    auto randBits() -> uint32_t {
        return SDL_rand_bits();
    }
}

}  // namespace sdl