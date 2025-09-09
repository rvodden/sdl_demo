#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cstdint>

#include <sdl/color.h>

namespace snake {

// Gameplay constants
const uint16_t kGridWidth  = 32U;
const uint16_t kGridHeight = 24U;

const uint16_t kStartingX = kGridWidth / 2;
const uint16_t kStartingY = kGridHeight / 2;

// UI Constants

const uint32_t kInitialWindowWidth  = 1024U;
const uint32_t kInitialWindowHeight =  768U;

const sdl::Color kWallColor = sdl::NamedColor::kWhite;

const float kCellBorderThickness = 2.0F;

}

#endif // CONSTANTS_H
