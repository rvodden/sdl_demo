#include <gtest/gtest.h>
#include <ttf.h>
#include <ttf_service.h>
#include <application.h>

using namespace sdlpp;
using namespace sdlpp::ttf;

// Test application that uses TTF service via the registry pattern
class TTFServiceTestApp : public BaseApplication {
public:
  TTFServiceTestApp() : ttfRequested_(false) {}
  
  auto init() -> bool override {
    try {
      // Request TTF service - should work due to ttf_service.h inclusion
      auto& ttf = requestService<TTF>();
      ttfRequested_ = true;
      static_cast<void>(ttf);  // Suppress unused variable warning
      
      return true;
    } catch (const std::exception& e) {
      errorMessage_ = e.what();
      return false;
    }
  }
  
  auto iterate() -> bool override { return false; }
  auto quit() -> void override {}
  
  auto wasTTFRequested() const -> bool { return ttfRequested_; }
  auto getErrorMessage() const -> std::string { return errorMessage_; }
  
private:
  bool ttfRequested_;
  std::string errorMessage_;
};

TEST(TTFServiceTest, TTFServiceAutomaticRegistration) {
  // Verify TTF service was registered automatically by including ttf_service.h
  EXPECT_TRUE(ServiceRegistry::isServiceRegistered<TTF>());
}

TEST(TTFServiceTest, TTFServiceCanBeCreated) {
  // Create TTF service instance directly
  auto ttfService = ServiceRegistry::createService<TTF>();
  EXPECT_NE(ttfService, nullptr);
}

TEST(TTFServiceTest, ApplicationCanRequestTTFService) {
  // Clean up any previous state
  ApplicationRunner::getInstance().reset();
  
  // Create test application that uses TTF service
  auto app = std::make_unique<TTFServiceTestApp>();
  auto* appPtr = app.get();
  
  // Register and initialize application
  ApplicationRunner::registerApplication(std::move(app));
  
  // Initialize the application (this should request TTF service)
  auto* registeredApp = ApplicationRunner::getInstance().getApplication();
  ASSERT_NE(registeredApp, nullptr);
  
  bool initSuccess = registeredApp->init();
  EXPECT_TRUE(initSuccess) << "Init failed with error: " << appPtr->getErrorMessage();
  
  // Verify TTF service was requested successfully
  EXPECT_TRUE(appPtr->wasTTFRequested());
  
  // Clean up
  ApplicationRunner::getInstance().reset();
}

TEST(TTFServiceTest, TTFServiceSingletonBehavior) {
  // Clean up any previous state
  ApplicationRunner::getInstance().reset();
  
  // Create application that requests TTF service twice
  class DoubleTTFApp : public BaseApplication {
  public:
    auto init() -> bool override {
      auto& ttf1 = requestService<TTF>();
      auto& ttf2 = requestService<TTF>();
      
      // Should be the same instance
      areSameInstance_ = (&ttf1 == &ttf2);
      return true;
    }
    
    auto iterate() -> bool override { return false; }
    auto quit() -> void override {}
    
    auto areSameInstance() const -> bool { return areSameInstance_; }
    
  private:
    bool areSameInstance_ = false;
  };
  
  auto app = std::make_unique<DoubleTTFApp>();
  auto* appPtr = app.get();
  
  ApplicationRunner::registerApplication(std::move(app));
  ApplicationRunner::getInstance().getApplication()->init();
  
  // Verify singleton behavior - same instance returned
  EXPECT_TRUE(appPtr->areSameInstance());
  
  // Clean up
  ApplicationRunner::getInstance().reset();
}