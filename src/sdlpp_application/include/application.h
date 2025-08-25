#ifndef SDLPP_APPLICATION_H
#define SDLPP_APPLICATION_H

#include <memory>
#include <cstdint>
#include <concepts>

#include "sdlpp_application_export.h"

namespace sdlpp {

// Forward declarations
class BaseEventBus;
namespace tools {
  class EventRouter;
}

enum class AppResult : uint8_t {
  kContinue,
  kSuccess,
  kFailure
};

// Base interface for all applications
class SDLPP_APPLICATION_EXPORT BaseApplication {
 public:
  virtual ~BaseApplication() = default;
  
  virtual auto init() -> bool = 0;
  virtual auto iterate() -> bool = 0;
  virtual auto quit() -> void = 0;
};

// Runner that manages application lifecycle
class SDLPP_APPLICATION_EXPORT ApplicationRunner {
 public:
  // Register an application instance
  static auto registerApplication(std::unique_ptr<BaseApplication> app) -> void;
  
  // Run the registered application (called by library's main)
  static auto run() -> int;
  
  // Get singleton instance (used by SDL callbacks)
  static auto getInstance() -> ApplicationRunner&;
  
  // Accessors for SDL callbacks
  auto getApplication() -> BaseApplication*;
  auto getEventBus() -> std::shared_ptr<BaseEventBus>;
  auto getEventRouter() -> std::shared_ptr<tools::EventRouter>;
  
  // Reset application state
  auto reset() -> void;
  
  // Initialize event system
  auto initializeEventSystem() -> void;

 private:
  // Private constructor for singleton
  ApplicationRunner() = default;
  
  // Private members instead of global state
  std::unique_ptr<BaseApplication> _application;
  std::shared_ptr<BaseEventBus> _eventBus;
  std::shared_ptr<tools::EventRouter> _eventRouter;
  
};

// Concept to enforce that registered applications derive from BaseApplication
template<typename T>
concept ApplicationType = std::derived_from<T, BaseApplication>;

// Template function for type-safe application registration
template<ApplicationType AppClass>
inline auto register_application() -> int {
  ApplicationRunner::registerApplication(std::make_unique<AppClass>());
  return 0;
}

}  // namespace sdlpp

// Convenience macro for application registration
#define REGISTER_APPLICATION(AppClass) \
    static auto _register_##AppClass = sdlpp::register_application<AppClass>();

#endif  // SDLPP_APPLICATION_H
