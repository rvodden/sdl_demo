#ifndef SDL_TOOLS_EVENT_ROUTER_IMPL_H
#define SDL_TOOLS_EVENT_ROUTER_IMPL_H

#include <atomic>
#include <vector>
#include <memory>

#include "event_router.h"

namespace sdlpp::tools {

class EventRouter;
class EventRouterImpl;

class DefaultQuitEventHandler : public sdlpp::EventHandler<QuitEvent>,
                                public sdlpp::BaseEventHandler {
 public:
  DefaultQuitEventHandler(EventRouterImpl& eventRouterImpl)
      : _eventRouterImpl(eventRouterImpl) {};
  void handle([[maybe_unused]] const QuitEvent& quitEvent) override;

 private:
  std::reference_wrapper<EventRouterImpl> _eventRouterImpl;
};

class EventRouterImpl {
  friend EventRouter;

 public:
  EventRouterImpl(std::shared_ptr<BaseEventBus> eventBus);
  EventRouterImpl(const EventRouterImpl& impl) = delete;
  EventRouterImpl(EventRouterImpl&& impl) noexcept = delete;

  ~EventRouterImpl();

  auto operator=(const EventRouterImpl& impl) -> EventRouterImpl& = delete;
  auto operator=(EventRouterImpl&& impl) noexcept -> EventRouterImpl& = delete;
  void quit();

 private:
  std::shared_ptr<BaseEventBus> _eventBus;
  std::vector<std::reference_wrapper<sdlpp::BaseEventHandler>>
      _eventHandlers;
  std::atomic_bool quitFlag{false};
  DefaultQuitEventHandler defaultQuitEventHandler{*this};
};

}  // namespace sdlpp::tools

#endif
