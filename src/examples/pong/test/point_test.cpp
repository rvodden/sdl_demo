#include <gtest/gtest.h>

#include "point.h"

using pong::Point;

// Test constants for int points
constexpr int kIntTestX = 10;
constexpr int kIntTestY = 20;
constexpr int kIntSecondX = 5;
constexpr int kIntSecondY = 15;
constexpr int kIntScalar = 3;

// Test constants for float points
constexpr float kFloatTestX = 10.5f;
constexpr float kFloatTestY = 20.75f;
constexpr float kFloatSecondX = 5.25f;
constexpr float kFloatSecondY = 15.5f;
constexpr float kFloatScalar = 2.5f;

// Tests for Point<int>
class IntPointTest : public ::testing::Test {
protected:
    using IntPoint = Point<int>;
};

TEST_F(IntPointTest, DefaultConstructor) {
    IntPoint point;
    EXPECT_EQ(point.x, 0);
    EXPECT_EQ(point.y, 0);
}

TEST_F(IntPointTest, ParameterizedConstructor) {
    IntPoint point(kIntTestX, kIntTestY);
    EXPECT_EQ(point.x, kIntTestX);
    EXPECT_EQ(point.y, kIntTestY);
}

TEST_F(IntPointTest, AdditionOperator) {
    IntPoint point1(kIntTestX, kIntTestY);
    IntPoint point2(kIntSecondX, kIntSecondY);
    IntPoint result = point1 + point2;
    
    EXPECT_EQ(result.x, kIntTestX + kIntSecondX);
    EXPECT_EQ(result.y, kIntTestY + kIntSecondY);
    
    // Original points should be unchanged
    EXPECT_EQ(point1.x, kIntTestX);
    EXPECT_EQ(point1.y, kIntTestY);
}

TEST_F(IntPointTest, AdditionAssignmentOperator) {
    IntPoint point1(kIntTestX, kIntTestY);
    IntPoint point2(kIntSecondX, kIntSecondY);
    IntPoint& result = point1 += point2;
    
    EXPECT_EQ(point1.x, kIntTestX + kIntSecondX);
    EXPECT_EQ(point1.y, kIntTestY + kIntSecondY);
    
    // Should return reference to modified object
    EXPECT_EQ(&result, &point1);
}

TEST_F(IntPointTest, ScalarMultiplicationOperator) {
    IntPoint point(kIntTestX, kIntTestY);
    IntPoint result = point * kIntScalar;
    
    EXPECT_EQ(result.x, kIntTestX * kIntScalar);
    EXPECT_EQ(result.y, kIntTestY * kIntScalar);
    
    // Original point should be unchanged
    EXPECT_EQ(point.x, kIntTestX);
    EXPECT_EQ(point.y, kIntTestY);
}

// Tests for Point<float>
class FloatPointTest : public ::testing::Test {
protected:
    using FloatPoint = Point<float>;
};

TEST_F(FloatPointTest, DefaultConstructor) {
    FloatPoint point;
    EXPECT_FLOAT_EQ(point.x, 0.0f);
    EXPECT_FLOAT_EQ(point.y, 0.0f);
}

TEST_F(FloatPointTest, ParameterizedConstructor) {
    FloatPoint point(kFloatTestX, kFloatTestY);
    EXPECT_FLOAT_EQ(point.x, kFloatTestX);
    EXPECT_FLOAT_EQ(point.y, kFloatTestY);
}

TEST_F(FloatPointTest, AdditionOperator) {
    FloatPoint point1(kFloatTestX, kFloatTestY);
    FloatPoint point2(kFloatSecondX, kFloatSecondY);
    FloatPoint result = point1 + point2;
    
    EXPECT_FLOAT_EQ(result.x, kFloatTestX + kFloatSecondX);
    EXPECT_FLOAT_EQ(result.y, kFloatTestY + kFloatSecondY);
}

TEST_F(FloatPointTest, AdditionAssignmentOperator) {
    FloatPoint point1(kFloatTestX, kFloatTestY);
    FloatPoint point2(kFloatSecondX, kFloatSecondY);
    FloatPoint& result = point1 += point2;
    
    EXPECT_FLOAT_EQ(point1.x, kFloatTestX + kFloatSecondX);
    EXPECT_FLOAT_EQ(point1.y, kFloatTestY + kFloatSecondY);
    EXPECT_EQ(&result, &point1);
}

TEST_F(FloatPointTest, ScalarMultiplicationOperator) {
    FloatPoint point(kFloatTestX, kFloatTestY);
    FloatPoint result = point * kFloatScalar;
    
    EXPECT_FLOAT_EQ(result.x, kFloatTestX * kFloatScalar);
    EXPECT_FLOAT_EQ(result.y, kFloatTestY * kFloatScalar);
}

TEST_F(FloatPointTest, ZeroScalarMultiplication) {
    FloatPoint point(kFloatTestX, kFloatTestY);
    FloatPoint result = point * 0.0f;
    
    EXPECT_FLOAT_EQ(result.x, 0.0f);
    EXPECT_FLOAT_EQ(result.y, 0.0f);
}

TEST_F(FloatPointTest, NegativeScalarMultiplication) {
    FloatPoint point(kFloatTestX, kFloatTestY);
    FloatPoint result = point * -1.0f;
    
    EXPECT_FLOAT_EQ(result.x, -kFloatTestX);
    EXPECT_FLOAT_EQ(result.y, -kFloatTestY);
}