#include <algorithm>
#include <tuple>
#include <exception>
#include <iostream>
#include <memory>
#include <string>

#include <sdl/application.h>
#include <vector>
#include <sdl/event.h>
#include <sdl/event_router.h>
#include <sdl/message_box.h>
#include <sdl/sdl.h>
#include <sdl/user_event.h>

#include "events.h"
#include "game_constants.h"
#include "tictactoe.h"
#include "tictactoe_ui.h"

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

  auto setupEventHandlers() -> void {
    auto eventRouter = getEventRouter();

    _eventRegistrations.push_back(eventRouter->registerEventHandler<ClickEvent>(
      [this](const ClickEvent& clickEvent) -> void {
        _ticTacToe->play(clickEvent.x, clickEvent.y);
      }));

    _eventRegistrations.push_back(eventRouter->registerEventHandler<PlayerOTurnEvent>(
      []([[maybe_unused]] const PlayerOTurnEvent&) -> void {
        std::cout << "O's Turn\n";
      }));
    
      _eventRegistrations.push_back(eventRouter->registerEventHandler<PlayerXTurnEvent>(
      []([[maybe_unused]] const PlayerXTurnEvent&) -> void {
        std::cout << "X's Turn\n";
      }));

    _eventRegistrations.push_back(eventRouter->registerEventHandler<GameCompletedEvent>(
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

        // work around Windows SDK's #define of MessageBox
        // TODO: find an alternative name for the MessageBox class
        #ifdef WIN32
          #undef MessageBox
        #endif
        MessageBox("Game Over!", message).addButton("OK").show();

        getEventBus()->publish(std::make_unique<StartNewGameEvent>());
      }));
    
    _eventRegistrations.push_back(eventRouter->registerEventHandler<StartNewGameEvent>(
      [this]([[maybe_unused]]const StartNewGameEvent& sNGE) {
        _ticTacToe->reset();
      }
    ));
  }

  std::shared_ptr<TicTacToe> _ticTacToe;
  std::shared_ptr<TicTacToeUI> _ticTacToeUI;
  std::vector<EventRegistration> _eventRegistrations;
};

REGISTER_APPLICATION(TicTacToeApp)
