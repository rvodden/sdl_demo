#include <gtest/gtest.h>
#include <rectangle.h>
#include <string>

// Test constants for int32_t rectangles
constexpr int32_t kIntTestX = 10;
constexpr int32_t kIntTestY = 20;
constexpr int32_t kIntTestWidth = 100;
constexpr int32_t kIntTestHeight = 200;

constexpr int32_t kIntCopyX = 15;
constexpr int32_t kIntCopyY = 25;
constexpr int32_t kIntCopyWidth = 50;
constexpr int32_t kIntCopyHeight = 75;

// Test constants for float rectangles
constexpr float kFloatTestX = 10.5f;
constexpr float kFloatTestY = 20.25f;
constexpr float kFloatTestWidth = 100.75f;
constexpr float kFloatTestHeight = 200.5f;

constexpr float kFloatCopyX = 15.125f;
constexpr float kFloatCopyY = 25.375f;
constexpr float kFloatCopyWidth = 50.5f;
constexpr float kFloatCopyHeight = 75.75f;

// Tests for Rectangle<int32_t>
class IntRectangleTest : public ::testing::Test {
protected:
    using IntRect = sdlpp::Rectangle<int32_t>;
};

TEST_F(IntRectangleTest, ConstructorAndGetters) {
    IntRect rect(kIntTestX, kIntTestY, kIntTestWidth, kIntTestHeight);
    
    EXPECT_EQ(rect.getX(), kIntTestX);
    EXPECT_EQ(rect.getY(), kIntTestY);
    EXPECT_EQ(rect.getWidth(), kIntTestWidth);
    EXPECT_EQ(rect.getHeight(), kIntTestHeight);
}

TEST_F(IntRectangleTest, CopyConstructor) {
    IntRect rect1(kIntCopyX, kIntCopyY, kIntCopyWidth, kIntCopyHeight);
    IntRect rect2(rect1);
    
    EXPECT_EQ(rect2.getX(), kIntCopyX);
    EXPECT_EQ(rect2.getY(), kIntCopyY);
    EXPECT_EQ(rect2.getWidth(), kIntCopyWidth);
    EXPECT_EQ(rect2.getHeight(), kIntCopyHeight);
    
    // Verify independence
    rect1.setX(999);
    EXPECT_EQ(rect2.getX(), kIntCopyX);  // rect2 should be unchanged
}

TEST_F(IntRectangleTest, MoveConstructor) {
    IntRect rect1(kIntTestX, kIntTestY, kIntTestWidth, kIntTestHeight);
    IntRect rect2(std::move(rect1));
    
    EXPECT_EQ(rect2.getX(), kIntTestX);
    EXPECT_EQ(rect2.getY(), kIntTestY);
    EXPECT_EQ(rect2.getWidth(), kIntTestWidth);
    EXPECT_EQ(rect2.getHeight(), kIntTestHeight);
}

TEST_F(IntRectangleTest, CopyAssignment) {
    IntRect rect1(kIntCopyX, kIntCopyY, kIntCopyWidth, kIntCopyHeight);
    IntRect rect2(0, 0, 0, 0);
    
    rect2 = rect1;
    
    EXPECT_EQ(rect2.getX(), kIntCopyX);
    EXPECT_EQ(rect2.getY(), kIntCopyY);
    EXPECT_EQ(rect2.getWidth(), kIntCopyWidth);
    EXPECT_EQ(rect2.getHeight(), kIntCopyHeight);
    
    // Verify independence
    rect1.setY(888);
    EXPECT_EQ(rect2.getY(), kIntCopyY);  // rect2 should be unchanged
}

TEST_F(IntRectangleTest, MoveAssignment) {
    IntRect rect1(kIntTestX, kIntTestY, kIntTestWidth, kIntTestHeight);
    IntRect rect2(0, 0, 0, 0);
    
    rect2 = std::move(rect1);
    
    EXPECT_EQ(rect2.getX(), kIntTestX);
    EXPECT_EQ(rect2.getY(), kIntTestY);
    EXPECT_EQ(rect2.getWidth(), kIntTestWidth);
    EXPECT_EQ(rect2.getHeight(), kIntTestHeight);
}

