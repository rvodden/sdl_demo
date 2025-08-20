#ifndef IMAGES_H
#define IMAGES_H

#include <cinttypes>

#ifdef _WIN32
      
  #include <windows.h>
  #include "resource.h"

  inline auto getTicTacToeData() -> const uint8_t* {
      HRSRC hResource = FindResource(nullptr, MAKEINTRESOURCE(kTicTacToe_Png), RT_RCDATA);
      if (hResource == nullptr) { return nullptr; }
      HGLOBAL hMemory = LoadResource(nullptr, hResource);
      if (hMemory == nullptr) { return nullptr; }
      return static_cast<const uint8_t*>(LockResource(hMemory));
  }

  inline auto ticTacToeSize() -> uint32_t {
      HRSRC hResource = FindResource(nullptr, MAKEINTRESOURCE(kTicTacToe_Png), RT_RCDATA);
      if (hResource == nullptr) { return 0; }
      return SizeofResource(nullptr, hResource);
  }

  #define _binary_tic_tac_toe_png_start (*getTicTacToeData())

#else

  extern "C" const uint8_t _binary_tic_tac_toe_png_start;
  extern "C" const uint8_t _binary_tic_tac_toe_png_end;

  auto ticTacToeSize() -> uint32_t {
    return static_cast<uint32_t>(&_binary_tic_tac_toe_png_end -
                                &_binary_tic_tac_toe_png_start);
  }

#endif // _WIN32

#endif // IMAGES_H
