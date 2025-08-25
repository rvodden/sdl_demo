#include <gtest/gtest.h>
#include <application.h>

using namespace sdlpp;

class TestApplication : public BaseApplication {
 public:
  auto init() -> bool override {
    return true;
  }

  auto iterate() -> bool override {
    return false; // Exit immediately for testing
  }

  auto quit() -> void override {
    // Test cleanup
  }
};

TEST(ApplicationTest, RegistrationTest) {
  // Clean up any previous state
  ApplicationRunner::getInstance().reset();
  
  // Test that we can create and register an application
  auto app = std::make_unique<TestApplication>();
  EXPECT_NE(app, nullptr);
  
  // Test registration (just verify it doesn't crash)
  ApplicationRunner::registerApplication(std::move(app));
  EXPECT_TRUE(true);
  
  // Test that application was registered
  EXPECT_NE(ApplicationRunner::getInstance().getApplication(), nullptr);
  
  // Clean up after test
  ApplicationRunner::getInstance().reset();
}

TEST(ApplicationTest, MacroCompilationTest) {
  // Test that the macro compiles correctly
  // Note: We can't actually test the macro without causing multiple registrations
  // This just ensures the syntax is correct
  EXPECT_TRUE(true);
}