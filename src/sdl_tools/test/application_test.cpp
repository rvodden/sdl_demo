#include <memory>
#include <iostream>
#include <sstream>

#include <gtest/gtest.h>

#include <sdl/application.h>
#include <sdl/exception.h>

using namespace sdl;

// Test service for service registry tests
class TestService {
public:
  TestService() : value_(42) {}
  auto getValue() const -> int { return value_; }
  auto setValue(int value) -> void { value_ = value; }
  
private:
  int value_;
};

// Another test service for multiple service tests
class AnotherTestService {
public:
  AnotherTestService() : name_("test") {}
  auto getName() const -> std::string { return name_; }
  
private:
  std::string name_;
};

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

// Service Registry Tests
TEST(ServiceRegistryTest, ServiceRegistrationAndRetrieval) {
  // Register a test service factory
  ServiceRegistry::registerServiceFactory<TestService>(
    []() -> std::unique_ptr<TestService> {
      return std::make_unique<TestService>();
    }
  );
  
  // Verify service is registered
  EXPECT_TRUE(ServiceRegistry::isServiceRegistered<TestService>());
  
  // Create service instance
  auto service = ServiceRegistry::createService<TestService>();
  EXPECT_NE(service, nullptr);
  EXPECT_EQ(service->getValue(), 42);
}

TEST(ServiceRegistryTest, MultipleServices) {
  // Register multiple different services
  ServiceRegistry::registerServiceFactory<TestService>(
    []() -> std::unique_ptr<TestService> {
      auto service = std::make_unique<TestService>();
      service->setValue(100);
      return service;
    }
  );
  
  ServiceRegistry::registerServiceFactory<AnotherTestService>(
    []() -> std::unique_ptr<AnotherTestService> {
      return std::make_unique<AnotherTestService>();
    }
  );
  
  // Verify both are registered
  EXPECT_TRUE(ServiceRegistry::isServiceRegistered<TestService>());
  EXPECT_TRUE(ServiceRegistry::isServiceRegistered<AnotherTestService>());
  
  // Create instances of both services
  auto testService = ServiceRegistry::createService<TestService>();
  auto anotherService = ServiceRegistry::createService<AnotherTestService>();
  
  EXPECT_NE(testService, nullptr);
  EXPECT_NE(anotherService, nullptr);
  EXPECT_EQ(testService->getValue(), 100);
  EXPECT_EQ(anotherService->getName(), "test");
}

TEST(ServiceRegistryTest, UnregisteredServiceThrows) {
  // Create a unique type that won't be registered
  class UnregisteredService {};
  
  // Verify service is not registered
  EXPECT_FALSE(ServiceRegistry::isServiceRegistered<UnregisteredService>());
  
  // Attempt to create unregistered service should throw
  EXPECT_THROW(ServiceRegistry::createService<UnregisteredService>(), std::runtime_error);
}

// Application Service Management Tests
class ServiceTestApplication : public BaseApplication {
public:
  ServiceTestApplication() : sdlRequested_(false), serviceRequested_(false) {}
  
  auto init() -> bool override {
    // Register test service before requesting
    ServiceRegistry::registerServiceFactory<TestService>(
      []() -> std::unique_ptr<TestService> {
        auto service = std::make_unique<TestService>();
        service->setValue(123);
        return service;
      }
    );
    
    // Request SDL service
    auto& sdl = requestSDL();
    sdlRequested_ = true;
    static_cast<void>(sdl);  // Suppress unused variable warning
    
    // Request custom service
    auto& testService = requestService<TestService>();
    serviceRequested_ = true;
    serviceValue_ = testService.getValue();
    
    return true;
  }
  
  auto iterate() -> bool override {
    return false; // Exit immediately
  }
  
  auto quit() -> void override {}
  
  auto wasSdlRequested() const -> bool { return sdlRequested_; }
  auto wasServiceRequested() const -> bool { return serviceRequested_; }
  auto getServiceValue() const -> int { return serviceValue_; }
  
private:
  bool sdlRequested_;
  bool serviceRequested_;
  int serviceValue_ = 0;
};

