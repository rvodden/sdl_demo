#include <algorithm>
#include <filesystem>
#include <vector>

#include <gtest/gtest.h>

#include <sdl/color.h>
#include <sdl/exception.h>
#include <sdl/surface.h>

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
  
  sdl::Surface surface(width, height);
  
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

  sdl::Surface surface(width, height, pixels);

  ASSERT_EQ(surface.getWidth(), width);
  ASSERT_EQ(surface.getHeight(), height);

  // Verify pixel data was set correctly using PixelAccess
  auto retrievedPixels = surface.getPixels();
  ASSERT_EQ(retrievedPixels.size(), pixels.size());
  ASSERT_TRUE(std::equal(retrievedPixels.begin(), retrievedPixels.end(), pixels.begin()));
}

TEST_F(SurfaceTest, testCreateSurfaceFromInvalidPixelData) {
  constexpr uint32_t width = 2;
  constexpr uint32_t height = 2;
  
  // Create invalid pixel data (wrong size)
  std::vector<uint8_t> pixels = {255, 0, 0, 255}; // Only 1 pixel instead of 4
  
  ASSERT_THROW(sdl::Surface surface(width, height, pixels), sdl::Exception);
}

TEST_F(SurfaceTest, testFillSurface) {
  constexpr uint32_t width = 10;
  constexpr uint32_t height = 10;
  
  sdl::Surface surface(width, height);
  sdl::Color fillColor(255, 128, 64, 255);
  
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
  
  sdl::Surface surface(width, height);
  sdl::Color testColor(200, 100, 50, 128);
  
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
  
  sdl::Surface surface(width, height);
  sdl::Color testColor(255, 255, 255, 255);
  
  // Test setting pixel out of bounds
  ASSERT_THROW(surface.setPixel(width, height, testColor), sdl::Exception);
  ASSERT_THROW(surface.setPixel(width + 1, 0, testColor), sdl::Exception);
  ASSERT_THROW(surface.setPixel(0, height + 1, testColor), sdl::Exception);
  
  // Test getting pixel out of bounds
  ASSERT_THROW((void)surface.getPixel(width, height), sdl::Exception);
  ASSERT_THROW((void)surface.getPixel(width + 1, 0), sdl::Exception);
  ASSERT_THROW((void)surface.getPixel(0, height + 1), sdl::Exception);
}

