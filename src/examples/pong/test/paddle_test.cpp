#include <gtest/gtest.h>
#include <stdexcept>

#include <sdl/rectangle.h>

#include "paddle.h"
#include "constants.h"
#include "point.h"
#include "events.h"

using pong::Paddle;
using pong::Point;
using pong::PaddleCollisionEvent;

// Test constants
constexpr float kTestX = 100.0f;
constexpr float kTestY = 200.0f;
constexpr float kMinY = 0.0f;
constexpr float kMaxY = 400.0f;
constexpr float kDeltaTime = 0.016f; // ~60 FPS

class PaddleTest : public ::testing::Test {
protected:
    void SetUp() override {
        initialPosition = Point<float>{kTestX, kTestY};
    }
    
    Point<float> initialPosition;
};

// Constructor Tests
TEST_F(PaddleTest, ValidConstructor) {
    Paddle paddle(initialPosition, kMinY, kMaxY);
    
    auto extent = paddle.getExtent();
    
    // Paddle should be centered at initial position
    EXPECT_FLOAT_EQ(extent.getX(), kTestX - pong::kPaddleSize.x / 2.0f);
    EXPECT_FLOAT_EQ(extent.getY(), kTestY - pong::kPaddleSize.y / 2.0f);
    EXPECT_FLOAT_EQ(extent.getWidth(), pong::kPaddleSize.x);
    EXPECT_FLOAT_EQ(extent.getHeight(), pong::kPaddleSize.y);
}

TEST_F(PaddleTest, CustomSizeConstructor) {
    Point<float> customSize{20.0f, 150.0f};
    Paddle paddle(initialPosition, kMinY, kMaxY, customSize);
    
    auto extent = paddle.getExtent();
    EXPECT_FLOAT_EQ(extent.getWidth(), customSize.x);
    EXPECT_FLOAT_EQ(extent.getHeight(), customSize.y);
}

TEST_F(PaddleTest, ConstructorWithNegativePosition) {
    Point<float> negativePosition{-10.0f, kTestY};
    EXPECT_THROW(Paddle paddle(negativePosition, kMinY, kMaxY), std::invalid_argument);
    
    Point<float> negativeYPosition{kTestX, -10.0f};
    EXPECT_THROW(Paddle paddle(negativeYPosition, kMinY, kMaxY), std::invalid_argument);
}

TEST_F(PaddleTest, ConstructorWithInvalidBounds) {
    // Negative minY
    EXPECT_THROW(Paddle paddle(initialPosition, -10.0f, kMaxY), std::invalid_argument);
    
    // Negative maxY
    EXPECT_THROW(Paddle paddle(initialPosition, kMinY, -10.0f), std::invalid_argument);
    
    // maxY <= minY
    EXPECT_THROW(Paddle paddle(initialPosition, 100.0f, 100.0f), std::invalid_argument);
    EXPECT_THROW(Paddle paddle(initialPosition, 100.0f, 50.0f), std::invalid_argument);
}

TEST_F(PaddleTest, ConstructorWithInvalidSize) {
    Point<float> invalidSize{0.0f, pong::kPaddleSize.y};
    EXPECT_THROW(Paddle paddle(initialPosition, kMinY, kMaxY, invalidSize), std::invalid_argument);
    
    Point<float> negativeSize{-10.0f, pong::kPaddleSize.y};
    EXPECT_THROW(Paddle paddle(initialPosition, kMinY, kMaxY, negativeSize), std::invalid_argument);
}

// Velocity Tests
TEST_F(PaddleTest, InitialVelocityIsStopped) {
    Paddle paddle(initialPosition, kMinY, kMaxY);
    
    // Update should not change position when stopped
    auto initialExtent = paddle.getExtent();
    paddle.update(kDeltaTime);
    auto finalExtent = paddle.getExtent();
    
    EXPECT_FLOAT_EQ(initialExtent.getX(), finalExtent.getX());
    EXPECT_FLOAT_EQ(initialExtent.getY(), finalExtent.getY());
}

