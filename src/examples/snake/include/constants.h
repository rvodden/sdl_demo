#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cstdint>

#include <sdl/color.h>

namespace snake {

// Gameplay constants
const uint16_t kGridWidth  = 32U;
const uint16_t kGridHeight = 24U;

enum class Direction : uint8_t {
  kUp,
  kDown,
  kLeft,
  kRight
};

const uint16_t kStartingX = kGridWidth / 2;
const uint16_t kStartingY = kGridHeight / 2;
const Direction kStartingDirection = Direction::kRight;

// UI Constants

const uint32_t kInitialWindowWidth  = 1024U;
const uint32_t kInitialWindowHeight =  768U;

const sdl::Color kWallColor  = sdl::NamedColor::kWhite;
const sdl::Color kSnakeColor = sdl::NamedColor::kRed;
const sdl::Color kFoodColor  = sdl::NamedColor::kGreen;

const float kCellBorderThickness = 2.0F;

}

#endif // CONSTANTS_H
