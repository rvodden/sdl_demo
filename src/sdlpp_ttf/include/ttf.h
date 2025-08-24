#ifndef SDLPP_TTF_TTF_H
#define SDLPP_TTF_TTF_H

#include <sdlpp_ttf_export.h>

namespace sdlpp::ttf {

class SDLPP_TTF_EXPORT TTF {
  public:
    TTF();
    TTF(const TTF& other) = delete;
    TTF(TTF&& other) = delete;

    auto operator=(const TTF& other) = delete;
    auto operator=(TTF&& other) noexcept = delete;

    ~TTF();

};

} // namespace sdlpp::ttf

#endif
