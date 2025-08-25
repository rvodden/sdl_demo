#include "tictactoe_ui.h"

#include <event.h>

#include <chrono>
#include <ranges>

TicTacToeUI::TicTacToeUI(std::shared_ptr<sdlpp::EventBus> bus,
                         std::shared_ptr<sdlpp::tools::EventRouter> router)
    : _eventBus(std::move(bus)),
      _eventRouter(std::move(router)) {
  _spriteSheet->setTextureBlendMode(sdlpp::Texture::kBlend);
  _buttons.reserve(static_cast<std::size_t>(kCellColumns * kCellRows));
  for (uint8_t x :
       std::ranges::iota_view<uint8_t, uint8_t>{0, kCellColumns}) {
    for (uint8_t y : std::ranges::iota_view<uint8_t, uint8_t>{0, kCellRows}) {
      float buttonX = (static_cast<float>(x) * kCellWidth) + 1;
      float buttonY = (static_cast<float>(y) * kCellHeight) + 1;
      auto button = std::make_unique<sdlpp::tools::Button>(
          _eventRouter,
          sdlpp::Rectangle<float>{buttonX, buttonY, kCellWidth, kCellHeight});
      button->registerEventHandler(
          [this, x, y](const sdlpp::MouseButtonEvent& mouseButtonEvent) -> void {
            auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now().time_since_epoch());
            if (mouseButtonEvent.down) {
              _eventBus->publish(
                  std::make_unique<ClickEvent>(now, 0, x, y));
            }
          });
      _buttons.emplace_back(std::move(button));
    }
  }
}

void TicTacToeUI::render(const std::shared_ptr<TicTacToe>& ticTacToe) {
  _renderer->setRenderDrawColour(sdlpp::NamedColor::kWhite);
  _renderer->clear();

  _spriteRenderer->render(_board, 0, 0);

  for (uint8_t x :
       std::ranges::iota_view<uint8_t, uint8_t>{0, kCellColumns}) {
    for (uint8_t y : std::ranges::iota_view<uint8_t, uint8_t>{0, kCellRows}) {
      const auto cellState = ticTacToe->getCellState(x, y);
      if (!cellState) {
        continue;
      }
      _spriteRenderer->render(
          cellState.value() == TicTacToe::Player::kO ? _letterO : _letterX,
          static_cast<float>(x) * kCellWidth,
          static_cast<float>(y) * kCellHeight);
    }
  }
  _renderer->present();
}