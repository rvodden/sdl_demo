#include <gtest/gtest.h>
#include <float_rectangle.h>

constexpr float kTestX = 10.5F;
constexpr float kTestY = 20.5F;
constexpr float kTestWidth = 100.0F;
constexpr float kTestHeight = 200.0F;

constexpr float kCopyX = 10.0F;
constexpr float kCopyY = 20.0F;
constexpr float kCopyWidth = 30.0F;
constexpr float kCopyHeight = 40.0F;

constexpr float kMoveX = 15.0F;
constexpr float kMoveY = 25.0F;
constexpr float kMoveWidth = 35.0F;
constexpr float kMoveHeight = 45.0F;

constexpr float kAssignX = 12.0F;
constexpr float kAssignY = 22.0F;
constexpr float kAssignWidth = 32.0F;
constexpr float kAssignHeight = 42.0F;

constexpr float kRectX = 10.0F;
constexpr float kRectY = 20.0F;
constexpr float kRectWidth = 50.0F;
constexpr float kRectHeight = 40.0F;

constexpr float kNegX = -10.0F;
constexpr float kNegY = -5.0F;
constexpr float kNegWidth = 20.0F;
constexpr float kNegHeight = 10.0F;

constexpr float kZero = 0.0F;

TEST(FloatRectangleTest, testConstructorAndGetters) {
  sdl::FloatRectangle rect(kTestX, kTestY, kTestWidth, kTestHeight);
  
  ASSERT_FLOAT_EQ(rect.getX(), kTestX);
  ASSERT_FLOAT_EQ(rect.getY(), kTestY);
  ASSERT_FLOAT_EQ(rect.getWidth(), kTestWidth);
  ASSERT_FLOAT_EQ(rect.getHeight(), kTestHeight);
}

TEST(FloatRectangleTest, testCopyConstructor) {
  sdl::FloatRectangle rect1(kCopyX, kCopyY, kCopyWidth, kCopyHeight);
  sdl::FloatRectangle rect2(rect1);
  
  ASSERT_FLOAT_EQ(rect2.getX(), kCopyX);
  ASSERT_FLOAT_EQ(rect2.getY(), kCopyY);
  ASSERT_FLOAT_EQ(rect2.getWidth(), kCopyWidth);
  ASSERT_FLOAT_EQ(rect2.getHeight(), kCopyHeight);
}

TEST(FloatRectangleTest, testMoveConstructor) {
  sdl::FloatRectangle rect1(kMoveX, kMoveY, kMoveWidth, kMoveHeight);
  sdl::FloatRectangle rect2(std::move(rect1));
  
  ASSERT_FLOAT_EQ(rect2.getX(), kMoveX);
  ASSERT_FLOAT_EQ(rect2.getY(), kMoveY);
  ASSERT_FLOAT_EQ(rect2.getWidth(), kMoveWidth);
  ASSERT_FLOAT_EQ(rect2.getHeight(), kMoveHeight);
}

TEST(FloatRectangleTest, testCopyAssignment) {
  sdl::FloatRectangle rect1(kCopyX, kCopyY, kCopyWidth, kCopyHeight);
  sdl::FloatRectangle rect2(kZero, kZero, kZero, kZero);
  
  rect2 = rect1;
  
  ASSERT_FLOAT_EQ(rect2.getX(), kCopyX);
  ASSERT_FLOAT_EQ(rect2.getY(), kCopyY);
  ASSERT_FLOAT_EQ(rect2.getWidth(), kCopyWidth);
  ASSERT_FLOAT_EQ(rect2.getHeight(), kCopyHeight);
}

TEST(FloatRectangleTest, testMoveAssignment) {
  sdl::FloatRectangle rect1(kAssignX, kAssignY, kAssignWidth, kAssignHeight);
  sdl::FloatRectangle rect2(kZero, kZero, kZero, kZero);
  
  rect2 = std::move(rect1);
  
  ASSERT_FLOAT_EQ(rect2.getX(), kAssignX);
  ASSERT_FLOAT_EQ(rect2.getY(), kAssignY);
  ASSERT_FLOAT_EQ(rect2.getWidth(), kAssignWidth);
  ASSERT_FLOAT_EQ(rect2.getHeight(), kAssignHeight);
}

TEST(FloatRectangleTest, testContains) {
  sdl::FloatRectangle rect(kRectX, kRectY, kRectWidth, kRectHeight);
  
  constexpr float kInsideX = 30.0F;
  constexpr float kInsideY = 35.0F;
  constexpr float kNearX = 59.9F;
  constexpr float kNearY = 59.9F;
  constexpr float kOutsideX1 = 5.0F;
  constexpr float kOutsideY1 = 15.0F;
  constexpr float kOutsideX2 = 65.0F;
  constexpr float kOutsideY2 = 65.0F;
  constexpr float kAboveY = 10.0F;
  
  // Test point inside rectangle
  ASSERT_TRUE(rect.contains(kInsideX, kInsideY));
  ASSERT_TRUE(rect.contains(kRectX, kRectY)); // Top-left corner
  ASSERT_TRUE(rect.contains(kNearX, kNearY)); // Near bottom-right corner
  
  // Test point outside rectangle
  ASSERT_FALSE(rect.contains(kOutsideX1, kOutsideY1));   // Before top-left
  ASSERT_FALSE(rect.contains(kOutsideX2, kOutsideY2));  // After bottom-right
  ASSERT_FALSE(rect.contains(kInsideX, kAboveY));  // Above rectangle
  ASSERT_FALSE(rect.contains(kOutsideX1, kInsideY));   // Left of rectangle
}

TEST(FloatRectangleTest, testZeroDimensions) {
  sdl::FloatRectangle rect(kZero, kZero, kZero, kZero);
  
  ASSERT_FLOAT_EQ(rect.getX(), kZero);
  ASSERT_FLOAT_EQ(rect.getY(), kZero);
  ASSERT_FLOAT_EQ(rect.getWidth(), kZero);
  ASSERT_FLOAT_EQ(rect.getHeight(), kZero);
  
  constexpr float kOne = 1.0F;
  // Point at origin should be contained
  ASSERT_TRUE(rect.contains(kZero, kZero));
  ASSERT_FALSE(rect.contains(kOne, kOne));
}

TEST(FloatRectangleTest, testNegativeCoordinates) {
  sdl::FloatRectangle rect(kNegX, kNegY, kNegWidth, kNegHeight);
  
  ASSERT_FLOAT_EQ(rect.getX(), kNegX);
  ASSERT_FLOAT_EQ(rect.getY(), kNegY);
  ASSERT_FLOAT_EQ(rect.getWidth(), kNegWidth);
  ASSERT_FLOAT_EQ(rect.getHeight(), kNegHeight);
  
  constexpr float kNegInsideX = -5.0F;
  constexpr float kNegOutsideX = -15.0F;
  constexpr float kNegOutsideY = -10.0F;
  
  // Test contains with negative coordinates
  ASSERT_TRUE(rect.contains(kNegInsideX, kZero));   // Inside
  ASSERT_TRUE(rect.contains(kNegX, kNegY)); // Top-left corner
  ASSERT_FALSE(rect.contains(kNegOutsideX, kNegOutsideY)); // Outside
}