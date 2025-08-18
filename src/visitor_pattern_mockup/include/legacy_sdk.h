#ifndef LEGACY_SDK_H
#define LEGACY_SDK_H

#include <cinttypes>
#include <deque>

// LEGACY SDK
// NOLINTBEGIN

enum OLD_EventType { OLD_USEREVENT,
                     OLD_SYSTEMEVENT,
                     OLD_CUSTOMEVENT };

struct OLD_UserEvent {
  OLD_EventType eventType;
  uint16_t userNumber;
};

struct OLD_SystemEvent {
  OLD_EventType eventType;
  uint16_t systemNumber;
};

struct OLD_CustomEvent {
  OLD_EventType eventType;
  uint16_t customEventNumber;
  void* payload;
};

union OLD_Event {
  OLD_EventType eventType;
  OLD_UserEvent user;
  OLD_SystemEvent system;
  OLD_CustomEvent custom;
};

extern std::deque<OLD_Event> oldEvents;

OLD_Event* waitForOldEvent();
void pushOldEvent(OLD_Event* oldEvent);

// NOLINTEND

#endif
