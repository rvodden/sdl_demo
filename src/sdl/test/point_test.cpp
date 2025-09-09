#include <string>
#include <utility>

#include <gtest/gtest.h>

#include <sdl/point.h>

// Test constants for int32_t points
constexpr int32_t kIntTestX = 10;
constexpr int32_t kIntTestY = 20;

constexpr int32_t kIntCopyX = 15;
constexpr int32_t kIntCopyY = 25;

// Test constants for float points
constexpr float kFloatTestX = 10.5f;
constexpr float kFloatTestY = 20.25f;

constexpr float kFloatCopyX = 15.125f;
constexpr float kFloatCopyY = 25.375f;

// Tests for Point<int32_t>
class IntPointTest : public ::testing::Test {
protected:
    using IntPoint = sdl::Point<int32_t>;
};

TEST_F(IntPointTest, ConstructorAndGetters) {
    IntPoint point(kIntTestX, kIntTestY);
    
    EXPECT_EQ(point.getX(), kIntTestX);
    EXPECT_EQ(point.getY(), kIntTestY);
}

TEST_F(IntPointTest, CopyConstructor) {
    IntPoint point1(kIntCopyX, kIntCopyY);
    IntPoint point2(point1);
    
    EXPECT_EQ(point2.getX(), kIntCopyX);
    EXPECT_EQ(point2.getY(), kIntCopyY);
    
    // Verify original is unchanged
    EXPECT_EQ(point1.getX(), kIntCopyX);
    EXPECT_EQ(point1.getY(), kIntCopyY);
}

TEST_F(IntPointTest, MoveConstructor) {
    IntPoint point1(kIntTestX, kIntTestY);
    IntPoint point2(std::move(point1));
    
    EXPECT_EQ(point2.getX(), kIntTestX);
    EXPECT_EQ(point2.getY(), kIntTestY);
    
    // Note: point1 is in valid but unspecified state after move
}

TEST_F(IntPointTest, CopyAssignmentOperator) {
    IntPoint point1(kIntCopyX, kIntCopyY);
    IntPoint point2(0, 0);
    
    point2 = point1;
    
    EXPECT_EQ(point2.getX(), kIntCopyX);
    EXPECT_EQ(point2.getY(), kIntCopyY);
    
    // Verify original is unchanged
    EXPECT_EQ(point1.getX(), kIntCopyX);
    EXPECT_EQ(point1.getY(), kIntCopyY);
}

TEST_F(IntPointTest, MoveAssignmentOperator) {
    IntPoint point1(kIntTestX, kIntTestY);
    IntPoint point2(0, 0);
    
    point2 = std::move(point1);
    
    EXPECT_EQ(point2.getX(), kIntTestX);
    EXPECT_EQ(point2.getY(), kIntTestY);
    
    // Note: point1 is in valid but unspecified state after move
}

TEST_F(IntPointTest, SelfAssignment) {
    IntPoint point(kIntTestX, kIntTestY);
    
    point = point;  // Self-assignment
    
    EXPECT_EQ(point.getX(), kIntTestX);
    EXPECT_EQ(point.getY(), kIntTestY);
}

TEST_F(IntPointTest, ExplicitConversionFromFloat) {
    sdl::Point<float> floatPoint(10.7f, 20.9f);
    IntPoint intPoint(floatPoint);  // Explicit conversion (truncation)
    
    EXPECT_EQ(intPoint.getX(), 10);
    EXPECT_EQ(intPoint.getY(), 20);
}

// Tests for Point<float>
class FloatPointTest : public ::testing::Test {
protected:
    using FloatPoint = sdl::Point<float>;
};

TEST_F(FloatPointTest, ConstructorAndGetters) {
    FloatPoint point(kFloatTestX, kFloatTestY);
    
    EXPECT_FLOAT_EQ(point.getX(), kFloatTestX);
    EXPECT_FLOAT_EQ(point.getY(), kFloatTestY);
}

TEST_F(FloatPointTest, CopyConstructor) {
    FloatPoint point1(kFloatCopyX, kFloatCopyY);
    FloatPoint point2(point1);
    
    EXPECT_FLOAT_EQ(point2.getX(), kFloatCopyX);
    EXPECT_FLOAT_EQ(point2.getY(), kFloatCopyY);
    
    // Verify original is unchanged
    EXPECT_FLOAT_EQ(point1.getX(), kFloatCopyX);
    EXPECT_FLOAT_EQ(point1.getY(), kFloatCopyY);
}

TEST_F(FloatPointTest, MoveConstructor) {
    FloatPoint point1(kFloatTestX, kFloatTestY);
    FloatPoint point2(std::move(point1));
    
    EXPECT_FLOAT_EQ(point2.getX(), kFloatTestX);
    EXPECT_FLOAT_EQ(point2.getY(), kFloatTestY);
    
    // Note: point1 is in valid but unspecified state after move
}

