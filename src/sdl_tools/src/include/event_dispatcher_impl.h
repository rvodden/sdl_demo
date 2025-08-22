#ifndef SDL_TOOLS_EVENT_DISPATCHER_IMPL_H
#define SDL_TOOLS_EVENT_DISPATCHER_IMPL_H

#include <atomic>
#include <forward_list>
#include <memory>

#include "event_dispatcher.h"

namespace sdlpp::tools {

class EventDispatcher;
class EventDispatcherImpl;

class DefaultQuitEventHandler : public sdlpp::EventHandler<QuitEvent>,
                                public sdlpp::BaseEventHandler {
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
  EventDispatcherImpl(EventDispatcherImpl&& impl) noexcept = delete;

  ~EventDispatcherImpl();

  auto operator=(const EventDispatcherImpl& impl) -> EventDispatcherImpl& = delete;
  auto operator=(EventDispatcherImpl&& impl) noexcept -> EventDispatcherImpl& = delete;
  void quit();

 private:
  std::shared_ptr<BaseEventProducer> _eventProducer;
  std::forward_list<std::reference_wrapper<sdlpp::BaseEventHandler>>
      _eventHandlers;
  std::atomic_bool quitFlag{false};
  DefaultQuitEventHandler defaultQuitEventHandler{*this};
};

}  // namespace sdlpp::tools

#endif
