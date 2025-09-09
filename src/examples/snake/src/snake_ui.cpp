#include <ranges>

#include "snake_ui.h"

namespace snake {

void SnakeUI::render([[maybe_unused]] const GameState& state) {
  const auto size = _window->getSize();

  const auto recWidth = static_cast<float>(size.getWidth()) / static_cast<float>(kGridWidth);
  const auto recHeight = static_cast<float>(size.getHeight()) / static_cast<float>(kGridHeight);

  _renderer->setDrawColour(sdl::NamedColor::kBlack);
  _renderer->clear();

  _renderer->setDrawColour(kWallColor);
  for(const uint16_t& x : std::ranges::views::iota(static_cast<uint16_t>(0), kGridWidth)) {
    _renderer->fillRect({
      (x * recWidth) + kCellBorderThickness,
      0 + kCellBorderThickness,
      recWidth - kCellBorderThickness,
      recHeight - kCellBorderThickness});

    _renderer->fillRect({
      (x * recWidth) + kCellBorderThickness,
      static_cast<float>(size.getHeight()) - recHeight + kCellBorderThickness,
      recWidth - kCellBorderThickness,
      recHeight - kCellBorderThickness
    });
  }
  
  for(const uint16_t& y : std::ranges::views::iota(static_cast<uint16_t>(0), kGridWidth)) {
    _renderer->fillRect({ 
      static_cast<float>(size.getWidth()) - recWidth + kCellBorderThickness,
      (y * recHeight) + kCellBorderThickness,
      recWidth - kCellBorderThickness,
      recHeight - kCellBorderThickness });

    _renderer->fillRect({ 
      0 + kCellBorderThickness,
      (y * recHeight) + kCellBorderThickness,
      recWidth - kCellBorderThickness,
      recHeight - kCellBorderThickness });
  }
  

  _renderer->present();
}

}