TEST_F(IntRectangleTest, Setters) {
    IntRect rect(0, 0, 0, 0);
    
    rect.setX(kIntTestX);
    rect.setY(kIntTestY);
    rect.setWidth(kIntTestWidth);
    rect.setHeight(kIntTestHeight);
    
    EXPECT_EQ(rect.getX(), kIntTestX);
    EXPECT_EQ(rect.getY(), kIntTestY);
    EXPECT_EQ(rect.getWidth(), kIntTestWidth);
    EXPECT_EQ(rect.getHeight(), kIntTestHeight);
}

TEST_F(IntRectangleTest, Contains) {
    IntRect rect(10, 20, 50, 40);  // Rectangle from (10,20) to (60,60)
    
    // Test points inside
    EXPECT_TRUE(rect.contains(10, 20));    // Top-left corner (inclusive)
    EXPECT_TRUE(rect.contains(30, 35));    // Inside rectangle
    EXPECT_TRUE(rect.contains(59, 59));    // Near bottom-right (inclusive)
    
    // Test points outside
    EXPECT_FALSE(rect.contains(9, 20));    // Left of rectangle
    EXPECT_FALSE(rect.contains(10, 19));   // Above rectangle
    EXPECT_FALSE(rect.contains(60, 35));   // Right of rectangle (exclusive)
    EXPECT_FALSE(rect.contains(30, 60));   // Below rectangle (exclusive)
    EXPECT_FALSE(rect.contains(60, 60));   // Bottom-right corner (exclusive)
}

TEST_F(IntRectangleTest, ZeroDimensions) {
    IntRect rect(0, 0, 0, 0);
    
    EXPECT_EQ(rect.getX(), 0);
    EXPECT_EQ(rect.getY(), 0);
    EXPECT_EQ(rect.getWidth(), 0);
    EXPECT_EQ(rect.getHeight(), 0);
    
    // Zero-size rectangle should not contain any points
    EXPECT_FALSE(rect.contains(0, 0));
    EXPECT_FALSE(rect.contains(1, 1));
}

TEST_F(IntRectangleTest, NegativeCoordinates) {
    IntRect rect(-10, -5, 20, 15);  // Rectangle from (-10,-5) to (10,10)
    
    EXPECT_EQ(rect.getX(), -10);
    EXPECT_EQ(rect.getY(), -5);
    EXPECT_EQ(rect.getWidth(), 20);
    EXPECT_EQ(rect.getHeight(), 15);
    
    // Test containment with negative coordinates
    EXPECT_TRUE(rect.contains(-10, -5));   // Top-left corner
    EXPECT_TRUE(rect.contains(0, 0));      // Origin inside
    EXPECT_TRUE(rect.contains(9, 9));      // Near bottom-right
    EXPECT_FALSE(rect.contains(-11, 0));   // Left of rectangle
    EXPECT_FALSE(rect.contains(10, 5));    // Right of rectangle (exclusive)
}

// Tests for Rectangle<float>
class FloatRectangleTest : public ::testing::Test {
protected:
    using FloatRect = sdlpp::Rectangle<float>;
};

TEST_F(FloatRectangleTest, ConstructorAndGetters) {
    FloatRect rect(kFloatTestX, kFloatTestY, kFloatTestWidth, kFloatTestHeight);
    
    EXPECT_FLOAT_EQ(rect.getX(), kFloatTestX);
    EXPECT_FLOAT_EQ(rect.getY(), kFloatTestY);
    EXPECT_FLOAT_EQ(rect.getWidth(), kFloatTestWidth);
    EXPECT_FLOAT_EQ(rect.getHeight(), kFloatTestHeight);
}

TEST_F(FloatRectangleTest, CopyConstructor) {
    FloatRect rect1(kFloatCopyX, kFloatCopyY, kFloatCopyWidth, kFloatCopyHeight);
    FloatRect rect2(rect1);
    
    EXPECT_FLOAT_EQ(rect2.getX(), kFloatCopyX);
    EXPECT_FLOAT_EQ(rect2.getY(), kFloatCopyY);
    EXPECT_FLOAT_EQ(rect2.getWidth(), kFloatCopyWidth);
    EXPECT_FLOAT_EQ(rect2.getHeight(), kFloatCopyHeight);
    
    // Verify independence
    rect1.setX(999.5f);
    EXPECT_FLOAT_EQ(rect2.getX(), kFloatCopyX);  // rect2 should be unchanged
}

