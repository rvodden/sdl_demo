#include <gtest/gtest.h>
#include <sprite.h>
#include <rectangle.h>

// Simple compilation test to ensure sdlpp_tools headers work
TEST(SdlToolsTest, testBasicCompilation) {
  // Just test that we can create a rectangle (this doesn't need SDL context)
  constexpr float kX = 10.0F;
  constexpr float kY = 20.0F;
  constexpr float kWidth = 100.0F;
  constexpr float kHeight = 200.0F;
  
  sdlpp::Rectangle<float> rect(kX, kY, kWidth, kHeight);
  
  ASSERT_EQ(rect.getX(), kX);
  ASSERT_EQ(rect.getY(), kY);
  ASSERT_EQ(rect.getWidth(), kWidth);
  ASSERT_EQ(rect.getHeight(), kHeight);
}
