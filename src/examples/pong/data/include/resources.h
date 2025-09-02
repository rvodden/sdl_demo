#ifndef RESOURCES_H
#define RESOURCES_H

#include <cinttypes>

#ifdef _WIN32
      
  #include <windows.h>
  #include "resource.h"

  inline auto getPressStart2PRegularData() -> const uint8_t* {
      static_assert(kPressStart2P_Regular_Ttf != 0, "Font resource ID must be defined");
      HRSRC hResource = FindResource(nullptr, MAKEINTRESOURCE(kPressStart2P_Regular_Ttf), RT_RCDATA);
      if (hResource == nullptr) { 
          return nullptr; 
      }
      HGLOBAL hMemory = LoadResource(nullptr, hResource);
      if (hMemory == nullptr) { 
          return nullptr; 
      }
      return static_cast<const uint8_t*>(LockResource(hMemory));
  }

  inline auto pressStart2PRegularSize() -> uint32_t {
      static_assert(kPressStart2P_Regular_Ttf != 0, "Font resource ID must be defined");
      HRSRC hResource = FindResource(nullptr, MAKEINTRESOURCE(kPressStart2P_Regular_Ttf), RT_RCDATA);
      if (hResource == nullptr) { return 0; }
      return SizeofResource(nullptr, hResource);
  }

  // For consistency with Linux, provide a symbol that can be addressed
  inline const uint8_t* const _binary_PressStart2P_Regular_ttf_start_ptr = getPressStart2PRegularData();
  #define _binary_PressStart2P_Regular_ttf_start (*_binary_PressStart2P_Regular_ttf_start_ptr)

#else

  extern "C" const uint8_t _binary_PressStart2P_Regular_ttf_start;
  extern "C" const uint8_t _binary_PressStart2P_Regular_ttf_end;

  inline auto pressStart2PRegularSize() -> uint32_t {
    return static_cast<uint32_t>(&_binary_PressStart2P_Regular_ttf_end -
                                &_binary_PressStart2P_Regular_ttf_start);
  }

#endif // _WIN32

#endif // RESOURCES_H
