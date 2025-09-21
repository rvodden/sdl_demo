#ifndef SDL_TTF_SERVICE_H
#define SDL_TTF_SERVICE_H

#include <memory>

#include "sdl/ttf.h"

// Only include and register if application framework is available
#ifdef __has_include
  #if __has_include("sdl/sdl_tools.h")

    #include <sdl/sdl_tools.h>
    
    namespace sdl::ttf::detail {
    constexpr bool kSDLApplicationAvailable = true;

    // Transparent TTF service registration for application framework
      inline auto registerTTFService() -> int {
        if constexpr (kSDLApplicationAvailable) {
          sdl::ServiceRegistry::registerServiceFactory<TTF>(
            []() -> std::unique_ptr<TTF> {
              return std::make_unique<TTF>();
            }
          );
        }
        return 0;
      }
      
      // Static initialization ensures registration happens when this header is included
      [[maybe_unused]] static auto ttf_registration = registerTTFService();

    } // namespace sdl::ttf::detail
  #else
    constexpr bool kSDLApplicationAvailable = false;
  #endif
#else
  constexpr bool kSDLApplicationAvailable = false;
#endif



#endif // SDL_TTF_SERVICE_H
