#ifndef SDL_RANDOM_H
#define SDL_RANDOM_H

#include <cstdint>
#include <concepts>
#include <memory>

#include "sdl_export.h"

namespace sdl {

/**
 * @brief Concept to restrict Random template to supported types.
 * 
 * Only int32_t and float are supported as they map directly to SDL3's
 * random functions - SDL_rand_r for integers and computed ranges for floats.
 */
template<typename T>
concept RandomType = std::same_as<T, int32_t> || std::same_as<T, float>;

// Forward declaration - specializations in random_impl.h
template<RandomType T> class RandomImpl;

/**
 * @brief Template-based random number generator with RAII state management.
 * 
 * Provides thread-safe random number generation using SDL3's random functions
 * with automatic state management. Each Random instance maintains its own state
 * for thread safety.
 * 
 * @tparam T Value type - must be int32_t or float
 * 
 * @par Supported Types:
 * - Random<int32_t> - Uses SDL_rand_r for integer generation
 * - Random<float> - Uses SDL_rand_r with conversion for float generation
 * 
 * @par Usage Examples:
 * @code
 * // Integer random generator
 * Random<int32_t> intRand(12345);
 * int value = intRand.range(1, 100);  // [1, 100)
 * 
 * // Float random generator
 * Random<float> floatRand;
 * float value = floatRand.range(0.0f, 1.0f);  // [0.0, 1.0)
 * @endcode
 */
template<RandomType T>
class SDL_EXPORT Random {
public:
    /**
     * @brief Create random generator with specified seed
     * @param seed Seed value (0 uses SDL_GetPerformanceCounter)
     */
    explicit Random(uint64_t seed = 0);
    
    /**
     * @brief Copy constructor is deleted (state should be unique)
     */
    Random(const Random& other) = delete;
    
    /**
     * @brief Move constructor is deleted (state should be unique)
     */
    Random(Random&& other) noexcept = delete;
    
    /**
     * @brief Destructor
     */
    ~Random();
    
    /**
     * @brief Copy assignment is deleted
     */
    auto operator=(const Random& other) -> Random& = delete;
    
    /**
     * @brief Move assignment is deleted
     */
    auto operator=(Random&& other) noexcept -> Random& = delete;
    
    /**
     * @brief Generate random value in range [min, max)
     * @param min Minimum value (inclusive)
     * @param max Maximum value (exclusive)
     * @return Random value in specified range
     */
    [[nodiscard]] auto range(T min, T max) -> T;

private:
    std::unique_ptr<RandomImpl<T>> _impl;
};

// Common type aliases for convenience
using IntRandom = Random<int32_t>;
using FloatRandom = Random<float>;

// Global convenience functions wrapping SDL's global random state
namespace random {
    /**
     * @brief Wrapper around SDL_srand
     * @param seed Seed value (0 uses SDL_GetPerformanceCounter)
     */
    void srand(uint64_t seed = 0);
    
    /**
     * @brief Wrapper around SDL_rand
     * @param n Upper bound (exclusive), must be positive
     * @return Random value in [0, n)
     */
    [[nodiscard]] auto rand(int32_t n) -> int32_t;
    
    /**
     * @brief Wrapper around SDL_randf
     * @return Random float in [0.0, 1.0)
     */
    [[nodiscard]] auto randf() -> float;
    
    /**
     * @brief Wrapper around SDL_rand_bits
     * @return Random 32-bit value
     */
    [[nodiscard]] auto randBits() -> uint32_t;
}

}  // namespace sdl

#endif  // SDL_RANDOM_H