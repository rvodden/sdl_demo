#include "surface.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <algorithm>
#include <cstring>
#include <memory>

#include "exception.h"
#include "surface_impl.h"

namespace sdl {

Surface::Surface(const std::filesystem::path& filePath)
    : _impl{std::make_unique<SurfaceImpl>()} {
  _impl->_sdlSurface = IMG_Load(filePath.string().c_str());
  if (_impl->_sdlSurface == nullptr) {
    throw Exception("IMG_Load failed: " + filePath.string());
  }

  // Convert to a consistent RGBA format for uniform pixel access
  SDL_Surface* converted = SDL_ConvertSurface(_impl->_sdlSurface, SDL_PIXELFORMAT_RGBA8888);
  if (converted == nullptr) {
    SDL_DestroySurface(_impl->_sdlSurface);
    throw Exception("SDL_ConvertSurface");
  }
  
  SDL_DestroySurface(_impl->_sdlSurface);
  _impl->_sdlSurface = converted;
}

Surface::Surface(uint32_t width, uint32_t height)
    : _impl{std::make_unique<SurfaceImpl>()} {
  _impl->_sdlSurface = SDL_CreateSurface(static_cast<int>(width), static_cast<int>(height), SDL_PIXELFORMAT_RGBA8888);
  if (_impl->_sdlSurface == nullptr) {
    throw Exception("SDL_CreateSurface");
  }
}

Surface::Surface(uint32_t width, uint32_t height, const std::vector<uint8_t>& pixels)
    : _impl{std::make_unique<SurfaceImpl>()} {
  if (pixels.size() != width * height * 4) {
    throw Exception("Invalid pixel data size");
  }

  _impl->_sdlSurface = SDL_CreateSurface(static_cast<int>(width), static_cast<int>(height), SDL_PIXELFORMAT_RGBA8888);
  if (_impl->_sdlSurface == nullptr) {
    throw Exception("SDL_CreateSurface");
  }

  // Copy pixel data
  if (_impl->_sdlSurface->pixels != nullptr) {
    std::memcpy(_impl->_sdlSurface->pixels, pixels.data(), pixels.size());
  } else {
    SDL_DestroySurface(_impl->_sdlSurface);
    throw Exception("Surface pixels not accessible");
  }
}

Surface::Surface(Surface&& other) noexcept
    : _impl{std::move(other._impl)} {}

Surface::~Surface() noexcept {
  if (_impl && _impl->_sdlSurface != nullptr) {
    SDL_DestroySurface(_impl->_sdlSurface);
  }
}

auto Surface::operator=(Surface&& other) noexcept -> Surface& {
  if (this == &other) {
    return *this;
  }
  if (_impl && _impl->_sdlSurface != nullptr) {
    SDL_DestroySurface(_impl->_sdlSurface);
  }
  _impl = std::move(other._impl);
  return *this;
}

auto Surface::getWidth() const -> uint32_t {
  return static_cast<uint32_t>(_impl->_sdlSurface->w);
}

auto Surface::getHeight() const -> uint32_t {
  return static_cast<uint32_t>(_impl->_sdlSurface->h);
}

auto Surface::getPixels() const -> std::vector<uint8_t> {
  auto* surface = _impl->_sdlSurface;
  const size_t pixelCount = static_cast<size_t>(surface->w) * static_cast<size_t>(surface->h) * 4;
  std::vector<uint8_t> pixels(pixelCount);

  // SDL3 surfaces may not require locking for direct access
  if (surface->pixels != nullptr) {
    std::memcpy(pixels.data(), surface->pixels, pixelCount);
  } else {
    throw Exception("Surface pixels not accessible");
  }

  return pixels;
}

void Surface::setPixels(const std::vector<uint8_t>& pixels) {
  auto* surface = _impl->_sdlSurface;
  const size_t expectedSize = static_cast<size_t>(surface->w) * static_cast<size_t>(surface->h) * 4;

  if (pixels.size() != expectedSize) {
    throw Exception("Invalid pixel data size");
  }

  // SDL3 surfaces may not require locking for direct access
  if (surface->pixels != nullptr) {
    std::memcpy(surface->pixels, pixels.data(), pixels.size());
  } else {
    throw Exception("Surface pixels not accessible");
  }
}

auto Surface::getPixel(uint32_t x, uint32_t y) const -> Color {
  auto* surface = _impl->_sdlSurface;
  
  if (x >= static_cast<uint32_t>(surface->w) || y >= static_cast<uint32_t>(surface->h)) {
    throw Exception("Pixel coordinates out of bounds");
  }

  if (surface->pixels == nullptr) {
    throw Exception("Surface pixels not accessible");
  }

  auto* pixels = static_cast<uint32_t*>(surface->pixels);
  uint32_t pixel = pixels[static_cast<size_t>(y) * static_cast<size_t>(surface->w) + static_cast<size_t>(x)];
  
  uint8_t r = static_cast<uint8_t>((pixel >> 24) & 0xFF);
  uint8_t g = static_cast<uint8_t>((pixel >> 16) & 0xFF);
  uint8_t b = static_cast<uint8_t>((pixel >> 8) & 0xFF);
  uint8_t a = static_cast<uint8_t>(pixel & 0xFF);
  
  return Color{r, g, b, a};
}

void Surface::setPixel(uint32_t x, uint32_t y, const Color& color) {
  auto* surface = _impl->_sdlSurface;
  
  if (x >= static_cast<uint32_t>(surface->w) || y >= static_cast<uint32_t>(surface->h)) {
    throw Exception("Pixel coordinates out of bounds");
  }

  if (surface->pixels == nullptr) {
    throw Exception("Surface pixels not accessible");
  }

  auto* pixels = static_cast<uint32_t*>(surface->pixels);
  uint32_t pixel = (static_cast<uint32_t>(color.getRed()) << 24) |
                   (static_cast<uint32_t>(color.getGreen()) << 16) |
                   (static_cast<uint32_t>(color.getBlue()) << 8) |
                   static_cast<uint32_t>(color.getAlpha());
  
  pixels[static_cast<size_t>(y) * static_cast<size_t>(surface->w) + static_cast<size_t>(x)] = pixel;
}

void Surface::fill(const Color& color) {
  auto* surface = _impl->_sdlSurface;
  // For RGBA8888 format: RGBA byte order in memory
  uint32_t pixel = (static_cast<uint32_t>(color.getRed()) << 24) |
                   (static_cast<uint32_t>(color.getGreen()) << 16) |
                   (static_cast<uint32_t>(color.getBlue()) << 8) |
                   static_cast<uint32_t>(color.getAlpha());

  if (!SDL_FillSurfaceRect(surface, nullptr, pixel)) {
    throw Exception("SDL_FillSurfaceRect");
  }
}

void Surface::fillRect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const Color& color) {
  auto* surface = _impl->_sdlSurface;
  
  SDL_Rect rect;
  rect.x = static_cast<int>(x);
  rect.y = static_cast<int>(y);
  rect.w = static_cast<int>(width);
  rect.h = static_cast<int>(height);

  // For RGBA8888 format: RGBA byte order in memory
  uint32_t pixel = (static_cast<uint32_t>(color.getRed()) << 24) |
                   (static_cast<uint32_t>(color.getGreen()) << 16) |
                   (static_cast<uint32_t>(color.getBlue()) << 8) |
                   static_cast<uint32_t>(color.getAlpha());

  if (!SDL_FillSurfaceRect(surface, &rect, pixel)) {
    throw Exception("SDL_FillSurfaceRect");
  }
}

