#ifndef SDL_RANDOM_IMPL_H
#define SDL_RANDOM_IMPL_H

#include <cstdint>
#include <SDL3/SDL.h>

#include "sdl/random.h"

namespace sdl {

/**
 * @brief Implementation base template for Random.
 * 
 * This template is specialized for supported types to provide
 * direct mapping to SDL3's random functions with zero overhead.
 */
template<RandomType T>
class RandomImpl {
};

/**
 * @brief Specialization for int32_t using SDL_rand_r.
 * 
 * This specialization uses SDL3's SDL_rand_r function for thread-safe
 * integer random number generation with per-instance state.
 */
template<>
class RandomImpl<int32_t> {
    friend Random<int32_t>;

public:
    /**
     * @brief Constructs with specified seed
     * @param seed Seed value (0 uses SDL_GetPerformanceCounter)
     */
    explicit RandomImpl(uint64_t seed);
    
    /**
     * @brief Copy constructor
     */
    RandomImpl(const RandomImpl& other) = default;
    
    /**
     * @brief Move constructor
     */
    RandomImpl(RandomImpl&& other) noexcept = default;
    
    /**
     * @brief Copy assignment
     */
    auto operator=(const RandomImpl& other) -> RandomImpl& = default;
    
    /**
     * @brief Move assignment
     */
    auto operator=(RandomImpl&& other) noexcept -> RandomImpl& = default;
    
    /**
     * @brief Generate random int32_t in range [min, max)
     * @param min Minimum value (inclusive)
     * @param max Maximum value (exclusive)
     * @return Random value in specified range
     */
    [[nodiscard]] auto range(int32_t min, int32_t max) -> int32_t;

private:
    uint64_t _state;
};

/**
 * @brief Specialization for float using SDL_rand_r with conversion.
 * 
 * This specialization uses SDL3's SDL_rand_r function and converts
 * the result to floating-point values in the specified range.
 */
template<>
class RandomImpl<float> {
    friend Random<float>;

public:
    /**
     * @brief Constructs with specified seed
     * @param seed Seed value (0 uses SDL_GetPerformanceCounter)
     */
    explicit RandomImpl(uint64_t seed);
    
    /**
     * @brief Copy constructor
     */
    RandomImpl(const RandomImpl& other) = default;
    
    /**
     * @brief Move constructor
     */
    RandomImpl(RandomImpl&& other) noexcept = default;
    
    /**
     * @brief Copy assignment
     */
    auto operator=(const RandomImpl& other) -> RandomImpl& = default;
    
    /**
     * @brief Move assignment
     */
    auto operator=(RandomImpl&& other) noexcept -> RandomImpl& = default;
    
    /**
     * @brief Generate random float in range [min, max)
     * @param min Minimum value (inclusive)
     * @param max Maximum value (exclusive)
     * @return Random value in specified range
     */
    [[nodiscard]] auto range(float min, float max) -> float;

private:
    uint64_t _state;
};

}  // namespace sdl

#endif  // SDL_RANDOM_IMPL_H