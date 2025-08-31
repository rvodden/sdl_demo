#ifndef PONG_CONSTANTS_H
#define PONG_CONSTANTS_H

#include <cstdint>

#include "point.h"

namespace pong {

constexpr uint32_t kMaxScore = 20;

// Window Constants (minimum viable window size)
constexpr auto kWindowWidth = 1280;
constexpr auto kWindowHeight = 720;
constexpr auto kMinWindowWidth = 400;
constexpr auto kMinWindowHeight = 300;

// Ball Constants
constexpr auto kBallSize = Point<float>{15.0F, 15.0F};
constexpr auto kBallSpeed = Point<float>{-300.0F, 0.0F}; // pixels per second
constexpr auto kMaxBallSpeed = 800.0F;

// Paddle Constants
constexpr auto kPaddleSize = Point<float>{10.0F, 100.0F};
constexpr auto kPaddleSpeed = 400.0F; // pixels per second
constexpr auto kMaxPaddleSpeed = 800.0F;
constexpr auto kMinPaddleSpeed = 100.0F;
constexpr auto kPaddleFromWallDistance = 50.0F;

// Game Physics Constants
constexpr auto kBallDeflectionAngle = 150.0F; // pixels per second
constexpr auto kMaxDeflectionAngle = 400.0F;
constexpr auto kBallResetSpeed = 300.0F;

// Collision Detection Constants
constexpr auto kPaddleCollisionZoneDivisions = 3.0F; // Used to divide paddle into collision zones

// UI Constants
constexpr auto kScoreFontSize = 40;
constexpr auto kScorePositionLeftRatio = 0.25F;
constexpr auto kScorePositionRightRatio = 0.75F;
constexpr auto kScoreVerticalOffset = 20.0F;

} // namespace pong

#endif // PONG_CONSTANTS_H
