#ifndef __SDL_SURFACE_H__
#define __SDL_SURFACE_H__

#include <cinttypes>
#include <filesystem>
#include <memory>

namespace sdl {

class SurfaceImpl;

//! A class which holds a collection of pixels to be used in software blitting.
class Surface {
  public:
    Surface(uint32_t width, uint32_t height, uint8_t depth, uint32_t redMask, uint32_t greenMask, uint32_t blueMask, uint32_t alphaMask );
    Surface(std::filesystem::path filePath);
    Surface(Surface&) = delete;
    Surface(Surface&& other) noexcept;
    ~Surface();

    Surface& operator=(Surface&) = delete;
    Surface& operator=(Surface&& other) noexcept;


  private:
    std::unique_ptr<SurfaceImpl> _surfaceImpl;
};

}

#endif