void Surface::blit(const Surface& source, uint32_t destX, uint32_t destY) {
  SDL_Rect destRect;
  destRect.x = static_cast<int>(destX);
  destRect.y = static_cast<int>(destY);
  destRect.w = source._impl->_sdlSurface->w;
  destRect.h = source._impl->_sdlSurface->h;

  if (!SDL_BlitSurface(source._impl->_sdlSurface, nullptr, 
                       _impl->_sdlSurface, &destRect)) {
    throw Exception("SDL_BlitSurface");
  }
}

void Surface::blit(const Surface& source, uint32_t srcX, uint32_t srcY, 
                   uint32_t srcWidth, uint32_t srcHeight, 
                   uint32_t destX, uint32_t destY) {
  SDL_Rect srcRect;
  srcRect.x = static_cast<int>(srcX);
  srcRect.y = static_cast<int>(srcY);
  srcRect.w = static_cast<int>(srcWidth);
  srcRect.h = static_cast<int>(srcHeight);

  SDL_Rect destRect;
  destRect.x = static_cast<int>(destX);
  destRect.y = static_cast<int>(destY);
  destRect.w = static_cast<int>(srcWidth);
  destRect.h = static_cast<int>(srcHeight);

  if (!SDL_BlitSurface(source._impl->_sdlSurface, &srcRect, 
                       _impl->_sdlSurface, &destRect)) {
    throw Exception("SDL_BlitSurface");
  }
}

void Surface::save(const std::filesystem::path& filePath) const {
  const std::string filePathStr = filePath.string();
  const std::string extension = filePath.extension().string();
  
  if (extension == ".bmp" || extension == ".BMP") {
    if (!SDL_SaveBMP(_impl->_sdlSurface, filePathStr.c_str())) {
      throw Exception("SDL_SaveBMP failed: " + filePathStr);
    }
  } else if (extension == ".png" || extension == ".PNG") {
    if (!IMG_SavePNG(_impl->_sdlSurface, filePathStr.c_str())) {
      throw Exception("IMG_SavePNG failed: " + filePathStr);
    }
  } else if (extension == ".jpg" || extension == ".jpeg" || extension == ".JPG" || extension == ".JPEG") {
    if (!IMG_SaveJPG(_impl->_sdlSurface, filePathStr.c_str(), 90)) {
      throw Exception("IMG_SaveJPG failed: " + filePathStr);
    }
  } else {
    throw Exception("Unsupported image format: " + extension);
  }
}

Surface::Surface() : _impl(std::make_unique<SurfaceImpl>()) {};

auto Surface::getImpl() const -> const SurfaceImpl& {
  return *_impl;
}

}  // namespace sdl
