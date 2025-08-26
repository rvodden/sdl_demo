#include <exception.h>
#include <font.h>
#include <font_impl.h>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

namespace sdlpp::ttf {

Font::Font(const void* location, std::size_t data_size, float point_size) : _impl(std::make_unique<FontImpl>()) {
  if(location == nullptr || data_size == 0) {
    throw Exception("Invalid font data: null pointer or zero size");
  }

  SDL_IOStream* iostream = SDL_IOFromConstMem(location, data_size);
  if(iostream == nullptr) {
    throw Exception("SDL_IOFromConstMem");
  }

  _impl->_font = TTF_OpenFontIO(iostream, true, point_size);
  if(_impl->_font == nullptr) {
    throw Exception("TTF_OpenFontIO");
  }
}

Font::Font(const Font& other) : _impl(std::make_unique<FontImpl>()) {
  *this = other;
}

Font::Font(Font&& other) noexcept {
  *this = std::move(other);
}

auto Font::operator=(const Font& other) -> Font& {
  if(this != &other) {
    _impl->_font = TTF_CopyFont(other._impl->_font);
    if(_impl->_font == nullptr) {
      throw Exception("TTF_CopyFont");
    }
  }
  return *this;
}

auto Font::operator=(Font&& other) noexcept -> Font& {
  if(this != &other) {
    _impl = std::move(other._impl);
  } 
  return *this;
}


Font::~Font() {
  if(_impl && _impl->_font) {
    TTF_CloseFont(_impl->_font);
  }
};

}
