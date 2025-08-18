#ifndef IMAGES_H
#define IMAGES_H

#include <cinttypes>

extern "C" const uint8_t _binary_tic_tac_toe_png_start;
extern "C" const uint8_t _binary_tic_tac_toe_png_end;

auto ticTacToeSize() -> uint32_t {
  return static_cast<uint32_t>(&_binary_tic_tac_toe_png_end -
                               &_binary_tic_tac_toe_png_start);
}

#endif
