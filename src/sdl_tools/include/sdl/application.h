#ifndef SDL_APPLICATION_H
#define SDL_APPLICATION_H

#include <memory>
#include <cstdint>
#include <concepts>
#include <functional>
#include <typeindex>
#include <unordered_map>
#include <any>
#include <stdexcept>

#include "sdl_tools_export.h"

namespace sdl {

// Forward declarations
class BaseEventBus;
class SDL;
class ApplicationRunner;

namespace tools {
  class EventRouter;
}

enum class AppResult : uint8_t {
  kContinue,
  kSuccess,
  kFailure
};

// Service registry for optional services
class SDL_TOOLS_EXPORT ServiceRegistry {
public:
  // Register a service factory function
  template<typename T>
  static auto registerServiceFactory(std::function<std::unique_ptr<T>()> factory) -> void {
    getInstance()._factories[std::type_index(typeid(T))] = [factory]() -> void* {
      return factory().release();
    };
  }
  
  // Create a service instance using registered factory
  template<typename T>
  static auto createService() -> std::unique_ptr<T> {
    auto& instance = getInstance();
    auto it = instance._factories.find(std::type_index(typeid(T)));
    if (it == instance._factories.end()) {
      throw std::runtime_error("Service factory not registered for type: " + std::string(typeid(T).name()));
    }
    
    void* rawPtr = it->second();
    return std::unique_ptr<T>(static_cast<T*>(rawPtr));
  }
  
  // Check if service factory is registered
  template<typename T>
  static auto isServiceRegistered() -> bool {
    return getInstance()._factories.find(std::type_index(typeid(T))) != getInstance()._factories.end();
  }

private:
  static auto getInstance() -> ServiceRegistry& {
    static ServiceRegistry instance;
    return instance;
  }
  
  std::unordered_map<std::type_index, std::function<void*()>> _factories;
};

// Base interface for all applications
class SDL_TOOLS_EXPORT BaseApplication {
 public:
  virtual ~BaseApplication() = default;
  
  virtual auto init() -> bool = 0;
  virtual auto iterate() -> bool = 0;
  virtual auto quit() -> void = 0;
  
  // Exception-safe wrappers for the virtual methods
  auto safeInit() -> bool;
  auto safeIterate() -> bool;
  auto safeQuit() -> void;
  
protected:
  // Service request methods - SDL has hard dependency, others use registry
  auto requestSDL() -> SDL&;
  
  template<typename T>
  auto requestService() -> T&;
  
  auto getEventBus() -> std::shared_ptr<BaseEventBus>;
  auto getEventRouter() -> std::shared_ptr<tools::EventRouter>;
  
private:
  friend class ApplicationRunner;  // Allow access to service management
};

// Runner that manages application lifecycle
class SDL_TOOLS_EXPORT ApplicationRunner {
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
  
  // Service management for BaseApplication
  auto getOrCreateSDL() -> SDL&;
  
  template<typename T>
  auto getOrCreateService() -> T& {
    auto serviceKey = std::type_index(typeid(T));
    
    auto it = _services.find(serviceKey);
    if (it != _services.end()) {
      return *static_cast<T*>(it->second.get());
    }
    
    // Create new service using registry
    auto service = ServiceRegistry::createService<T>();
    T* servicePtr = service.get();
    
    // Store as type-erased unique_ptr with custom deleter
    auto deleter = [](void* ptr) { delete static_cast<T*>(ptr); };
    _services.emplace(serviceKey, std::unique_ptr<void, void(*)(void*)>(service.release(), deleter));
    
    return *servicePtr;
  }

 private:
  // Private constructor for singleton
  ApplicationRunner() = default;
  
  // Private members instead of global state
  std::unique_ptr<BaseApplication> _application;
  std::shared_ptr<BaseEventBus> _eventBus;
  std::shared_ptr<tools::EventRouter> _eventRouter;
  
  // Service storage - SDL managed directly, others via registry
  std::unique_ptr<SDL> _sdl;
  std::unordered_map<std::type_index, std::unique_ptr<void, void(*)(void*)>> _services;
  
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

// Template method implementations (must be after ApplicationRunner declaration)
template<typename T>
inline auto BaseApplication::requestService() -> T& {
  return ApplicationRunner::getInstance().getOrCreateService<T>();
}

}  // namespace sdl

// Convenience macro for application registration
#define REGISTER_APPLICATION(AppClass) \
    static auto _register_##AppClass = sdl::register_application<AppClass>();

#endif  // SDL_APPLICATION_H
