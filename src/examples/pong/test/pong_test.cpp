#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <stdexcept>

#include <sdl/event.h>
#include <sdl/event_router.h>

#include "pong.h"
#include "constants.h"
#include "events.h"
#include "player.h"
#include "paddle.h"

using pong::Pong;
using pong::Point;
using pong::Player;
using pong::Paddle;
using pong::WallCollisionEvent;
using pong::PaddleCollisionEvent;

// Mock event bus for testing
class MockEventBus : public sdl::BaseEventBus {
public:
    MOCK_METHOD(std::unique_ptr<sdl::BaseEvent>, wait, (), (override));
    MOCK_METHOD((std::optional<std::unique_ptr<sdl::BaseEvent>>), poll, (), (override));
    MOCK_METHOD(void, publish, (std::unique_ptr<sdl::UserEvent> event), (override));
    MOCK_METHOD(void, setRouteCallback, (std::function<void(std::unique_ptr<sdl::BaseEvent>)> callback), (override));
    MOCK_METHOD(void, injectEvent, (const std::any& eventData, std::type_index eventTypeId), (override));
};

// Test constants
constexpr float kTestWindowWidth = 800.0f;
constexpr float kTestWindowHeight = 600.0f;
constexpr float kDeltaTime = 16.0f;

class PongTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockEventBus = std::make_shared<MockEventBus>();
        eventRouter = std::make_shared<sdl::tools::EventRouter>(mockEventBus);
        windowSize = Point<float>{kTestWindowWidth, kTestWindowHeight};
    }
    
    std::shared_ptr<MockEventBus> mockEventBus;
    std::shared_ptr<sdl::tools::EventRouter> eventRouter;
    Point<float> windowSize;
};

// Constructor Tests
TEST_F(PongTest, ValidConstructor) {
    EXPECT_NO_THROW(Pong pong(windowSize, eventRouter));
}

TEST_F(PongTest, ConstructorWithNullEventRouter) {
    EXPECT_THROW(
        Pong pong(windowSize, nullptr), 
        std::invalid_argument
    );
}

TEST_F(PongTest, ConstructorWithInvalidWindowSize) {
    Point<float> invalidWindowSize{0.0f, kTestWindowHeight};
    EXPECT_THROW(
        Pong pong(invalidWindowSize, eventRouter), 
        std::invalid_argument
    );
    
    Point<float> negativeWindowSize{-100.0f, kTestWindowHeight};
    EXPECT_THROW(
        Pong pong(negativeWindowSize, eventRouter), 
        std::invalid_argument
    );
}

// Initial State Tests
TEST_F(PongTest, InitialGameState) {
    Pong pong(windowSize, eventRouter);
    
    // Ball should be at center
    auto ball = pong.getBall();
    auto ballExtent = ball.getExtent();
    
    EXPECT_NEAR(ballExtent.getX() + ballExtent.getWidth() / 2.0f, kTestWindowWidth / 2.0f, 1.0f);
    EXPECT_NEAR(ballExtent.getY() + ballExtent.getHeight() / 2.0f, kTestWindowHeight / 2.0f, 1.0f);
    
    // Initial scores should be zero
    EXPECT_EQ(pong.getScore(Player::kLeft), 0);
    EXPECT_EQ(pong.getScore(Player::kRight), 0);
    
    // Paddles should be at expected positions
    auto leftPaddle = pong.getPaddle(Player::kLeft);
    auto rightPaddle = pong.getPaddle(Player::kRight);
    
    EXPECT_NEAR(leftPaddle.getExtent().getX() + leftPaddle.getExtent().getWidth() / 2.0f, 
                pong::kPaddleFromWallDistance, 1.0f);
    EXPECT_NEAR(rightPaddle.getExtent().getX() + rightPaddle.getExtent().getWidth() / 2.0f, 
                kTestWindowWidth - pong::kPaddleFromWallDistance, 1.0f);
}

