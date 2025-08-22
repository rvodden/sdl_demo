#include "tictactoe.h"
#include "tictactoe_ui.h"
#include <event_router.h>
#include <sdl.h>

#include <exception>
#include <iostream>
#include <memory>

using namespace sdlpp;
using namespace sdlpp::tools;

auto main() -> int {
  try {
    SDL sdl;
    sdl.initSubSystem(SDL::kVideo);

    auto eventBus = std::make_shared<EventBus>();
    auto eventRouter = std::make_shared<EventRouter>(std::static_pointer_cast<BaseEventBus>(eventBus));

    auto ticTacToe = std::make_shared<TicTacToe>(eventBus);
    auto ticTacToeUI =
        std::make_shared<TicTacToeUI>(eventBus, eventRouter);

    ticTacToeUI->render(ticTacToe);

    eventRouter->registerEventHandler<ClickEvent>(
      [=]([[maybe_unused]] const ClickEvent& clickEvent) -> void {
        ticTacToeUI->render(ticTacToe);
      });

    eventRouter->registerEventHandler<GameCompletedEvent>(
      [=](const GameCompletedEvent& gCE) -> void {
        using enum GameState;
        switch(gCE.getState()) {
          case PlayerOWins:
            std::cout << "O Wins!\n";
            break;
          case PlayerXWins:
            std::cout << "X Wins!\n";
            break;
          case Draw:
            std::cout << "It's a draw!\n";
            break;
          case Playing:
            std::cout << "The game isn't over!\n";
            break;
        }
      });
    
    eventRouter->registerEventHandler<ClickEvent>(
      [=](const ClickEvent& clickEvent) -> void {
        ticTacToe->play(clickEvent.x, clickEvent.y);
      });


    eventRouter->run();
  } catch (std::exception& e) {
    std::cout << "Some kind of error happened!\n";
    std::cout << e.what() << "\n";
    return -1;
  }
  return 0;
}
