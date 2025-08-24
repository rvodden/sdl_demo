#ifndef SDLPP_TTF_FONT_H
#define SDLPP_TTF_FONT_H

#include <memory>

#include <sdlpp_ttf_export.h>

namespace sdlpp::ttf {

class Text;
class FontImpl;

class SDLPP_TTF_EXPORT Font {
    friend Text;
  public:
    Font(const void* location, std::size_t data_size, float point_size);
    Font(const Font& other);
    Font(Font&& other) noexcept;

    auto operator=(const Font& other) -> Font&;
    auto operator=(Font&& other) noexcept -> Font&;
    ~Font();

  private:
    std::unique_ptr<FontImpl> _impl;
};

}

#endif