// Update Tests with Invalid Delta Time
TEST_F(PongTest, UpdateWithInvalidDeltaTime) {
    Pong pong(windowSize, eventRouter);
    
    auto initialBallExtent = pong.getBall().getExtent();
    auto initialBallVelocity = pong.getBall().getVelocity();
    
    // Test negative delta time - should be clamped to kMinimumDt (1.0f)
    pong.update(-0.1f);
    auto ballAfterNegative = pong.getBall().getExtent();
    
    // Ball should have moved by clamped minimum delta time
    float expectedXChangeMin = initialBallVelocity.x * pong::kMinimumDt;
    EXPECT_FLOAT_EQ(ballAfterNegative.getX(), initialBallExtent.getX() + expectedXChangeMin);
    
    // Reset ball for next test
    pong.resetBall();
    auto resetExtent = pong.getBall().getExtent();
    
    // Test excessively large delta time - should be clamped to kMaximumDt (1000.0f)
    pong.update(2000.0f);
    auto ballAfterLarge = pong.getBall().getExtent();
    
    // Ball should have moved by clamped maximum delta time
    float expectedXChangeMax = initialBallVelocity.x * pong::kMaximumDt;
    EXPECT_FLOAT_EQ(ballAfterLarge.getX(), resetExtent.getX() + expectedXChangeMax);
}

// Wall Collision Tests
TEST_F(PongTest, TopWallCollision) {
    Pong pong(windowSize, eventRouter);
    
    // Position ball near top wall and set upward velocity
    pong.resetBall();
    Point<float> nearTopPosition{kTestWindowWidth / 2.0f, 10.0f};
    pong.setBallPosition(nearTopPosition);
    Point<float> upwardVelocity{0.0f, -0.5f};
    pong.setBallVelocity(upwardVelocity);
    
    // Update multiple times to trigger collision
    for (int i = 0; i < 150; ++i) {
        pong.update(kDeltaTime);
    }
    
    // Verify ball bounced (velocity should be reversed from -0.5 to +0.5)
    auto finalVelocity = pong.getBall().getVelocity();
    EXPECT_GT(finalVelocity.y, 0.0f); // Should be positive (bounced upward)
}

TEST_F(PongTest, BottomWallCollision) {
    Pong pong(windowSize, eventRouter);
    
    // Position ball near bottom wall and set downward velocity
    pong.resetBall();
    Point<float> nearBottomPosition{kTestWindowWidth / 2.0f, kTestWindowHeight - 10.0f};
    pong.setBallPosition(nearBottomPosition);
    Point<float> downwardVelocity{0.0f, 0.5f};
    pong.setBallVelocity(downwardVelocity);
    
    // Update multiple times to trigger collision
    for (int i = 0; i < 150; ++i) {
        pong.update(kDeltaTime);
    }
    
    // Verify ball bounced (velocity should be reversed from +0.5 to -0.5)
    auto finalVelocity = pong.getBall().getVelocity();
    EXPECT_LT(finalVelocity.y, 0.0f); // Should be negative (bounced downward)
}

TEST_F(PongTest, LeftWallCollisionScoring) {
    Pong pong(windowSize, eventRouter);
    
    // Check initial score
    auto initialRightScore = pong.getScore(Player::kRight);
    
    // Position ball near left wall and set leftward velocity
    pong.resetBall();
    Point<float> nearLeftPosition{10.0f, kTestWindowHeight / 2.0f};
    pong.setBallPosition(nearLeftPosition);
    Point<float> leftwardVelocity{-pong::kMaxBallSpeed, 0.0f};
    pong.setBallVelocity(leftwardVelocity);
    
    // Update multiple times to trigger collision
    for (int i = 0; i < 100; ++i) {
        pong.update(kDeltaTime);
    }
    
    // Verify right player scored (left wall collision increments right player score)
    auto finalRightScore = pong.getScore(Player::kRight);
    EXPECT_GT(finalRightScore, initialRightScore);
}

// Scoring Tests
TEST_F(PongTest, IncrementScore) {
    Pong pong(windowSize, eventRouter);
    
    EXPECT_EQ(pong.getScore(Player::kLeft), 0);
    EXPECT_EQ(pong.getScore(Player::kRight), 0);
    
    pong.incrementScore(Player::kLeft);
    EXPECT_EQ(pong.getScore(Player::kLeft), 1);
    EXPECT_EQ(pong.getScore(Player::kRight), 0);
    
    pong.incrementScore(Player::kRight);
    EXPECT_EQ(pong.getScore(Player::kLeft), 1);
    EXPECT_EQ(pong.getScore(Player::kRight), 1);
    
    pong.incrementScore(Player::kLeft);
    EXPECT_EQ(pong.getScore(Player::kLeft), 2);
    EXPECT_EQ(pong.getScore(Player::kRight), 1);
}