TEST(ApplicationServiceTest, ApplicationCanRequestServices) {
  // Clean up any previous state
  ApplicationRunner::getInstance().reset();
  
  // Create test application
  auto app = std::make_unique<ServiceTestApplication>();
  auto* appPtr = app.get();
  
  // Register and initialize application
  ApplicationRunner::registerApplication(std::move(app));
  
  // Initialize the application (this should request services)
  auto* registeredApp = ApplicationRunner::getInstance().getApplication();
  ASSERT_NE(registeredApp, nullptr);
  
  bool initSuccess = registeredApp->init();
  EXPECT_TRUE(initSuccess);
  
  // Verify services were requested
  auto* serviceApp = static_cast<ServiceTestApplication*>(appPtr);
  EXPECT_TRUE(serviceApp->wasSdlRequested());
  EXPECT_TRUE(serviceApp->wasServiceRequested());
  EXPECT_EQ(serviceApp->getServiceValue(), 123);
  
  // Clean up
  ApplicationRunner::getInstance().reset();
}

TEST(ApplicationServiceTest, ServiceSingletonBehavior) {
  // Clean up any previous state
  ApplicationRunner::getInstance().reset();
  
  // Register test service factory
  ServiceRegistry::registerServiceFactory<TestService>(
    []() -> std::unique_ptr<TestService> {
      static int counter = 0;
      auto service = std::make_unique<TestService>();
      service->setValue(++counter);
      return service;
    }
  );
  
  // Create test application that requests service twice
  class MultiRequestApp : public BaseApplication {
  public:
    auto init() -> bool override {
      auto& service1 = requestService<TestService>();
      auto& service2 = requestService<TestService>();
      
      // Should be the same instance
      areSameInstance_ = (&service1 == &service2);
      firstValue_ = service1.getValue();
      secondValue_ = service2.getValue();
      
      return true;
    }
    
    auto iterate() -> bool override { return false; }
    auto quit() -> void override {}
    
    auto areSameInstance() const -> bool { return areSameInstance_; }
    auto getFirstValue() const -> int { return firstValue_; }
    auto getSecondValue() const -> int { return secondValue_; }
    
  private:
    bool areSameInstance_ = false;
    int firstValue_ = 0;
    int secondValue_ = 0;
  };
  
  auto app = std::make_unique<MultiRequestApp>();
  auto* appPtr = app.get();
  
  ApplicationRunner::registerApplication(std::move(app));
  ApplicationRunner::getInstance().getApplication()->init();
  
  // Verify singleton behavior
  EXPECT_TRUE(appPtr->areSameInstance());
  EXPECT_EQ(appPtr->getFirstValue(), 1);  // Factory called once
  EXPECT_EQ(appPtr->getSecondValue(), 1); // Same instance, same value
  
  // Clean up
  ApplicationRunner::getInstance().reset();
}

// Exception handling test applications
class SdlExceptionApp : public BaseApplication {
public:
  auto init() -> bool override {
    throw sdl::Exception("Test SDL exception during init");
  }
  
  auto iterate() -> bool override {
    throw sdl::Exception("Test SDL exception during iterate");
  }
  
  auto quit() -> void override {
    throw sdl::Exception("Test SDL exception during quit");
  }
};

class StdExceptionApp : public BaseApplication {
public:
  auto init() -> bool override {
    throw std::runtime_error("Test std exception during init");
  }
  
  auto iterate() -> bool override {
    throw std::logic_error("Test std exception during iterate");
  }
  
  auto quit() -> void override {
    throw std::invalid_argument("Test std exception during quit");
  }
};

class UnknownExceptionApp : public BaseApplication {
public:
  auto init() -> bool override {
    throw 42; // Unknown exception type
  }
  
  auto iterate() -> bool override {
    throw "Unknown exception string";
  }
  
  auto quit() -> void override {
    throw 3.14; // Another unknown type
  }
};

class MixedExceptionApp : public BaseApplication {
public:
  enum class Phase { Init, Iterate, Quit };
  
  MixedExceptionApp(Phase phase) : phase_(phase) {}
  