TEST_F(PaddleTest, SetVelocityUp) {
    Paddle paddle(initialPosition, kMinY, kMaxY);
    paddle.setVelocity(Paddle::Velocity::kUp);
    
    auto initialExtent = paddle.getExtent();
    paddle.update(kDeltaTime);
    auto finalExtent = paddle.getExtent();
    
    float expectedYChange = pong::kPaddleSpeed * kDeltaTime;
    EXPECT_FLOAT_EQ(finalExtent.getY(), initialExtent.getY() - expectedYChange);
    EXPECT_FLOAT_EQ(finalExtent.getX(), initialExtent.getX()); // X should not change
}

TEST_F(PaddleTest, SetVelocityDown) {
    Paddle paddle(initialPosition, kMinY, kMaxY);
    paddle.setVelocity(Paddle::Velocity::kDown);
    
    auto initialExtent = paddle.getExtent();
    paddle.update(kDeltaTime);
    auto finalExtent = paddle.getExtent();
    
    float expectedYChange = pong::kPaddleSpeed * kDeltaTime;
    EXPECT_FLOAT_EQ(finalExtent.getY(), initialExtent.getY() + expectedYChange);
    EXPECT_FLOAT_EQ(finalExtent.getX(), initialExtent.getX()); // X should not change
}

// Boundary Tests
TEST_F(PaddleTest, MovementBoundaryTopLimit) {
    // Start near the top
    Point<float> nearTopPosition{kTestX, kMinY + 10.0f};
    Paddle paddle(nearTopPosition, kMinY, kMaxY);
    
    paddle.setVelocity(Paddle::Velocity::kUp);
    
    // Update multiple times to ensure it would go past boundary
    for (int i = 0; i < 100; ++i) {
        paddle.update(kDeltaTime);
    }
    
    auto finalExtent = paddle.getExtent();
    EXPECT_FLOAT_EQ(finalExtent.getY(), kMinY);
}

TEST_F(PaddleTest, MovementBoundaryBottomLimit) {
    // Start near the bottom
    Point<float> nearBottomPosition{kTestX, kMaxY - pong::kPaddleSize.y + 49.0f};
    Paddle paddle(nearBottomPosition, kMinY, kMaxY);
    
    paddle.setVelocity(Paddle::Velocity::kDown);
    
    // Update multiple times to ensure it would go past boundary
    for (int i = 0; i < 100; ++i) {
        paddle.update(kDeltaTime);
    }
    
    auto finalExtent = paddle.getExtent();
    EXPECT_FLOAT_EQ(finalExtent.getY(), kMaxY - pong::kPaddleSize.y);
}

// Collision Detection Tests
TEST_F(PaddleTest, CollisionDetectionPositive) {
    Paddle paddle(initialPosition, kMinY, kMaxY);
    auto paddleExtent = paddle.getExtent();
    
    // Create overlapping rectangle
    sdl::Rectangle<float> overlappingRect{
        paddleExtent.getX() + 5.0f,
        paddleExtent.getY() + 5.0f,
        10.0f,
        10.0f
    };
    
    EXPECT_TRUE(paddle.checkCollision(overlappingRect));
}

TEST_F(PaddleTest, CollisionDetectionNegative) {
    Paddle paddle(initialPosition, kMinY, kMaxY);
    
    // Create non-overlapping rectangle
    sdl::Rectangle<float> separateRect{
        kTestX + 100.0f,
        kTestY + 100.0f,
        10.0f,
        10.0f
    };
    
    EXPECT_FALSE(paddle.checkCollision(separateRect));
}

TEST_F(PaddleTest, CollisionDetectionEdgeCase) {
    Paddle paddle(initialPosition, kMinY, kMaxY);
    auto paddleExtent = paddle.getExtent();
    
    // Create rectangle that just touches edge (with small gap to avoid intersection)
    sdl::Rectangle<float> touchingRect{
        paddleExtent.getX() + paddleExtent.getWidth() + 1.0f, // Add 1 pixel gap
        paddleExtent.getY(),
        10.0f,
        10.0f
    };
    
    // Should not intersect with gap
    EXPECT_FALSE(paddle.checkCollision(touchingRect));
}

