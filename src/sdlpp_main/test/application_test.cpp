#include <gtest/gtest.h>
#include <application.h>

using namespace sdlpp;

class TestApplication : public Application<TestApplication> {
  friend Application<TestApplication>;

 private:
  auto init() -> bool {
    return true;
  }

  auto iterate() -> bool {
    return false; // Exit immediately for testing
  }

  auto quit() -> void {
    // Test cleanup
  }
};

TEST(ApplicationTest, CompilationTest) {
  // Just test that the template compiles correctly
  EXPECT_TRUE(true);
}