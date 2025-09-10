#include <functional>
#include <ranges>

#include "snake_ui.h"

namespace snake {

void SnakeUI::_drawBrick(const float recWidth, const float recHeight, const uint16_t x, const uint16_t y) const {
  _renderer->fillRect({
    (x * recWidth) + kCellBorderThickness,
    (y * recHeight) + kCellBorderThickness,
    recWidth - kCellBorderThickness,
    recHeight - kCellBorderThickness
  });
}

void SnakeUI::_renderWalls(const float recWidth, const float recHeight) const {
  _renderer->setDrawColour(kWallColor);
  
  auto drawSizedBrick = std::bind_front(&SnakeUI::_drawBrick, this, recWidth, recHeight);
  for(const uint16_t& x : std::ranges::views::iota(static_cast<uint16_t>(0), kGridWidth)) {
    drawSizedBrick(x, 0);
    drawSizedBrick(x, kGridHeight - 1);
  }
  
  for(const uint16_t& y : std::ranges::views::iota(static_cast<uint16_t>(0), kGridWidth)) {
    drawSizedBrick(0, y);
    drawSizedBrick(kGridWidth - 1, y);
  }
}

void SnakeUI::_renderSnake(const float recWidth, const float recHeight, const SnakeBody& snakeBody) const {
  _renderer->setDrawColour(kSnakeColor);
  for(const auto& p : snakeBody) {
    _drawBrick(recWidth, recHeight, p.first, p.second);
  }
}

void SnakeUI::render([[maybe_unused]] const GameState& state) {
  const auto size = _window->getSize();

  const auto recWidth = static_cast<float>(size.getWidth()) / static_cast<float>(kGridWidth);
  const auto recHeight = static_cast<float>(size.getHeight()) / static_cast<float>(kGridHeight);

  _renderer->setDrawColour(sdl::NamedColor::kBlack);
  _renderer->clear();

  _renderWalls(recWidth, recHeight);
  _renderSnake(recWidth, recHeight, state.getSnakeBody());

  _renderer->present();
}

}
