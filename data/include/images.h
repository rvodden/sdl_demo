#ifndef __IMAGES_H__
#define __IMAGES_H__

#include <cinttypes>

extern "C" const uint8_t _binary_tic_tac_toe_png_start;
extern "C" const uint8_t _binary_tic_tac_toe_png_end;

uint32_t ticTacToeSize() {
  return static_cast<uint32_t>(&_binary_tic_tac_toe_png_end - &_binary_tic_tac_toe_png_start);
}

#endif
