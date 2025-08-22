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
    window = std::make_unique<sdlpp::Window>("Test Window", kTestWidth, kTestHeight, 0);
  }

  void TearDown() override {
    window.reset();
  }

  sdlpp::SDL sdl;
  std::unique_ptr<sdlpp::Window> window;
};

TEST_F(RendererTest, testRendererCreation) {
  sdlpp::Renderer renderer(*window);
  // Test that renderer can be created successfully
  // Since we can't easily test the underlying SDL renderer state,
  // the fact that construction doesn't throw is sufficient
}

TEST_F(RendererTest, testRendererMoveConstructor) {
  sdlpp::Renderer renderer1(*window);
  sdlpp::Renderer renderer2 = std::move(renderer1);
  
  // Test that the moved-to renderer can still be used
  sdlpp::Color testColor(kMaxColorValue, 0, 0, kMaxColorValue);
  ASSERT_NO_THROW(renderer2.setRenderDrawColour(testColor));
  ASSERT_NO_THROW(renderer2.clear());
  ASSERT_NO_THROW(renderer2.present());
}

TEST_F(RendererTest, testRendererMoveAssignment) {
  // Create two separate windows since SDL doesn't allow multiple renderers per window
  auto window2 = std::make_unique<sdlpp::Window>("Test Window 2", kTestWidth, kTestHeight, 0);
  
  sdlpp::Renderer renderer1(*window);
  sdlpp::Renderer renderer2(*window2);
  renderer2 = std::move(renderer1);
  
  // Test that the moved-to renderer can still be used
  sdlpp::Color testColor(0, kMaxColorValue, 0, kMaxColorValue);
  ASSERT_NO_THROW(renderer2.setRenderDrawColour(testColor));
  ASSERT_NO_THROW(renderer2.clear());
  ASSERT_NO_THROW(renderer2.present());
}

TEST_F(RendererTest, testSetRenderDrawColour) {
  sdlpp::Renderer renderer(*window);
  
  // Test setting various colors
  sdlpp::Color red(kMaxColorValue, 0, 0, kMaxColorValue);
  sdlpp::Color green(0, kMaxColorValue, 0, kMaxColorValue);
  sdlpp::Color blue(0, 0, kMaxColorValue, kMaxColorValue);
  
  // These should not throw
  renderer.setRenderDrawColour(red);
  renderer.setRenderDrawColour(green);
  renderer.setRenderDrawColour(blue);
}

TEST_F(RendererTest, testClearAndPresent) {
  sdlpp::Renderer renderer(*window);
  
  // Test clear and present operations
  renderer.clear();
  renderer.present();
  
  // Test with different colors
  sdlpp::Color testColor(kHalfColorValue, kHalfColorValue, kHalfColorValue, kMaxColorValue);
  renderer.setRenderDrawColour(testColor);
  renderer.clear();
  renderer.present();
}