TEST_F(FloatRectangleTest, MoveConstructor) {
    FloatRect rect1(kFloatTestX, kFloatTestY, kFloatTestWidth, kFloatTestHeight);
    FloatRect rect2(std::move(rect1));
    
    EXPECT_FLOAT_EQ(rect2.getX(), kFloatTestX);
    EXPECT_FLOAT_EQ(rect2.getY(), kFloatTestY);
    EXPECT_FLOAT_EQ(rect2.getWidth(), kFloatTestWidth);
    EXPECT_FLOAT_EQ(rect2.getHeight(), kFloatTestHeight);
}

TEST_F(FloatRectangleTest, CopyAssignment) {
    FloatRect rect1(kFloatCopyX, kFloatCopyY, kFloatCopyWidth, kFloatCopyHeight);
    FloatRect rect2(0.0f, 0.0f, 0.0f, 0.0f);
    
    rect2 = rect1;
    
    EXPECT_FLOAT_EQ(rect2.getX(), kFloatCopyX);
    EXPECT_FLOAT_EQ(rect2.getY(), kFloatCopyY);
    EXPECT_FLOAT_EQ(rect2.getWidth(), kFloatCopyWidth);
    EXPECT_FLOAT_EQ(rect2.getHeight(), kFloatCopyHeight);
    
    // Verify independence
    rect1.setY(888.25f);
    EXPECT_FLOAT_EQ(rect2.getY(), kFloatCopyY);  // rect2 should be unchanged
}

TEST_F(FloatRectangleTest, MoveAssignment) {
    FloatRect rect1(kFloatTestX, kFloatTestY, kFloatTestWidth, kFloatTestHeight);
    FloatRect rect2(0.0f, 0.0f, 0.0f, 0.0f);
    
    rect2 = std::move(rect1);
    
    EXPECT_FLOAT_EQ(rect2.getX(), kFloatTestX);
    EXPECT_FLOAT_EQ(rect2.getY(), kFloatTestY);
    EXPECT_FLOAT_EQ(rect2.getWidth(), kFloatTestWidth);
    EXPECT_FLOAT_EQ(rect2.getHeight(), kFloatTestHeight);
}

TEST_F(FloatRectangleTest, Setters) {
    FloatRect rect(0.0f, 0.0f, 0.0f, 0.0f);
    
    rect.setX(kFloatTestX);
    rect.setY(kFloatTestY);
    rect.setWidth(kFloatTestWidth);
    rect.setHeight(kFloatTestHeight);
    
    EXPECT_FLOAT_EQ(rect.getX(), kFloatTestX);
    EXPECT_FLOAT_EQ(rect.getY(), kFloatTestY);
    EXPECT_FLOAT_EQ(rect.getWidth(), kFloatTestWidth);
    EXPECT_FLOAT_EQ(rect.getHeight(), kFloatTestHeight);
}

TEST_F(FloatRectangleTest, Contains) {
    FloatRect rect(10.5f, 20.25f, 50.0f, 40.0f);  // Sub-pixel positioning
    
    // Test points inside
    EXPECT_TRUE(rect.contains(10.5f, 20.25f));     // Top-left corner (inclusive)
    EXPECT_TRUE(rect.contains(30.75f, 35.125f));   // Inside rectangle
    EXPECT_TRUE(rect.contains(60.49f, 60.24f));    // Near bottom-right
    
    // Test points outside
    EXPECT_FALSE(rect.contains(10.4f, 30.0f));     // Left of rectangle
    EXPECT_FALSE(rect.contains(30.0f, 20.0f));     // Above rectangle
    EXPECT_FALSE(rect.contains(60.5f, 35.0f));     // Right of rectangle (exclusive)
    EXPECT_FALSE(rect.contains(30.0f, 60.25f));    // Below rectangle (exclusive)
}