// Collision Zone Determination Tests
TEST_F(PaddleTest, CollisionZoneTop) {
    Paddle paddle(initialPosition, kMinY, kMaxY);
    auto paddleExtent = paddle.getExtent();
    
    // Ball in top third of paddle
    sdl::Rectangle<float> ballInTopZone{
        paddleExtent.getX() + 5.0f,
        paddleExtent.getY() + 5.0f,
        pong::kBallSize.x,
        pong::kBallSize.y
    };
    
    auto zone = paddle.determineCollisionZone(ballInTopZone);
    EXPECT_EQ(zone, PaddleCollisionEvent::Zone::kTop);
}

TEST_F(PaddleTest, CollisionZoneMiddle) {
    Paddle paddle(initialPosition, kMinY, kMaxY);
    auto paddleExtent = paddle.getExtent();
    
    // Ball in middle third of paddle (center of paddle)
    float ballCenterY = paddleExtent.getY() + paddleExtent.getHeight() / 2.0f - pong::kBallSize.y / 2.0f;
    sdl::Rectangle<float> ballInMiddleZone{
        paddleExtent.getX() + 5.0f,
        ballCenterY,
        pong::kBallSize.x,
        pong::kBallSize.y
    };
    
    auto zone = paddle.determineCollisionZone(ballInMiddleZone);
    EXPECT_EQ(zone, PaddleCollisionEvent::Zone::kMiddle);
}

TEST_F(PaddleTest, CollisionZoneBottom) {
    Paddle paddle(initialPosition, kMinY, kMaxY);
    auto paddleExtent = paddle.getExtent();
    
    // Ball in bottom third of paddle
    float ballY = paddleExtent.getY() + paddleExtent.getHeight() - pong::kBallSize.y - 5.0f;
    sdl::Rectangle<float> ballInBottomZone{
        paddleExtent.getX() + 5.0f,
        ballY,
        pong::kBallSize.x,
        pong::kBallSize.y
    };
    
    auto zone = paddle.determineCollisionZone(ballInBottomZone);
    EXPECT_EQ(zone, PaddleCollisionEvent::Zone::kBottom);
}

TEST_F(PaddleTest, CollisionZoneBoundaryTopMiddle) {
    Paddle paddle(initialPosition, kMinY, kMaxY);
    auto paddleExtent = paddle.getExtent();
    
    // Ball exactly at boundary between top and middle (should be middle)
    float paddleThird = paddleExtent.getHeight() / pong::kPaddleCollisionZoneDivisions;
    float ballCenterY = paddleExtent.getY() + paddleThird;
    float ballY = ballCenterY - pong::kBallSize.y / 2.0f;
    
    sdl::Rectangle<float> ballAtBoundary{
        paddleExtent.getX() + 5.0f,
        ballY,
        pong::kBallSize.x,
        pong::kBallSize.y
    };
    
    auto zone = paddle.determineCollisionZone(ballAtBoundary);
    EXPECT_EQ(zone, PaddleCollisionEvent::Zone::kMiddle);
}

TEST_F(PaddleTest, VelocityStateChanges) {
    Paddle paddle(initialPosition, kMinY, kMaxY);
    
    // Test velocity state changes
    paddle.setVelocity(Paddle::Velocity::kUp);
    paddle.update(kDeltaTime);
    auto upPosition = paddle.getExtent().getY();
    
    paddle.setVelocity(Paddle::Velocity::kStopped);
    paddle.update(kDeltaTime);
    auto stoppedPosition = paddle.getExtent().getY();
    
    paddle.setVelocity(Paddle::Velocity::kDown);
    paddle.update(kDeltaTime);
    auto downPosition = paddle.getExtent().getY();
    
    // Verify movement directions
    EXPECT_LT(upPosition, kTestY - pong::kPaddleSize.y / 2.0f); // Moved up (Y decreased)
    EXPECT_FLOAT_EQ(stoppedPosition, upPosition); // Didn't move when stopped
    EXPECT_GT(downPosition, stoppedPosition); // Moved down (Y increased)
}