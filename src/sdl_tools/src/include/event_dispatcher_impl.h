#ifndef SDL_TOOLS_EVENT_DISPATCHER_IMPL_H
#define SDL_TOOLS_EVENT_DISPATCHER_IMPL_H

#include <atomic>
#include <forward_list>
#include <memory>

#include "event_dispatcher.h"

namespace sdl::tools {

class EventDispatcher;
class EventDispatcherImpl;

class DefaultQuitEventHandler : public sdl::EventHandler<QuitEvent>,
                                public sdl::BaseEventHandler {
 public:
  DefaultQuitEventHandler(EventDispatcherImpl& eventDispatcherImpl)
      : _eventDispatcherImpl(eventDispatcherImpl) {};
  void handle([[maybe_unused]] const QuitEvent& quitEvent) override;

 private:
  std::reference_wrapper<EventDispatcherImpl> _eventDispatcherImpl;
};

class EventDispatcherImpl {
  friend EventDispatcher;

 public:
  EventDispatcherImpl(std::shared_ptr<EventProducer> eventProducer);
  EventDispatcherImpl(const EventDispatcherImpl& impl) = delete;
  EventDispatcherImpl(EventDispatcherImpl&& impl) noexcept = default;

  ~EventDispatcherImpl();

  auto operator=(const EventDispatcherImpl& impl) -> EventDispatcherImpl& = delete;
  auto operator=(EventDispatcherImpl&& impl) noexcept -> EventDispatcherImpl& = default;
  void quit();

 private:
  std::shared_ptr<BaseEventProducer> _eventProducer;
  std::forward_list<std::reference_wrapper<sdl::BaseEventHandler>>
      _eventHandlers;
  std::atomic_bool quitFlag{false};
  DefaultQuitEventHandler defaultQuitEventHandler{*this};
};

}  // namespace sdl::tools

#endif
