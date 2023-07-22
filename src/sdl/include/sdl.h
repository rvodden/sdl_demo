#ifndef __SDL_SDL_H__
#define __SDL_SDL_H__

#include <chrono>
#include <memory>

namespace sdl {

class SDLImpl;

class SDL {
  public:
    SDL();
    ~SDL() noexcept;

    typedef uint8_t SubSystem; 
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
    void initSubSystem(const SubSystem& subSystem );

  private:
    std::unique_ptr<SDLImpl> _sdlImpl;
};

void delay_ms(uint32_t duration);

//! @brief Wait a specified duration before returning
template<class T>
void delay(std::chrono::duration<T> duration) {
  delay_ms((std::chrono::duration_cast<std::chrono::milliseconds>(duration)).count());
}

}
#endif