TEST_F(SurfaceTest, testFillRect) {
  constexpr uint32_t width = 20;
  constexpr uint32_t height = 20;
  
  sdl::Surface surface(width, height);
  
  // Fill entire surface with black first
  surface.fill(sdl::NamedColor::kBlack);
  
  // Fill a rectangle with white
  sdl::Color fillColor = sdl::NamedColor::kWhite;
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
  sdl::Surface source(width, height);
  source.fill(sdl::Color(255, 0, 0, 255));
  
  // Create destination surface and fill with blue
  sdl::Surface dest(width * 2, height * 2);
  dest.fill(sdl::Color(0, 0, 255, 255));
  
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
  sdl::Surface source(width, height);
  source.fillRect(0, 0, 5, 5, sdl::Color(255, 0, 0, 255));   // Red top-left
  source.fillRect(5, 0, 5, 5, sdl::Color(0, 255, 0, 255));   // Green top-right
  source.fillRect(0, 5, 5, 5, sdl::Color(0, 0, 255, 255));   // Blue bottom-left
  source.fillRect(5, 5, 5, 5, sdl::Color(255, 255, 0, 255)); // Yellow bottom-right
  
  // Create destination surface
  sdl::Surface dest(width, height);
  dest.fill(sdl::NamedColor::kBlack);
  
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
  
  sdl::Surface surface(width, height);
  surface.fill(sdl::Color(255, 128, 64, 255));
  
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
  
  sdl::Surface original(width, height);
  original.fill(sdl::Color(100, 150, 200, 255));
  
  // Move construct
  sdl::Surface moved(std::move(original));
  
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
  
  sdl::Surface original(width, height);
  original.fill(sdl::Color(75, 125, 175, 255));
  
  sdl::Surface target(1, 1); // Create a different surface
  
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

// =============================================================================
// PixelAccess Tests
// =============================================================================

TEST_F(SurfaceTest, testPixelAccessSize) {
  constexpr uint32_t width = 10;
  constexpr uint32_t height = 20;

  sdl::Surface surface(width, height);
  auto pixels = surface.getPixels();

  ASSERT_EQ(pixels.size(), width * height * 4);
  ASSERT_FALSE(pixels.empty());
}

TEST_F(SurfaceTest, testPixelAccessData) {
  constexpr uint32_t width = 2;
  constexpr uint32_t height = 2;

  std::vector<uint8_t> initialData = {
    255, 0, 0, 255,    // Red
    0, 255, 0, 255,    // Green
    0, 0, 255, 255,    // Blue
    255, 255, 0, 255   // Yellow
  };

  sdl::Surface surface(width, height, initialData);
  auto pixels = surface.getPixels();

  ASSERT_NE(pixels.data(), nullptr);
  ASSERT_EQ(pixels[0], 255);  // Red component of first pixel
  ASSERT_EQ(pixels[4], 0);    // Red component of second pixel (green)
}

TEST_F(SurfaceTest, testPixelAccessSpan) {
  constexpr uint32_t width = 4;
  constexpr uint32_t height = 4;

  sdl::Surface surface(width, height);
  auto pixels = surface.getPixels();

  auto span = pixels.span();
  ASSERT_EQ(span.size(), width * height * 4);
  ASSERT_EQ(span.data(), pixels.data());
}

TEST_F(SurfaceTest, testPixelAccessModification) {
  constexpr uint32_t width = 2;
  constexpr uint32_t height = 2;

  sdl::Surface surface(width, height);
  surface.fill(sdl::NamedColor::kBlack);

  // Modify pixels through PixelAccess
  // Note: SDL_PIXELFORMAT_RGBA8888 on little-endian stores bytes as ABGR in memory
  // The 32-bit packed value is (R << 24) | (G << 16) | (B << 8) | A
  // So in memory (little-endian): byte[0]=A, byte[1]=B, byte[2]=G, byte[3]=R
  {
    auto pixels = surface.getPixels();
    pixels[0] = 255;  // Alpha
    pixels[1] = 64;   // Blue
    pixels[2] = 128;  // Green
    pixels[3] = 200;  // Red
  }

  // Verify the modification persisted
  auto color = surface.getPixel(0, 0);
  ASSERT_EQ(color.getRed(), 200);
  ASSERT_EQ(color.getGreen(), 128);
  ASSERT_EQ(color.getBlue(), 64);
  ASSERT_EQ(color.getAlpha(), 255);
}

TEST_F(SurfaceTest, testPixelAccessIterators) {
  constexpr uint32_t width = 4;
  constexpr uint32_t height = 4;

  sdl::Surface surface(width, height);
  auto pixels = surface.getPixels();

  // Fill using iterators
  std::fill(pixels.begin(), pixels.end(), static_cast<uint8_t>(128));

  // Verify all bytes are set
  for (size_t i = 0; i < pixels.size(); ++i) {
    ASSERT_EQ(pixels[i], 128);
  }
}

TEST_F(SurfaceTest, testPixelAccessRangeBasedFor) {
  constexpr uint32_t width = 2;
  constexpr uint32_t height = 2;

  sdl::Surface surface(width, height);
  auto pixels = surface.getPixels();

  // Set all bytes to 200 using range-based for
  for (auto& byte : pixels.span()) {
    byte = 200;
  }

  // Verify
  for (size_t i = 0; i < pixels.size(); ++i) {
    ASSERT_EQ(pixels[i], 200);
  }
}

TEST_F(SurfaceTest, testPixelAccessMoveOnly) {
  constexpr uint32_t width = 4;
  constexpr uint32_t height = 4;

  sdl::Surface surface(width, height);
  auto pixels1 = surface.getPixels();

  // Move construct
  auto pixels2 = std::move(pixels1);
  ASSERT_EQ(pixels2.size(), width * height * 4);

  // Move assign
  sdl::Surface surface2(2, 2);
  auto pixels3 = surface2.getPixels();
  pixels3 = std::move(pixels2);
  ASSERT_EQ(pixels3.size(), width * height * 4);
}

TEST_F(SurfaceTest, testConstPixelAccessReadOnly) {
  constexpr uint32_t width = 2;
  constexpr uint32_t height = 2;

  std::vector<uint8_t> initialData = {
    100, 150, 200, 255,
    50, 75, 100, 128,
    25, 50, 75, 64,
    10, 20, 30, 32
  };

  const sdl::Surface surface(width, height, initialData);
  auto pixels = surface.getPixels();  // Returns ConstPixelAccess

  ASSERT_EQ(pixels.size(), width * height * 4);
  ASSERT_EQ(pixels[0], 100);
  ASSERT_EQ(pixels[4], 50);

  // Verify const span
  auto span = pixels.span();
  ASSERT_EQ(span.size(), width * height * 4);
}

TEST_F(SurfaceTest, testPixelAccessDirectMemoryAccess) {
  constexpr uint32_t width = 8;
  constexpr uint32_t height = 8;

  sdl::Surface surface(width, height);
  surface.fill(sdl::Color(10, 20, 30, 40));

  auto pixels = surface.getPixels();
  auto* data = pixels.data();

  // Direct memory access - verify byte layout
  // SDL_PIXELFORMAT_RGBA8888 on little-endian stores bytes as ABGR in memory
  // The 32-bit packed value is (R << 24) | (G << 16) | (B << 8) | A
  // So in memory (little-endian): byte[0]=A, byte[1]=B, byte[2]=G, byte[3]=R
  ASSERT_EQ(data[0], 40);  // A
  ASSERT_EQ(data[1], 30);  // B
  ASSERT_EQ(data[2], 20);  // G
  ASSERT_EQ(data[3], 10);  // R
}

TEST_F(SurfaceTest, testPixelAccessZeroCopyVerification) {
  constexpr uint32_t width = 4;
  constexpr uint32_t height = 4;

  sdl::Surface surface(width, height);

  // Get two PixelAccess objects - they should point to the same memory
  auto pixels1 = surface.getPixels();
  auto pixels2 = surface.getPixels();

  // Modify through first access
  pixels1[0] = 99;

  // Change should be visible through second access (same underlying memory)
  ASSERT_EQ(pixels2[0], 99);
}

TEST_F(SurfaceTest, testPixelAccessStdAlgorithms) {
  constexpr uint32_t width = 4;
  constexpr uint32_t height = 4;

  sdl::Surface surface(width, height);
  auto pixels = surface.getPixels();

  // Use std::fill
  std::fill(pixels.begin(), pixels.end(), static_cast<uint8_t>(50));

  // Use std::count
  auto count = std::count(pixels.begin(), pixels.end(), static_cast<uint8_t>(50));
  ASSERT_EQ(count, static_cast<ptrdiff_t>(width * height * 4));

  // Use std::transform
  std::transform(pixels.begin(), pixels.end(), pixels.begin(),
                 [](uint8_t b) { return static_cast<uint8_t>(b * 2); });

  ASSERT_EQ(pixels[0], 100);
}