TEST_F(FloatRectangleTest, SubPixelPrecision) {
    FloatRect rect(0.1f, 0.2f, 1.3f, 1.4f);
    
    EXPECT_FLOAT_EQ(rect.getX(), 0.1f);
    EXPECT_FLOAT_EQ(rect.getY(), 0.2f);
    EXPECT_FLOAT_EQ(rect.getWidth(), 1.3f);
    EXPECT_FLOAT_EQ(rect.getHeight(), 1.4f);
    
    // Test sub-pixel containment
    EXPECT_TRUE(rect.contains(0.1f, 0.2f));        // Top-left corner
    EXPECT_TRUE(rect.contains(0.7f, 0.8f));        // Inside
    EXPECT_FALSE(rect.contains(1.4f, 1.6f));       // Outside (exclusive boundary)
}

TEST_F(FloatRectangleTest, ZeroDimensions) {
    FloatRect rect(0.0f, 0.0f, 0.0f, 0.0f);
    
    EXPECT_FLOAT_EQ(rect.getX(), 0.0f);
    EXPECT_FLOAT_EQ(rect.getY(), 0.0f);
    EXPECT_FLOAT_EQ(rect.getWidth(), 0.0f);
    EXPECT_FLOAT_EQ(rect.getHeight(), 0.0f);
    
    // Zero-size rectangle should not contain any points
    EXPECT_FALSE(rect.contains(0.0f, 0.0f));
    EXPECT_FALSE(rect.contains(0.1f, 0.1f));
}

// Test type aliases
TEST(RectangleAliasTest, TypeAliases) {
    static_assert(std::is_same_v<sdlpp::IntRectangle, sdlpp::Rectangle<int32_t>>);
    static_assert(std::is_same_v<sdlpp::FloatRectangle, sdlpp::Rectangle<float>>);
    
    sdlpp::IntRectangle intRect(1, 2, 3, 4);
    sdlpp::FloatRectangle floatRect(1.5f, 2.5f, 3.5f, 4.5f);
    
    EXPECT_EQ(intRect.getX(), 1);
    EXPECT_FLOAT_EQ(floatRect.getX(), 1.5f);
}

// Test RectangleCoordinate concept
TEST(RectangleConceptTest, ValidTypes) {
    // Test that the concept correctly identifies valid coordinate types
    static_assert(sdlpp::RectangleCoordinate<int32_t>);
    static_assert(sdlpp::RectangleCoordinate<float>);
    
    // Test that these types can be used to create rectangles
    sdlpp::Rectangle<int32_t> intRect(10, 20, 100, 200);
    sdlpp::Rectangle<float> floatRect(10.5f, 20.5f, 100.5f, 200.5f);
    
    EXPECT_EQ(intRect.getX(), 10);
    EXPECT_EQ(intRect.getY(), 20);
    EXPECT_FLOAT_EQ(floatRect.getX(), 10.5f);
    EXPECT_FLOAT_EQ(floatRect.getY(), 20.5f);
}

TEST(RectangleConceptTest, InvalidTypes) {
    // Test that the concept correctly rejects invalid coordinate types
    static_assert(!sdlpp::RectangleCoordinate<uint32_t>);
    static_assert(!sdlpp::RectangleCoordinate<double>);
    static_assert(!sdlpp::RectangleCoordinate<int16_t>);
    static_assert(!sdlpp::RectangleCoordinate<uint16_t>);
    static_assert(!sdlpp::RectangleCoordinate<int8_t>);
    static_assert(!sdlpp::RectangleCoordinate<uint8_t>);
    static_assert(!sdlpp::RectangleCoordinate<char>);
    static_assert(!sdlpp::RectangleCoordinate<std::string>);
    static_assert(!sdlpp::RectangleCoordinate<void*>);
}

TEST(RectangleConceptTest, ConceptRequirements) {
    // Verify that the concept matches exactly the expected types
    static_assert(std::same_as<int32_t, int32_t>);
    static_assert(std::same_as<float, float>);
    static_assert(!std::same_as<uint32_t, int32_t>);
    static_assert(!std::same_as<double, float>);
    
    // Test the concept definition logic directly
    static_assert(std::same_as<int32_t, int32_t> || std::same_as<int32_t, float>);
    static_assert(std::same_as<float, int32_t> || std::same_as<float, float>);
    static_assert(!(std::same_as<uint32_t, int32_t> || std::same_as<uint32_t, float>));
    static_assert(!(std::same_as<double, int32_t> || std::same_as<double, float>));
}