TEST_F(FloatPointTest, CopyAssignmentOperator) {
    FloatPoint point1(kFloatCopyX, kFloatCopyY);
    FloatPoint point2(0.0f, 0.0f);
    
    point2 = point1;
    
    EXPECT_FLOAT_EQ(point2.getX(), kFloatCopyX);
    EXPECT_FLOAT_EQ(point2.getY(), kFloatCopyY);
    
    // Verify original is unchanged
    EXPECT_FLOAT_EQ(point1.getX(), kFloatCopyX);
    EXPECT_FLOAT_EQ(point1.getY(), kFloatCopyY);
}

TEST_F(FloatPointTest, MoveAssignmentOperator) {
    FloatPoint point1(kFloatTestX, kFloatTestY);
    FloatPoint point2(0.0f, 0.0f);
    
    point2 = std::move(point1);
    
    EXPECT_FLOAT_EQ(point2.getX(), kFloatTestX);
    EXPECT_FLOAT_EQ(point2.getY(), kFloatTestY);
    
    // Note: point1 is in valid but unspecified state after move
}

TEST_F(FloatPointTest, SelfAssignment) {
    FloatPoint point(kFloatTestX, kFloatTestY);
    
    point = point;  // Self-assignment
    
    EXPECT_FLOAT_EQ(point.getX(), kFloatTestX);
    EXPECT_FLOAT_EQ(point.getY(), kFloatTestY);
}

TEST_F(FloatPointTest, ExplicitConversionFromInt) {
    sdl::Point<int32_t> intPoint(10, 20);
    FloatPoint floatPoint(intPoint);  // Explicit conversion
    
    EXPECT_FLOAT_EQ(floatPoint.getX(), 10.0f);
    EXPECT_FLOAT_EQ(floatPoint.getY(), 20.0f);
}

TEST_F(FloatPointTest, ExplicitConversionWithPrecision) {
    sdl::Point<int32_t> intPoint(-5, 100);
    FloatPoint floatPoint(intPoint);  // Explicit conversion
    
    EXPECT_FLOAT_EQ(floatPoint.getX(), -5.0f);
    EXPECT_FLOAT_EQ(floatPoint.getY(), 100.0f);
}

// Cross-type conversion tests
class PointConversionTest : public ::testing::Test {
};

TEST_F(PointConversionTest, IntToFloatConversion) {
    sdl::Point<int32_t> intPoint(42, -17);
    sdl::Point<float> floatPoint(intPoint);
    
    EXPECT_FLOAT_EQ(floatPoint.getX(), 42.0f);
    EXPECT_FLOAT_EQ(floatPoint.getY(), -17.0f);
}

TEST_F(PointConversionTest, FloatToIntConversionTruncation) {
    sdl::Point<float> floatPoint(3.14159f, -2.71828f);
    sdl::Point<int32_t> intPoint(floatPoint);
    
    EXPECT_EQ(intPoint.getX(), 3);
    EXPECT_EQ(intPoint.getY(), -2);
}

TEST_F(PointConversionTest, EdgeCaseConversions) {
    // Test zero values
    sdl::Point<int32_t> zeroInt(0, 0);
    sdl::Point<float> zeroFloat(zeroInt);
    
    EXPECT_FLOAT_EQ(zeroFloat.getX(), 0.0f);
    EXPECT_FLOAT_EQ(zeroFloat.getY(), 0.0f);
    
    // Test negative values with fractional parts
    sdl::Point<float> negativeFloat(-10.99f, 5.01f);
    sdl::Point<int32_t> negativeInt(negativeFloat);
    
    EXPECT_EQ(negativeInt.getX(), -10);
    EXPECT_EQ(negativeInt.getY(), 5);
}

// Type alias tests
TEST(PointAliasTest, IntPointAlias) {
    sdl::IntPoint point(100, 200);
    
    EXPECT_EQ(point.getX(), 100);
    EXPECT_EQ(point.getY(), 200);
}

TEST(PointAliasTest, FloatPointAlias) {
    sdl::FloatPoint point(100.5f, 200.25f);
    
    EXPECT_FLOAT_EQ(point.getX(), 100.5f);
    EXPECT_FLOAT_EQ(point.getY(), 200.25f);
}

// Test chaining operations
TEST(PointOperationTest, CopyAndMoveChaining) {
    sdl::Point<int32_t> point1(10, 20);
    sdl::Point<int32_t> point2(point1);  // Copy
    sdl::Point<int32_t> point3(std::move(point2));  // Move
    sdl::Point<int32_t> point4(0, 0);
    
    point4 = point3;  // Copy assignment
    
    EXPECT_EQ(point4.getX(), 10);
    EXPECT_EQ(point4.getY(), 20);
}