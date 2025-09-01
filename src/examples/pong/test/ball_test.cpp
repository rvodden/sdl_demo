#include <gtest/gtest.h>
#include <stdexcept>

#include "ball.h"
#include "constants.h"
#include "point.h"

using pong::Ball;
using pong::Point;

// Test constants
constexpr float kTestX = 100.0f;
constexpr float kTestY = 50.0f;
constexpr float kValidVelocityX = 7.0f;
constexpr float kValidVelocityY = 6.0f;
constexpr float kDeltaTime = 0.016f; // ~60 FPS
constexpr float kLargeDeltaTime = 0.1f;

class BallTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Use valid positive coordinates
        initialPosition = Point<float>{kTestX, kTestY};
    }
    
    Point<float> initialPosition;
};

// Constructor Tests
TEST_F(BallTest, ValidConstructor) {
    Ball ball(initialPosition);
    
    auto extent = ball.getExtent();
    // Ball should be centered at initial position
    EXPECT_FLOAT_EQ(extent.getX(), kTestX - pong::kBallSize.x / 2.0f);
    EXPECT_FLOAT_EQ(extent.getY(), kTestY - pong::kBallSize.y / 2.0f);
    EXPECT_FLOAT_EQ(extent.getWidth(), pong::kBallSize.x);
    EXPECT_FLOAT_EQ(extent.getHeight(), pong::kBallSize.y);
}

TEST_F(BallTest, ConstructorWithNegativePosition) {
    Point<float> negativePosition{-10.0f, kTestY};
    EXPECT_THROW(Ball ball(negativePosition), std::invalid_argument);
    
    Point<float> negativeYPosition{kTestX, -10.0f};
    EXPECT_THROW(Ball ball(negativeYPosition), std::invalid_argument);
}

// Initial Velocity Tests
TEST_F(BallTest, InitialVelocity) {
    Ball ball(initialPosition);
    
    auto velocity = ball.getVelocity();
    EXPECT_FLOAT_EQ(velocity.x, pong::kBallSpeed.x);
    EXPECT_FLOAT_EQ(velocity.y, pong::kBallSpeed.y);
}

// Velocity Setting Tests
TEST_F(BallTest, ValidVelocitySetting) {
    Ball ball(initialPosition);
    Point<float> newVelocity{kValidVelocityX, kValidVelocityY};
    
    ball.setVelocity(newVelocity);
    auto retrievedVelocity = ball.getVelocity();
    
    EXPECT_FLOAT_EQ(retrievedVelocity.x, kValidVelocityX);
    EXPECT_FLOAT_EQ(retrievedVelocity.y, kValidVelocityY);
}

TEST_F(BallTest, ExcessiveVelocityThrowsException) {
    Ball ball(initialPosition);
    
    // Test X velocity too high
    Point<float> excessiveVelocityX{pong::kMaxBallSpeed + 1.0f, kValidVelocityY};
    EXPECT_THROW(ball.setVelocity(excessiveVelocityX), std::invalid_argument);
    
    // Test Y velocity too high
    Point<float> excessiveVelocityY{kValidVelocityX, pong::kMaxBallSpeed + 1.0f};
    EXPECT_THROW(ball.setVelocity(excessiveVelocityY), std::invalid_argument);
    
    // Test negative velocities beyond limit
    Point<float> excessiveNegativeX{-pong::kMaxBallSpeed - 1.0f, kValidVelocityY};
    EXPECT_THROW(ball.setVelocity(excessiveNegativeX), std::invalid_argument);
}

TEST_F(BallTest, MaximumValidVelocity) {
    Ball ball(initialPosition);
    
    // Test maximum valid velocities (should not throw)
    Point<float> maxVelocity{pong::kMaxBallSpeed, pong::kMaxBallSpeed};
    EXPECT_NO_THROW(ball.setVelocity(maxVelocity));
    
    Point<float> maxNegativeVelocity{-pong::kMaxBallSpeed, -pong::kMaxBallSpeed};
    EXPECT_NO_THROW(ball.setVelocity(maxNegativeVelocity));
}

