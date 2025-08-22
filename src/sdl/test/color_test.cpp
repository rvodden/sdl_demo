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
  sdlpp::Color color;
  ASSERT_EQ(color.getRed(), 0);
  ASSERT_EQ(color.getGreen(), 0);
  ASSERT_EQ(color.getBlue(), 0);
  ASSERT_EQ(color.getAlpha(), static_cast<uint8_t>(sdlpp::Alpha::kOpaque));
}

TEST(ColorTest, testParameterizedConstructor) {
  sdlpp::Color color(kMaxColorValue, kHalfColorValue, kQuarterColorValue, kEighthColorValue);
  ASSERT_EQ(color.getRed(), kMaxColorValue);
  ASSERT_EQ(color.getGreen(), kHalfColorValue);
  ASSERT_EQ(color.getBlue(), kQuarterColorValue);
  ASSERT_EQ(color.getAlpha(), kEighthColorValue);
}

TEST(ColorTest, testConstexprConstructor) {
  constexpr sdlpp::Color color(kTestRed, kTestGreen, kTestBlue, kTestAlpha);
  ASSERT_EQ(color.getRed(), kTestRed);
  ASSERT_EQ(color.getGreen(), kTestGreen);
  ASSERT_EQ(color.getBlue(), kTestBlue);
  ASSERT_EQ(color.getAlpha(), kTestAlpha);
}

TEST(ColorTest, testNamedColors) {
  // Test black color
  ASSERT_EQ(sdlpp::NamedColor::kBlack.getRed(), 0);
  ASSERT_EQ(sdlpp::NamedColor::kBlack.getGreen(), 0);
  ASSERT_EQ(sdlpp::NamedColor::kBlack.getBlue(), 0);
  ASSERT_EQ(sdlpp::NamedColor::kBlack.getAlpha(), static_cast<uint8_t>(sdlpp::Alpha::kOpaque));
  
  // Test white color
  ASSERT_EQ(sdlpp::NamedColor::kWhite.getRed(), kMaxColorValue);
  ASSERT_EQ(sdlpp::NamedColor::kWhite.getGreen(), kMaxColorValue);
  ASSERT_EQ(sdlpp::NamedColor::kWhite.getBlue(), kMaxColorValue);
  ASSERT_EQ(sdlpp::NamedColor::kWhite.getAlpha(), static_cast<uint8_t>(sdlpp::Alpha::kOpaque));
  
  // Test magenta color
  ASSERT_EQ(sdlpp::NamedColor::kMagenta.getRed(), kMagentaRed);
  ASSERT_EQ(sdlpp::NamedColor::kMagenta.getGreen(), kMagentaGreen);
  ASSERT_EQ(sdlpp::NamedColor::kMagenta.getBlue(), kMagentaBlue);
  ASSERT_EQ(sdlpp::NamedColor::kMagenta.getAlpha(), static_cast<uint8_t>(sdlpp::Alpha::kOpaque));
}

TEST(ColorTest, testAlphaValues) {
  // Test opaque alpha
  sdlpp::Color opaqueColor(kMaxColorValue, kMaxColorValue, kMaxColorValue, static_cast<uint8_t>(sdlpp::Alpha::kOpaque));
  ASSERT_EQ(opaqueColor.getAlpha(), kMaxColorValue);
  
  // Test transparent alpha
  sdlpp::Color transparentColor(kMaxColorValue, kMaxColorValue, kMaxColorValue, static_cast<uint8_t>(sdlpp::Alpha::kTranparent));
  ASSERT_EQ(transparentColor.getAlpha(), 0);
}
