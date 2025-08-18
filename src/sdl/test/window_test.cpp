#include <gtest/gtest.h>
#include <sdl.h>
#include <window.h>

TEST(WindowTest, testMoveConstructor) {
  sdl::SDL sdl;
  sdl::Window windowOne{"This is a title", 100, 100, 0};
  sdl::Window windowTwo{std::move(windowOne)};

  ASSERT_EQ(windowTwo.getTitle(), "This is a title");
}