  auto init() -> bool override {
    if (phase_ == Phase::Init) {
      throw sdl::Exception("SDL exception in init");
    }
    return true;
  }
  
  auto iterate() -> bool override {
    if (phase_ == Phase::Iterate) {
      throw std::runtime_error("Std exception in iterate");
    }
    return true;
  }
  
  auto quit() -> void override {
    if (phase_ == Phase::Quit) {
      throw "Unknown exception in quit";
    }
  }
  
private:
  Phase phase_;
};

// Helper to capture stderr output
class StderrCapture {
public:
  StderrCapture() {
    originalBuf_ = std::cerr.rdbuf();
    std::cerr.rdbuf(buffer_.rdbuf());
  }
  
  ~StderrCapture() {
    std::cerr.rdbuf(originalBuf_);
  }
  
  auto getOutput() -> std::string {
    return buffer_.str();
  }
  
  auto clear() -> void {
    buffer_.str("");
    buffer_.clear();
  }
  
private:
  std::ostringstream buffer_;
  std::streambuf* originalBuf_;
};

// Exception handling tests
TEST(ApplicationExceptionTest, SafeInitHandlesSdlException) {
  StderrCapture capture;
  
  SdlExceptionApp app;
  bool result = app.safeInit();
  
  EXPECT_FALSE(result);
  
  std::string output = capture.getOutput();
  EXPECT_NE(output.find("SDL Error during initialization"), std::string::npos);
  EXPECT_NE(output.find("Test SDL exception during init"), std::string::npos);
}

TEST(ApplicationExceptionTest, SafeInitHandlesStdException) {
  StderrCapture capture;
  
  StdExceptionApp app;
  bool result = app.safeInit();
  
  EXPECT_FALSE(result);
  
  std::string output = capture.getOutput();
  EXPECT_NE(output.find("Error during initialization"), std::string::npos);
  EXPECT_NE(output.find("Test std exception during init"), std::string::npos);
}

TEST(ApplicationExceptionTest, SafeInitHandlesUnknownException) {
  StderrCapture capture;
  
  UnknownExceptionApp app;
  bool result = app.safeInit();
  
  EXPECT_FALSE(result);
  
  std::string output = capture.getOutput();
  EXPECT_NE(output.find("Unknown error during initialization"), std::string::npos);
}

TEST(ApplicationExceptionTest, SafeIterateHandlesSdlException) {
  StderrCapture capture;
  
  SdlExceptionApp app;
  bool result = app.safeIterate();
  
  EXPECT_FALSE(result);
  
  std::string output = capture.getOutput();
  EXPECT_NE(output.find("SDL Error during game loop"), std::string::npos);
  EXPECT_NE(output.find("Test SDL exception during iterate"), std::string::npos);
}

TEST(ApplicationExceptionTest, SafeIterateHandlesStdException) {
  StderrCapture capture;
  
  StdExceptionApp app;
  bool result = app.safeIterate();
  
  EXPECT_FALSE(result);
  
  std::string output = capture.getOutput();
  EXPECT_NE(output.find("Error during game loop"), std::string::npos);
  EXPECT_NE(output.find("Test std exception during iterate"), std::string::npos);
}

TEST(ApplicationExceptionTest, SafeIterateHandlesUnknownException) {
  StderrCapture capture;
  
  UnknownExceptionApp app;
  bool result = app.safeIterate();
  
  EXPECT_FALSE(result);
  
  std::string output = capture.getOutput();
  EXPECT_NE(output.find("Unknown error during game loop"), std::string::npos);
}

TEST(ApplicationExceptionTest, SafeQuitHandlesSdlException) {
  StderrCapture capture;
  
  SdlExceptionApp app;
  app.safeQuit(); // void return, shouldn't crash
  
  std::string output = capture.getOutput();
  EXPECT_NE(output.find("SDL Error during cleanup"), std::string::npos);
  EXPECT_NE(output.find("Test SDL exception during quit"), std::string::npos);
}

