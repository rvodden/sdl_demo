#include "tictactoe.h"
#include "tictactoe_ui.h"
#include <application.h>
#include <event_router.h>
#include <message_box.h>
#include <sdl.h>

#include <exception>
#include <iostream>
#include <memory>

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
      
      ticTacToe_ = std::make_shared<TicTacToe>(eventBus);
      ticTacToeUI_ = std::make_shared<TicTacToeUI>(eventBus, eventRouter);

      setupEventHandlers();
      ticTacToeUI_->render(ticTacToe_);

      std::cout << "TicTacToe initialized successfully\n";
      return true;
    } catch (const std::exception& e) {
      std::cout << "Failed to initialize TicTacToe: " << e.what() << "\n";
      return false;
    }
  }

  auto iterate() -> bool override {
    // The event loop is handled by the framework
    // Game logic is driven by events
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
        ticTacToe_->play(clickEvent.x, clickEvent.y);
      });

    eventRouter->registerEventHandler<ClickEvent>(
      [this]([[maybe_unused]] const ClickEvent& clickEvent) -> void {
        ticTacToeUI_->render(ticTacToe_);
      });

    eventRouter->registerEventHandler<GameCompletedEvent>(
      [this](const GameCompletedEvent& gCE) -> void {
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
        ticTacToe_->reset();
        ticTacToeUI_->render(ticTacToe_);
      }
    );
  }

  std::shared_ptr<TicTacToe> ticTacToe_;
  std::shared_ptr<TicTacToeUI> ticTacToeUI_;
};

REGISTER_APPLICATION(TicTacToeApp)
