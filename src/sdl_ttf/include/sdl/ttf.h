#ifndef SDL_TTF_TTF_H
#define SDL_TTF_TTF_H

#include <sdl_ttf_export.h>

namespace sdl::ttf {

class SDL_TTF_EXPORT TTF {
  public:
    TTF();
    TTF(const TTF& other) = delete;
    TTF(TTF&& other) = delete;

    auto operator=(const TTF& other) = delete;
    auto operator=(TTF&& other) noexcept = delete;

    ~TTF();

};

} // namespace sdl::ttf

#endif
