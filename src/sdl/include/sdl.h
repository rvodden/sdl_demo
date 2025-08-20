#ifndef SDL_SDL_H
#define SDL_SDL_H

#include <chrono>
#include <memory>

#include "sdl_export.h"

namespace sdl {

class SDLImpl;

class SDL_EXPORT SDL {
 public:
  SDL();
  SDL(const SDL& sdl) = delete;
  SDL(SDL&& sdl) noexcept = delete;

  auto operator=(const SDL&) -> SDL& = delete;
  auto operator=(SDL&&) -> SDL& = delete;

  ~SDL() noexcept;

  using SubSystem = uint8_t;
  static constexpr SubSystem kTimer = 0;
  static constexpr SubSystem kAudio = 1;
  static constexpr SubSystem kVideo = 2;
  static constexpr SubSystem kJoystick = 3;
  static constexpr SubSystem kHaptic = 4;
  static constexpr SubSystem kGameController = 5;
  static constexpr SubSystem kEvents = 6;
  static constexpr SubSystem kEverything = 7;
  static constexpr SubSystem kNoParachute = 8;

  //! @brief Initialize the SubSystems of the SDL library.
  void initSubSystem(const SubSystem& subSystem);

 private:
  std::unique_ptr<SDLImpl> _sdlImpl;
};

void delay_ms(uint32_t duration);

//! @brief Wait a specified duration before returning
template <class T>
void delay(std::chrono::duration<T> duration) {
  delay_ms((std::chrono::duration_cast<std::chrono::milliseconds>(duration))
               .count());
}

}  // namespace sdl
#endif
