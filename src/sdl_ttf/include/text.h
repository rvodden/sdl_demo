#ifndef SDL_TTF_TEXT_H
#define SDL_TTF_TEXT_H

#include <string_view>

#include <sdl_ttf_export.h>

#include "color.h"
#include "font.h"
#include "surface.h"

namespace sdl::ttf {

class TextImpl;

class SDL_TTF_EXPORT Text {
  public:
    Text(std::string_view text, const Font& font);
    Text(const Text& other);
    Text(Text&& other) noexcept;

    auto operator=(const Text& other) -> Text&;
    auto operator=(Text&& other) noexcept -> Text&;
    
    ~Text();

    static auto renderBlended(const Font& font, const std::string_view& text, const Color& color) -> Surface;

  private:
    std::unique_ptr<TextImpl> _impl {};
};

}

#endif
