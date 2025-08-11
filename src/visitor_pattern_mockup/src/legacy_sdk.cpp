#include "legacy_sdk.h"

#include <iostream>

// NOLINTBEGIN

std::deque<OLD_Event> oldEvents = {
  { .user   = { OLD_USEREVENT, 1 }},
  { .system = { OLD_SYSTEMEVENT, 2 }},
  { .user   = { OLD_USEREVENT, 3 }},
  { .system = { OLD_SYSTEMEVENT, 4 }},
};

OLD_Event* waitForOldEvent() {
  if(std::size(oldEvents) == 0) return nullptr;
  OLD_Event *t = new OLD_Event(std::move(oldEvents.back()));
  oldEvents.pop_back();
  return t;
}

void pushOldEvent(OLD_Event* event) {
  oldEvents.push_back(*event);
}

//NOLINTEND
