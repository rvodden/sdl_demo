#ifndef SDLPP_TTF_SERVICE_H
#define SDLPP_TTF_SERVICE_H

#include <memory>
#include "ttf.h"

// Only include and register if application framework is available
#ifdef __has_include
  #if __has_include("application.h")
    #include "application.h"
    constexpr bool kSDLPPApplicationAvailable = true;
  #else
    constexpr bool kSDLPPApplicationAvailable = false;
  #endif
#else
  constexpr bool kSDLPPApplicationAvailable = false;
#endif

namespace sdlpp::ttf {

// Transparent TTF service registration for application framework
namespace detail {
  inline auto registerTTFService() -> int {
    if constexpr (kSDLPPApplicationAvailable) {
      sdlpp::ServiceRegistry::registerServiceFactory<TTF>(
        []() -> std::unique_ptr<TTF> {
          return std::make_unique<TTF>();
        }
      );
    }
    return 0;
  }
  
  // Static initialization ensures registration happens when this header is included
  [[maybe_unused]] static auto ttf_registration = registerTTFService();
}

} // namespace sdlpp::ttf

#endif // SDLPP_TTF_SERVICE_H