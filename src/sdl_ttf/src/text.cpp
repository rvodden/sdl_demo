#include <text.h>
#include <text_impl.h>
#include <font_impl.h>

#include "surface_impl.h"
#include "color_impl.h"

#include <SDL3_ttf/SDL_ttf.h>

namespace sdl::ttf {

Text::Text(std::string_view text, const Font& font) {
  _impl->_text = TTF_CreateText(nullptr, font._impl->_font, text.data(), text.length());
}

Text::~Text() {
  TTF_DestroyText(_impl->_text);
}

auto Text::renderBlended(const Font& font, const std::string_view& text, const Color& color) -> Surface {
  auto *sdl_surface = TTF_RenderText_Blended(font._impl->_font, text.data(), text.length(), createSDLColor(color));
  return SurfaceImpl::createSurface(sdl_surface);
}

}
