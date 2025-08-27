#include <gtest/gtest.h>
#include <color.h>

constexpr uint8_t kMaxColorValue = 255;
constexpr uint8_t kHalfColorValue = 128;
constexpr uint8_t kQuarterColorValue = 64;
constexpr uint8_t kEighthColorValue = 32;
constexpr uint8_t kTestRed = 100;
constexpr uint8_t kTestGreen = 150;
constexpr uint8_t kTestBlue = 200;
constexpr uint8_t kTestAlpha = 250;
constexpr uint8_t kMagentaRed = 0xc2;
constexpr uint8_t kMagentaGreen = 0x00;
constexpr uint8_t kMagentaBlue = 0x78;

TEST(ColorTest, testDefaultConstructor) {
  sdl::Color color;
  ASSERT_EQ(color.getRed(), 0);
  ASSERT_EQ(color.getGreen(), 0);
  ASSERT_EQ(color.getBlue(), 0);
  ASSERT_EQ(color.getAlpha(), static_cast<uint8_t>(sdl::Alpha::kOpaque));
}

TEST(ColorTest, testParameterizedConstructor) {
  sdl::Color color(kMaxColorValue, kHalfColorValue, kQuarterColorValue, kEighthColorValue);
  ASSERT_EQ(color.getRed(), kMaxColorValue);
  ASSERT_EQ(color.getGreen(), kHalfColorValue);
  ASSERT_EQ(color.getBlue(), kQuarterColorValue);
  ASSERT_EQ(color.getAlpha(), kEighthColorValue);
}

TEST(ColorTest, testConstexprConstructor) {
  constexpr sdl::Color color(kTestRed, kTestGreen, kTestBlue, kTestAlpha);
  ASSERT_EQ(color.getRed(), kTestRed);
  ASSERT_EQ(color.getGreen(), kTestGreen);
  ASSERT_EQ(color.getBlue(), kTestBlue);
  ASSERT_EQ(color.getAlpha(), kTestAlpha);
}

TEST(ColorTest, testNamedColors) {
  // Test black color
  ASSERT_EQ(sdl::NamedColor::kBlack.getRed(), 0);
  ASSERT_EQ(sdl::NamedColor::kBlack.getGreen(), 0);
  ASSERT_EQ(sdl::NamedColor::kBlack.getBlue(), 0);
  ASSERT_EQ(sdl::NamedColor::kBlack.getAlpha(), static_cast<uint8_t>(sdl::Alpha::kOpaque));
  
  // Test white color
  ASSERT_EQ(sdl::NamedColor::kWhite.getRed(), kMaxColorValue);
  ASSERT_EQ(sdl::NamedColor::kWhite.getGreen(), kMaxColorValue);
  ASSERT_EQ(sdl::NamedColor::kWhite.getBlue(), kMaxColorValue);
  ASSERT_EQ(sdl::NamedColor::kWhite.getAlpha(), static_cast<uint8_t>(sdl::Alpha::kOpaque));
  
  // Test magenta color
  ASSERT_EQ(sdl::NamedColor::kMagenta.getRed(), kMagentaRed);
  ASSERT_EQ(sdl::NamedColor::kMagenta.getGreen(), kMagentaGreen);
  ASSERT_EQ(sdl::NamedColor::kMagenta.getBlue(), kMagentaBlue);
  ASSERT_EQ(sdl::NamedColor::kMagenta.getAlpha(), static_cast<uint8_t>(sdl::Alpha::kOpaque));
}

TEST(ColorTest, testAlphaValues) {
  // Test opaque alpha
  sdl::Color opaqueColor(kMaxColorValue, kMaxColorValue, kMaxColorValue, static_cast<uint8_t>(sdl::Alpha::kOpaque));
  ASSERT_EQ(opaqueColor.getAlpha(), kMaxColorValue);
  
  // Test transparent alpha
  sdl::Color transparentColor(kMaxColorValue, kMaxColorValue, kMaxColorValue, static_cast<uint8_t>(sdl::Alpha::kTranparent));
  ASSERT_EQ(transparentColor.getAlpha(), 0);
}
