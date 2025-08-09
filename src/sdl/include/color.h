#ifndef SDL_COLOR_H
#define SDL_COLOR_H

#include <cstdint>
#include <tuple>

namespace sdl {

enum class Alpha : uint8_t {
  kOpaque = 255,
  kTranparent = 0
};

class Color {
  public:
    Color() = default;
    constexpr Color(
      const uint8_t& red, 
      const uint8_t& green, 
      const uint8_t& blue, 
      const uint8_t& alpha) : _color { std::make_tuple(red, green, blue) }, _alpha { alpha } = default;

    uint8_t getRed() const { return std::get<0>(_color); };
    uint8_t getGreen() const { return std::get<1>(_color); };
    uint8_t getBlue() const { return std::get<2>(_color); };
    uint8_t getAlpha() const { return _alpha; };
  
  private:
    std::tuple<uint8_t, uint8_t, uint8_t> _color { 0, 0, 0 };
    uint8_t _alpha { static_cast<uint8_t>(Alpha::kOpaque) };

};

class NamedColor {
  public:
    static constexpr Color kBlack = Color{ 0, 0, 0, static_cast<uint8_t>(Alpha::kTranparent) };
    static constexpr Color kWhite = Color{ 255, 255, 255, static_cast<uint8_t>(Alpha::kTranparent) };
    static constexpr Color kMagenta = Color{ 0xc2, 0x00, 0x78, static_cast<uint8_t>(Alpha::kTranparent) };
};

}

#endif
