#include <gtest/gtest.h>
#include <surface.h>
#include <color.h>
#include <exception.h>

#include <vector>
#include <filesystem>

class SurfaceTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Create a test image file if needed
    testImagePath = std::filesystem::temp_directory_path() / "test_surface.bmp";
  }

  void TearDown() override {
    // Clean up test files
    if (std::filesystem::exists(testImagePath)) {
      std::filesystem::remove(testImagePath);
    }
  }

  std::filesystem::path testImagePath;
};

TEST_F(SurfaceTest, testCreateSurfaceWithDimensions) {
  constexpr uint32_t width = 100;
  constexpr uint32_t height = 50;
  
  sdlpp::Surface surface(width, height);
  
  ASSERT_EQ(surface.getWidth(), width);
  ASSERT_EQ(surface.getHeight(), height);
}

TEST_F(SurfaceTest, testCreateSurfaceFromPixelData) {
  constexpr uint32_t width = 2;
  constexpr uint32_t height = 2;
  
  // Create 2x2 RGBA pixel data (4 pixels * 4 bytes per pixel = 16 bytes)
  std::vector<uint8_t> pixels = {
    255, 0, 0, 255,    // Red pixel
    0, 255, 0, 255,    // Green pixel
    0, 0, 255, 255,    // Blue pixel
    255, 255, 255, 255 // White pixel
  };
  
  sdlpp::Surface surface(width, height, pixels);
  
  ASSERT_EQ(surface.getWidth(), width);
  ASSERT_EQ(surface.getHeight(), height);
  
  // Verify pixel data was set correctly
  auto retrievedPixels = surface.getPixels();
  ASSERT_EQ(retrievedPixels.size(), pixels.size());
  ASSERT_EQ(retrievedPixels, pixels);
}

TEST_F(SurfaceTest, testCreateSurfaceFromInvalidPixelData) {
  constexpr uint32_t width = 2;
  constexpr uint32_t height = 2;
  
  // Create invalid pixel data (wrong size)
  std::vector<uint8_t> pixels = {255, 0, 0, 255}; // Only 1 pixel instead of 4
  
  ASSERT_THROW(sdlpp::Surface surface(width, height, pixels), sdlpp::Exception);
}

TEST_F(SurfaceTest, testFillSurface) {
  constexpr uint32_t width = 10;
  constexpr uint32_t height = 10;
  
  sdlpp::Surface surface(width, height);
  sdlpp::Color fillColor(255, 128, 64, 255);
  
  surface.fill(fillColor);
  
  // Check a few pixels to verify the fill worked
  auto pixel1 = surface.getPixel(0, 0);
  auto pixel2 = surface.getPixel(5, 5);
  auto pixel3 = surface.getPixel(9, 9);
  
  ASSERT_EQ(pixel1.getRed(), fillColor.getRed());
  ASSERT_EQ(pixel1.getGreen(), fillColor.getGreen());
  ASSERT_EQ(pixel1.getBlue(), fillColor.getBlue());
  ASSERT_EQ(pixel1.getAlpha(), fillColor.getAlpha());
  
  ASSERT_EQ(pixel2.getRed(), fillColor.getRed());
  ASSERT_EQ(pixel3.getRed(), fillColor.getRed());
}

TEST_F(SurfaceTest, testSetAndGetPixel) {
  constexpr uint32_t width = 10;
  constexpr uint32_t height = 10;
  
  sdlpp::Surface surface(width, height);
  sdlpp::Color testColor(200, 100, 50, 128);
  
  surface.setPixel(5, 3, testColor);
  auto retrievedColor = surface.getPixel(5, 3);
  
  ASSERT_EQ(retrievedColor.getRed(), testColor.getRed());
  ASSERT_EQ(retrievedColor.getGreen(), testColor.getGreen());
  ASSERT_EQ(retrievedColor.getBlue(), testColor.getBlue());
  ASSERT_EQ(retrievedColor.getAlpha(), testColor.getAlpha());
}

TEST_F(SurfaceTest, testPixelOutOfBounds) {
  constexpr uint32_t width = 10;
  constexpr uint32_t height = 10;
  
  sdlpp::Surface surface(width, height);
  sdlpp::Color testColor(255, 255, 255, 255);
  
  // Test setting pixel out of bounds
  ASSERT_THROW(surface.setPixel(width, height, testColor), sdlpp::Exception);
  ASSERT_THROW(surface.setPixel(width + 1, 0, testColor), sdlpp::Exception);
  ASSERT_THROW(surface.setPixel(0, height + 1, testColor), sdlpp::Exception);
  
  // Test getting pixel out of bounds
  ASSERT_THROW((void)surface.getPixel(width, height), sdlpp::Exception);
  ASSERT_THROW((void)surface.getPixel(width + 1, 0), sdlpp::Exception);
  ASSERT_THROW((void)surface.getPixel(0, height + 1), sdlpp::Exception);
}

TEST_F(SurfaceTest, testFillRect) {
  constexpr uint32_t width = 20;
  constexpr uint32_t height = 20;
  
  sdlpp::Surface surface(width, height);
  
  // Fill entire surface with black first
  surface.fill(sdlpp::NamedColor::kBlack);
  
  // Fill a rectangle with white
  sdlpp::Color fillColor = sdlpp::NamedColor::kWhite;
  surface.fillRect(5, 5, 10, 10, fillColor);
  
  // Check pixels inside the rectangle
  auto insidePixel = surface.getPixel(10, 10);
  ASSERT_EQ(insidePixel.getRed(), fillColor.getRed());
  ASSERT_EQ(insidePixel.getGreen(), fillColor.getGreen());
  ASSERT_EQ(insidePixel.getBlue(), fillColor.getBlue());
  
  // Check pixels outside the rectangle (should still be black)
  auto outsidePixel = surface.getPixel(2, 2);
  ASSERT_EQ(outsidePixel.getRed(), 0);
  ASSERT_EQ(outsidePixel.getGreen(), 0);
  ASSERT_EQ(outsidePixel.getBlue(), 0);
}