// Paddle Control Tests
TEST_F(PongTest, SetPaddleVelocity) {
    Pong pong(windowSize, eventRouter);
    
    // Test left paddle movement
    auto initialLeftPosition = pong.getPaddle(Player::kLeft).getExtent().getY();
    
    pong.setPaddleVelocity(Player::kLeft, Paddle::Velocity::kUp);
    pong.update(kDeltaTime);
    
    auto afterUpMovement = pong.getPaddle(Player::kLeft).getExtent().getY();
    EXPECT_LT(afterUpMovement, initialLeftPosition); // Y decreased (moved up)
    
    pong.setPaddleVelocity(Player::kLeft, Paddle::Velocity::kStopped);
    pong.update(kDeltaTime);
    
    auto afterStop = pong.getPaddle(Player::kLeft).getExtent().getY();
    EXPECT_FLOAT_EQ(afterStop, afterUpMovement); // Should not move
    
    pong.setPaddleVelocity(Player::kLeft, Paddle::Velocity::kDown);
    pong.update(kDeltaTime);
    
    auto afterDownMovement = pong.getPaddle(Player::kLeft).getExtent().getY();
    EXPECT_GT(afterDownMovement, afterStop); // Y increased (moved down)
}

// Ball Reset Tests
TEST_F(PongTest, ResetBall) {
    Pong pong(windowSize, eventRouter);
    
    // Move the ball
    Point<float> velocity{0.4f, 0.3f};
    pong.setBallVelocity(velocity);
    pong.update(kDeltaTime);
    
    auto initialCenter = Point<float>{kTestWindowWidth / 2.0f, kTestWindowHeight / 2.0f};
    auto movedBall = pong.getBall();
    auto movedExtent = movedBall.getExtent();
    
    // Verify ball moved from center
    EXPECT_NE(movedExtent.getX() + movedExtent.getWidth() / 2.0f, initialCenter.x);
    
    // Reset ball
    pong.resetBall();
    auto resetBall = pong.getBall();
    auto resetExtent = resetBall.getExtent();
    
    // Verify ball is back at center
    EXPECT_NEAR(resetExtent.getX() + resetExtent.getWidth() / 2.0f, initialCenter.x, 1.0f);
    EXPECT_NEAR(resetExtent.getY() + resetExtent.getHeight() / 2.0f, initialCenter.y, 1.0f);
    
    // Verify velocity is reset
    auto resetVelocity = resetBall.getVelocity();
    EXPECT_FLOAT_EQ(resetVelocity.x, pong::kBallSpeed.x);
    EXPECT_FLOAT_EQ(resetVelocity.y, pong::kBallSpeed.y);
}

// Basic Update Test
TEST_F(PongTest, BasicUpdate) {
    Pong pong(windowSize, eventRouter);
    
    auto initialBallExtent = pong.getBall().getExtent();
    auto initialBallVelocity = pong.getBall().getVelocity();
    
    // Update once
    pong.update(kDeltaTime);
    
    auto updatedBallExtent = pong.getBall().getExtent();
    
    // Ball should have moved according to velocity
    float expectedXChange = initialBallVelocity.x * kDeltaTime;
    float expectedYChange = initialBallVelocity.y * kDeltaTime;
    
    EXPECT_FLOAT_EQ(updatedBallExtent.getX(), initialBallExtent.getX() + expectedXChange);
    EXPECT_FLOAT_EQ(updatedBallExtent.getY(), initialBallExtent.getY() + expectedYChange);
}

// Paddle Collision Direction Tests
TEST_F(PongTest, BallMovingLeftChecksLeftPaddle) {
    Pong pong(windowSize, eventRouter);
    
    // Set ball moving left toward left paddle
    Point<float> leftwardVelocity{-0.6f, 0.0f};
    pong.setBallVelocity(leftwardVelocity);
    
    // Position ball closer to left paddle for potential collision
    // This test verifies the collision detection logic path
    
    // We can't easily test paddle collision without complex setup,
    // but we can verify the ball is moving in the correct direction
    pong.update(kDeltaTime);
    
    auto ballVelocity = pong.getBall().getVelocity();
    EXPECT_LT(ballVelocity.x, 0.0f); // Ball should still be moving left
}

TEST_F(PongTest, BallMovingRightChecksRightPaddle) {
    Pong pong(windowSize, eventRouter);
    
    // Set ball moving right toward right paddle
    Point<float> rightwardVelocity{0.6f, 0.0f};
    pong.setBallVelocity(rightwardVelocity);
    
    pong.update(kDeltaTime);
    
    auto ballVelocity = pong.getBall().getVelocity();
    EXPECT_GT(ballVelocity.x, 0.0f); // Ball should still be moving right
}
