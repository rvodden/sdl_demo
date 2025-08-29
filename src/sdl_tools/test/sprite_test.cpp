#include <cstdint>
#include <string>
#include <memory>
#include <utility>

#include <gtest/gtest.h>

#include "rectangle.h"
#include "renderer.h"
#include "sdl.h"
#include "sprite.h"
#include "texture.h"
#include "window.h"

constexpr uint16_t kTestWidth = 800;
constexpr uint16_t kTestHeight = 600;

class SpriteTest : public ::testing::Test {
protected:
  void SetUp() override {
    window = std::make_unique<sdl::Window>("Test Window", kTestWidth, kTestHeight, 0);
    renderer = std::make_unique<sdl::Renderer>(*window);
    
    // Create a simple 1x1 red pixel texture
    constexpr uint32_t redPixel = 0xFF0000FF;  // RGBA format: red with full alpha
    texture = std::make_shared<sdl::Texture>(*renderer, 1, 1, &redPixel);
  }

  void TearDown() override {
    texture.reset();
    renderer.reset();
    window.reset();
  }

  sdl::SDL sdl;
  std::unique_ptr<sdl::Window> window;
  std::unique_ptr<sdl::Renderer> renderer;
  std::shared_ptr<sdl::Texture> texture;
};

TEST_F(SpriteTest, testSpriteCreation) {
  constexpr float kRectSize = 100.0F;
  constexpr float kZero = 0.0F;
  sdl::Rectangle<float> rect(kZero, kZero, kRectSize, kRectSize);
  
  // Should not throw when creating sprite
  ASSERT_NO_THROW(sdl::tools::Sprite sprite(texture, rect));
}

TEST_F(SpriteTest, testSpriteMoveConstructor) {
  constexpr float kX = 10.0F;
  constexpr float kY = 20.0F;
  constexpr float kWidth = 50.0F;
  constexpr float kHeight = 60.0F;
  sdl::Rectangle<float> rect(kX, kY, kWidth, kHeight);
  sdl::tools::Sprite sprite1(texture, rect);
  
  // Move constructor should work
  ASSERT_NO_THROW(sdl::tools::Sprite sprite2(std::move(sprite1)));
}

TEST_F(SpriteTest, testSpriteMoveAssignment) {
  constexpr float kX1 = 10.0F;
  constexpr float kY1 = 20.0F;
  constexpr float kWidth1 = 30.0F;
  constexpr float kHeight1 = 40.0F;
  constexpr float kX2 = 50.0F;
  constexpr float kY2 = 60.0F;
  constexpr float kWidth2 = 70.0F;
  constexpr float kHeight2 = 80.0F;
  
  sdl::Rectangle<float> rect1(kX1, kY1, kWidth1, kHeight1);
  sdl::Rectangle<float> rect2(kX2, kY2, kWidth2, kHeight2);
  
  sdl::tools::Sprite sprite1(texture, rect1);
  sdl::tools::Sprite sprite2(texture, rect2);
  
  // Move assignment should work
  ASSERT_NO_THROW(sprite2 = std::move(sprite1));
}

TEST_F(SpriteTest, testSpriteWithDifferentRectangles) {
  constexpr float kZero = 0.0F;
  constexpr float kSmallSize = 16.0F;
  constexpr float kLargeSize = 256.0F;
  constexpr float kMidSize = 64.0F;
  constexpr float kOffset = 100.0F;
  
  // Test with various rectangle configurations
  sdl::Rectangle<float> smallRect(kZero, kZero, kSmallSize, kSmallSize);
  sdl::Rectangle<float> largeRect(kZero, kZero, kLargeSize, kLargeSize);
  sdl::Rectangle<float> offsetRect(kOffset, kOffset, kMidSize, kMidSize);
  
  ASSERT_NO_THROW(sdl::tools::Sprite sprite1(texture, smallRect));
  ASSERT_NO_THROW(sdl::tools::Sprite sprite2(texture, largeRect));
  ASSERT_NO_THROW(sdl::tools::Sprite sprite3(texture, offsetRect));
}

TEST_F(SpriteTest, testSpriteWithNegativeCoordinates) {
  constexpr float kNegX = -10.0F;
  constexpr float kNegY = -20.0F;
  constexpr float kWidth = 50.0F;
  constexpr float kHeight = 60.0F;
  sdl::Rectangle<float> rect(kNegX, kNegY, kWidth, kHeight);
  
  // Should handle negative coordinates
  ASSERT_NO_THROW(sdl::tools::Sprite sprite(texture, rect));
}

TEST_F(SpriteTest, testSpriteWithZeroDimensions) {
  constexpr float kZero = 0.0F;
  sdl::Rectangle<float> rect(kZero, kZero, kZero, kZero);
  
  // Should handle zero dimensions
  ASSERT_NO_THROW(sdl::tools::Sprite sprite(texture, rect));
}