TEST_F(SurfaceTest, testBlit) {
  constexpr uint32_t width = 10;
  constexpr uint32_t height = 10;
  
  // Create source surface and fill with red
  sdlpp::Surface source(width, height);
  source.fill(sdlpp::Color(255, 0, 0, 255));
  
  // Create destination surface and fill with blue
  sdlpp::Surface dest(width * 2, height * 2);
  dest.fill(sdlpp::Color(0, 0, 255, 255));
  
  // Blit source to destination at position (5, 5)
  dest.blit(source, 5, 5);
  
  // Check that the blitted area has red pixels
  auto blittedPixel = dest.getPixel(7, 7);
  ASSERT_EQ(blittedPixel.getRed(), 255);
  ASSERT_EQ(blittedPixel.getGreen(), 0);
  ASSERT_EQ(blittedPixel.getBlue(), 0);
  
  // Check that area outside blit is still blue
  auto outsidePixel = dest.getPixel(2, 2);
  ASSERT_EQ(outsidePixel.getRed(), 0);
  ASSERT_EQ(outsidePixel.getGreen(), 0);
  ASSERT_EQ(outsidePixel.getBlue(), 255);
}

TEST_F(SurfaceTest, testBlitRegion) {
  constexpr uint32_t width = 10;
  constexpr uint32_t height = 10;
  
  // Create source surface with different colored quadrants
  sdlpp::Surface source(width, height);
  source.fillRect(0, 0, 5, 5, sdlpp::Color(255, 0, 0, 255));   // Red top-left
  source.fillRect(5, 0, 5, 5, sdlpp::Color(0, 255, 0, 255));   // Green top-right
  source.fillRect(0, 5, 5, 5, sdlpp::Color(0, 0, 255, 255));   // Blue bottom-left
  source.fillRect(5, 5, 5, 5, sdlpp::Color(255, 255, 0, 255)); // Yellow bottom-right
  
  // Create destination surface
  sdlpp::Surface dest(width, height);
  dest.fill(sdlpp::NamedColor::kBlack);
  
  // Blit only the top-left red quadrant to center of destination
  dest.blit(source, 0, 0, 5, 5, 2, 2);
  
  // Check that the blitted area has red pixels
  auto blittedPixel = dest.getPixel(4, 4);
  ASSERT_EQ(blittedPixel.getRed(), 255);
  ASSERT_EQ(blittedPixel.getGreen(), 0);
  ASSERT_EQ(blittedPixel.getBlue(), 0);
  
  // Check that area outside blit is still black
  auto outsidePixel = dest.getPixel(0, 0);
  ASSERT_EQ(outsidePixel.getRed(), 0);
  ASSERT_EQ(outsidePixel.getGreen(), 0);
  ASSERT_EQ(outsidePixel.getBlue(), 0);
}

TEST_F(SurfaceTest, testSaveBMP) {
  constexpr uint32_t width = 4;
  constexpr uint32_t height = 4;
  
  sdlpp::Surface surface(width, height);
  surface.fill(sdlpp::Color(255, 128, 64, 255));
  
  auto bmpPath = std::filesystem::temp_directory_path() / "test_save.bmp";
  
  // Save should not throw
  ASSERT_NO_THROW(surface.save(bmpPath));
  
  // File should exist
  ASSERT_TRUE(std::filesystem::exists(bmpPath));
  
  // Clean up
  std::filesystem::remove(bmpPath);
}

TEST_F(SurfaceTest, testMoveConstructor) {
  constexpr uint32_t width = 10;
  constexpr uint32_t height = 15;
  
  sdlpp::Surface original(width, height);
  original.fill(sdlpp::Color(100, 150, 200, 255));
  
  // Move construct
  sdlpp::Surface moved(std::move(original));
  
  // Moved surface should have the original properties
  ASSERT_EQ(moved.getWidth(), width);
  ASSERT_EQ(moved.getHeight(), height);
  
  // Check that the pixel data was moved correctly
  auto pixel = moved.getPixel(5, 7);
  ASSERT_EQ(pixel.getRed(), 100);
  ASSERT_EQ(pixel.getGreen(), 150);
  ASSERT_EQ(pixel.getBlue(), 200);
}

TEST_F(SurfaceTest, testMoveAssignment) {
  constexpr uint32_t width = 8;
  constexpr uint32_t height = 12;
  
  sdlpp::Surface original(width, height);
  original.fill(sdlpp::Color(75, 125, 175, 255));
  
  sdlpp::Surface target(1, 1); // Create a different surface
  
  // Move assign
  target = std::move(original);
  
  // Target should now have the original properties
  ASSERT_EQ(target.getWidth(), width);
  ASSERT_EQ(target.getHeight(), height);
  
  // Check that the pixel data was moved correctly
  auto pixel = target.getPixel(4, 6);
  ASSERT_EQ(pixel.getRed(), 75);
  ASSERT_EQ(pixel.getGreen(), 125);
  ASSERT_EQ(pixel.getBlue(), 175);
}