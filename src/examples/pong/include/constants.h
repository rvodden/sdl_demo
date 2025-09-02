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
constexpr auto kBallSpeed = Point<float>{-0.2F, 0.0F}; // pixels per millisecond
constexpr auto kMaxBallSpeed = 0.8F;

// Paddle Constants
constexpr auto kPaddleSize = Point<float>{10.0F, 100.0F};
constexpr auto kPaddleSpeed = 0.3F; // pixels per millisecond
constexpr auto kMaxPaddleSpeed = 0.6F;
constexpr auto kMinPaddleSpeed = 0.1F;
constexpr auto kPaddleFromWallDistance = 50.0F;

// Game Physics Constants
constexpr auto kBallDeflectionRatio = 0.75F;
constexpr auto kBallDeflectionAngle = kBallDeflectionRatio * kBallSpeed.x; // pixels per millisecond
constexpr auto kMaxDeflectionAngle = 4.0F;
constexpr auto kBallResetSpeed = kBallSpeed;
constexpr auto kMinimumDt = 1.0F;
constexpr auto kMaximumDt = 1000.0F;

// Collision Detection Constants
constexpr auto kPaddleCollisionZoneDivisions = 3.0F; // Used to divide paddle into collision zones

// UI Constants
constexpr auto kScoreFontSize = 40.0F;
constexpr auto kScorePositionLeftRatio = 0.25F;
constexpr auto kScorePositionRightRatio = 0.75F;
constexpr auto kScoreVerticalOffset = 20.0F;

} // namespace pong

#endif // PONG_CONSTANTS_H
