#include <algorithm>
#include <exception>
#include <iostream>
#include <memory>
#include <string>

#include "application.h"
#include "event.h"
#include "events.h"
#include "event_router.h"
#include "game_constants.h"
#include "tictactoe.h"
#include "tictactoe_ui.h"
#include "message_box.h"
#include "sdl.h"
#include "user_event.h"

using namespace sdl;
using namespace sdl::tools;

class TicTacToeApp : public BaseApplication {
public:
  auto init() -> bool override {
    try {
      auto& sdl = requestSDL();
      sdl.initSubSystem(SDL::kVideo);
      sdl.initSubSystem(SDL::kEvents);

      auto eventBus = getEventBus();
      auto eventRouter = getEventRouter();
      
      _ticTacToe = std::make_shared<TicTacToe>(eventBus);
      _ticTacToeUI = std::make_shared<TicTacToeUI>(eventBus, eventRouter);

      setupEventHandlers();

      std::cout << "TicTacToe initialized successfully\n";
      return true;
    } catch (const std::exception& e) {
      std::cout << "Failed to initialize TicTacToe: " << e.what() << "\n";
      return false;
    }
  }

  auto iterate() -> bool override {
    _ticTacToeUI->render(_ticTacToe);
    return true;
  }

  auto quit() -> void override {
    std::cout << "TicTacToe::quit() called\n";
  }

private:
  auto getEventBus() -> std::shared_ptr<BaseEventBus> {
    return ApplicationRunner::getInstance().getEventBus();
  }

  auto getEventRouter() -> std::shared_ptr<EventRouter> {
    return ApplicationRunner::getInstance().getEventRouter();
  }

  auto setupEventHandlers() -> void {
    auto eventRouter = getEventRouter();

    eventRouter->registerEventHandler<ClickEvent>(
      [this](const ClickEvent& clickEvent) -> void {
        _ticTacToe->play(clickEvent.x, clickEvent.y);
      });

    eventRouter->registerEventHandler<PlayerOTurnEvent>(
      []([[maybe_unused]] const PlayerOTurnEvent&) -> void {
        std::cout << "O's Turn\n";
      });
    
      eventRouter->registerEventHandler<PlayerXTurnEvent>(
      []([[maybe_unused]] const PlayerXTurnEvent&) -> void {
        std::cout << "X's Turn\n";
      });

    eventRouter->registerEventHandler<GameCompletedEvent>(
      [this](const GameCompletedEvent& gCE) -> void {
        //not sure why we need to reder here...
        _ticTacToeUI->render(_ticTacToe);
        
        using enum GameState;
        std::string message;
        switch(gCE.getState()) {
          case kPlayerOWins:
            message = "O Wins!\n";
            break;
          case kPlayerXWins:
            message = "X Wins!\n";
            break;
          case kDraw:
            message = "It's a draw!\n";
            break;
          case kPlaying:
            message = "The game isn't over!\n";
            break;
        }

        // TODO: Blocking call in event handler feels bad
        MessageBox("Game Over!", message).addButton("OK").show();

        getEventBus()->publish(std::make_unique<StartNewGameEvent>());
      });
    
    eventRouter->registerEventHandler<StartNewGameEvent>(
      [this]([[maybe_unused]]const StartNewGameEvent& sNGE) {
        _ticTacToe->reset();
      }
    );
  }

  std::shared_ptr<TicTacToe> _ticTacToe;
  std::shared_ptr<TicTacToeUI> _ticTacToeUI;
};

REGISTER_APPLICATION(TicTacToeApp)
