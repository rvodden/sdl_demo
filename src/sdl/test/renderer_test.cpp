#include <gtest/gtest.h>
#include <sdl.h>
#include <window.h>
#include <renderer.h>
#include <color.h>

constexpr uint16_t kTestWidth = 800;
constexpr uint16_t kTestHeight = 600;
constexpr uint8_t kMaxColorValue = 255;
constexpr uint8_t kHalfColorValue = 128;

class RendererTest : public ::testing::Test {
protected:
  void SetUp() override {
    // Create SDL context and window for renderer tests
    window = std::make_unique<sdl::Window>("Test Window", kTestWidth, kTestHeight, 0);
  }

  void TearDown() override {
    window.reset();
  }

  sdl::SDL sdl;
  std::unique_ptr<sdl::Window> window;
};

TEST_F(RendererTest, testRendererCreation) {
  sdl::Renderer renderer(*window);
  // Test that renderer can be created successfully
  // Since we can't easily test the underlying SDL renderer state,
  // the fact that construction doesn't throw is sufficient
}

TEST_F(RendererTest, testRendererMoveConstructor) {
  sdl::Renderer renderer1(*window);
  sdl::Renderer renderer2 = std::move(renderer1);
  
  // Test that the moved-to renderer can still be used
  sdl::Color testColor(kMaxColorValue, 0, 0, kMaxColorValue);
  ASSERT_NO_THROW(renderer2.setDrawColour(testColor));
  ASSERT_NO_THROW(renderer2.clear());
  ASSERT_NO_THROW(renderer2.present());
}

TEST_F(RendererTest, testRendererMoveAssignment) {
  // Create two separate windows since SDL doesn't allow multiple renderers per window
  auto window2 = std::make_unique<sdl::Window>("Test Window 2", kTestWidth, kTestHeight, 0);
  
  sdl::Renderer renderer1(*window);
  sdl::Renderer renderer2(*window2);
  renderer2 = std::move(renderer1);
  
  // Test that the moved-to renderer can still be used
  sdl::Color testColor(0, kMaxColorValue, 0, kMaxColorValue);
  ASSERT_NO_THROW(renderer2.setDrawColour(testColor));
  ASSERT_NO_THROW(renderer2.clear());
  ASSERT_NO_THROW(renderer2.present());
}

TEST_F(RendererTest, testSetRenderDrawColour) {
  sdl::Renderer renderer(*window);
  
  // Test setting various colors
  sdl::Color red(kMaxColorValue, 0, 0, kMaxColorValue);
  sdl::Color green(0, kMaxColorValue, 0, kMaxColorValue);
  sdl::Color blue(0, 0, kMaxColorValue, kMaxColorValue);
  
  // These should not throw
  renderer.setDrawColour(red);
  renderer.setDrawColour(green);
  renderer.setDrawColour(blue);
}

TEST_F(RendererTest, testClearAndPresent) {
  sdl::Renderer renderer(*window);
  
  // Test clear and present operations
  renderer.clear();
  renderer.present();
  
  // Test with different colors
  sdl::Color testColor(kHalfColorValue, kHalfColorValue, kHalfColorValue, kMaxColorValue);
  renderer.setDrawColour(testColor);
  renderer.clear();
  renderer.present();
}