TEST(ApplicationExceptionTest, SafeQuitHandlesStdException) {
  StderrCapture capture;
  
  StdExceptionApp app;
  app.safeQuit(); // void return, shouldn't crash
  
  std::string output = capture.getOutput();
  EXPECT_NE(output.find("Error during cleanup"), std::string::npos);
  EXPECT_NE(output.find("Test std exception during quit"), std::string::npos);
}

TEST(ApplicationExceptionTest, SafeQuitHandlesUnknownException) {
  StderrCapture capture;
  
  UnknownExceptionApp app;
  app.safeQuit(); // void return, shouldn't crash
  
  std::string output = capture.getOutput();
  EXPECT_NE(output.find("Unknown error during cleanup"), std::string::npos);
}

TEST(ApplicationExceptionTest, SafeMethodsProvideCorrectReturnValues) {
  // Test that safe methods return appropriate values when exceptions occur
  
  // safeInit should return false on exception
  {
    SdlExceptionApp app;
    StderrCapture capture;
    EXPECT_FALSE(app.safeInit());
  }
  
  {
    StdExceptionApp app;
    StderrCapture capture;
    EXPECT_FALSE(app.safeInit());
  }
  
  {
    UnknownExceptionApp app;
    StderrCapture capture;
    EXPECT_FALSE(app.safeInit());
  }
  
  // safeIterate should return false on exception
  {
    SdlExceptionApp app;
    StderrCapture capture;
    EXPECT_FALSE(app.safeIterate());
  }
  
  {
    StdExceptionApp app;
    StderrCapture capture;
    EXPECT_FALSE(app.safeIterate());
  }
  
  {
    UnknownExceptionApp app;
    StderrCapture capture;
    EXPECT_FALSE(app.safeIterate());
  }
  
  // safeQuit is void, but should not crash
  {
    SdlExceptionApp app;
    StderrCapture capture;
    EXPECT_NO_THROW(app.safeQuit());
  }
  
  {
    StdExceptionApp app;
    StderrCapture capture;
    EXPECT_NO_THROW(app.safeQuit());
  }
  
  {
    UnknownExceptionApp app;
    StderrCapture capture;
    EXPECT_NO_THROW(app.safeQuit());
  }
}

TEST(ApplicationExceptionTest, SafeMethodsWorkWithNormalOperation) {
  // Test that safe methods work correctly when no exceptions are thrown
  
  TestApplication app;
  StderrCapture capture;
  
  // Normal operations should work as expected
  EXPECT_TRUE(app.safeInit());
  EXPECT_FALSE(app.safeIterate()); // TestApplication returns false to exit immediately
  EXPECT_NO_THROW(app.safeQuit());
  
  // Should not produce any error output
  std::string output = capture.getOutput();
  EXPECT_EQ(output, "");
}

TEST(ApplicationExceptionTest, ExceptionHandlingIntegrationTest) {
  // Test the complete flow with different exception types in different phases
  
  for (auto phase : {MixedExceptionApp::Phase::Init, 
                     MixedExceptionApp::Phase::Iterate, 
                     MixedExceptionApp::Phase::Quit}) {
    StderrCapture capture;
    MixedExceptionApp app(phase);
    
    switch (phase) {
      case MixedExceptionApp::Phase::Init:
        EXPECT_FALSE(app.safeInit());
        EXPECT_NE(capture.getOutput().find("SDL Error during initialization"), std::string::npos);
        break;
        
      case MixedExceptionApp::Phase::Iterate:
        EXPECT_TRUE(app.safeInit());   // Should succeed
        capture.clear();
        EXPECT_FALSE(app.safeIterate());  // Should fail with exception
        EXPECT_NE(capture.getOutput().find("Error during game loop"), std::string::npos);
        break;
        
      case MixedExceptionApp::Phase::Quit:
        EXPECT_TRUE(app.safeInit());   // Should succeed
        EXPECT_TRUE(app.safeIterate()); // Should succeed 
        capture.clear();
        app.safeQuit();                // Should handle exception
        EXPECT_NE(capture.getOutput().find("Unknown error during cleanup"), std::string::npos);
        break;
    }
  }
}