// Position Update Tests
TEST_F(BallTest, PositionUpdateWithPositiveVelocity) {
    Ball ball(initialPosition);
    Point<float> velocity{kValidVelocityX, kValidVelocityY};
    ball.setVelocity(velocity);
    
    auto initialExtent = ball.getExtent();
    ball.update(kDeltaTime);
    auto updatedExtent = ball.getExtent();
    
    float expectedXChange = kValidVelocityX * kDeltaTime;
    float expectedYChange = kValidVelocityY * kDeltaTime;
    
    EXPECT_FLOAT_EQ(updatedExtent.getX(), initialExtent.getX() + expectedXChange);
    EXPECT_FLOAT_EQ(updatedExtent.getY(), initialExtent.getY() + expectedYChange);
}

TEST_F(BallTest, PositionUpdateWithNegativeVelocity) {
    Ball ball(initialPosition);
    Point<float> velocity{-kValidVelocityX, -kValidVelocityY};
    ball.setVelocity(velocity);
    
    auto initialExtent = ball.getExtent();
    ball.update(kDeltaTime);
    auto updatedExtent = ball.getExtent();
    
    float expectedXChange = -kValidVelocityX * kDeltaTime;
    float expectedYChange = -kValidVelocityY * kDeltaTime;
    
    EXPECT_FLOAT_EQ(updatedExtent.getX(), initialExtent.getX() + expectedXChange);
    EXPECT_FLOAT_EQ(updatedExtent.getY(), initialExtent.getY() + expectedYChange);
}

TEST_F(BallTest, PositionUpdateWithZeroVelocity) {
    Ball ball(initialPosition);
    Point<float> zeroVelocity{0.0f, 0.0f};
    ball.setVelocity(zeroVelocity);
    
    auto initialExtent = ball.getExtent();
    ball.update(kDeltaTime);
    auto updatedExtent = ball.getExtent();
    
    EXPECT_FLOAT_EQ(updatedExtent.getX(), initialExtent.getX());
    EXPECT_FLOAT_EQ(updatedExtent.getY(), initialExtent.getY());
}

TEST_F(BallTest, PositionUpdateWithLargeDeltaTime) {
    Ball ball(initialPosition);
    Point<float> velocity{kValidVelocityX, kValidVelocityY};
    ball.setVelocity(velocity);
    
    auto initialExtent = ball.getExtent();
    ball.update(kLargeDeltaTime);
    auto updatedExtent = ball.getExtent();
    
    float expectedXChange = kValidVelocityX * kLargeDeltaTime;
    float expectedYChange = kValidVelocityY * kLargeDeltaTime;
    
    EXPECT_FLOAT_EQ(updatedExtent.getX(), initialExtent.getX() + expectedXChange);
    EXPECT_FLOAT_EQ(updatedExtent.getY(), initialExtent.getY() + expectedYChange);
}

// Reset Tests
TEST_F(BallTest, ResetBallPosition) {
    Ball ball(initialPosition);
    
    // Move the ball
    Point<float> velocity{kValidVelocityX, kValidVelocityY};
    ball.setVelocity(velocity);
    ball.update(kDeltaTime);
    
    // Verify it moved
    auto movedExtent = ball.getExtent();
    EXPECT_NE(movedExtent.getX(), kTestX - pong::kBallSize.x / 2.0f);
    
    // Reset and verify position and velocity
    ball.resetToStartPositionAndVelocity();
    
    auto resetExtent = ball.getExtent();
    auto resetVelocity = ball.getVelocity();
    
    EXPECT_FLOAT_EQ(resetExtent.getX(), kTestX - pong::kBallSize.x / 2.0f);
    EXPECT_FLOAT_EQ(resetExtent.getY(), kTestY - pong::kBallSize.y / 2.0f);
    EXPECT_FLOAT_EQ(resetVelocity.x, pong::kBallSpeed.x);
    EXPECT_FLOAT_EQ(resetVelocity.y, pong::kBallSpeed.y);
}

// Extent Consistency Tests
TEST_F(BallTest, ExtentSizeConsistency) {
    Ball ball(initialPosition);
    
    // After multiple updates, size should remain constant
    ball.setVelocity({kValidVelocityX, kValidVelocityY});
    
    for (int i = 0; i < 10; ++i) {
        ball.update(kDeltaTime);
        auto extent = ball.getExtent();
        EXPECT_FLOAT_EQ(extent.getWidth(), pong::kBallSize.x);
        EXPECT_FLOAT_EQ(extent.getHeight(), pong::kBallSize.y);
    }
}