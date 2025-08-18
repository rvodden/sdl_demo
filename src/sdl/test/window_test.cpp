#include <gtest/gtest.h>
#include <sdl.h>
#include <window.h>

const uint8_t kWindowDimension = 100;

TEST(WindowTest, testMoveConstructor) {
  sdl::SDL sdl;
  sdl::Window windowOne{"This is a title", kWindowDimension, kWindowDimension, 0};
  sdl::Window windowTwo{std::move(windowOne)};

  ASSERT_EQ(windowTwo.getTitle(), "This is a title");
}
