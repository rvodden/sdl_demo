#include <gtest/gtest.h>
#include <sdl.h>
#include <window.h>

const uint8_t kWindowDimension = 100;

TEST(WindowTest, testMoveConstructor) {
  sdl::SDL sdl;
  sdl::Window windowOne{"This is a title", kWindowDimension, kWindowDimension, 0};
  
  // Verify the window works before move
  ASSERT_EQ(windowOne.getTitle(), "This is a title");
  
  // Move construct new window
  sdl::Window windowTwo{std::move(windowOne)};

  // Test that the moved-to window retains the title
  ASSERT_EQ(windowTwo.getTitle(), "This is a title");
  
  // Test that we can still use the moved-to window
  ASSERT_NO_THROW(windowTwo.setTitle("New Title After Move"));
  ASSERT_EQ(windowTwo.getTitle(), "New Title After Move");
}

TEST(WindowTest, testMoveAssignment) {
  sdl::SDL sdl;
  sdl::Window windowOne{"Original Title", kWindowDimension, kWindowDimension, 0};
  sdl::Window windowTwo{"New Title", kWindowDimension, kWindowDimension, 0};
  
  // Verify both windows work before move
  ASSERT_EQ(windowOne.getTitle(), "Original Title");
  ASSERT_EQ(windowTwo.getTitle(), "New Title");
  
  // Move assign
  windowTwo = std::move(windowOne);
  
  // Test that the moved-to window has the correct title
  ASSERT_EQ(windowTwo.getTitle(), "Original Title");
  
  // Test that we can still use the moved-to window
  ASSERT_NO_THROW(windowTwo.setTitle("Modified Title"));
  ASSERT_EQ(windowTwo.getTitle(), "Modified Title");
}

TEST(WindowTest, testTitleGetterSetter) {
  sdl::SDL sdl;
  sdl::Window window{"Initial Title", kWindowDimension, kWindowDimension, 0};
  
  ASSERT_EQ(window.getTitle(), "Initial Title");
  
  window.setTitle("Updated Title");
  ASSERT_EQ(window.getTitle(), "Updated Title");
}

TEST(WindowTest, testWindowCreation) {
  sdl::SDL sdl;
  
  // Test different window configurations
  sdl::Window window1{"Test Window 1", 640, 480, 0};
  ASSERT_EQ(window1.getTitle(), "Test Window 1");
  
  sdl::Window window2{"Test Window 2", 1024, 768, 0};
  ASSERT_EQ(window2.getTitle(), "Test Window 2");
}
