#include <gtest/gtest.h>

#include <legacy_sdk.h>

#include <visitor_pattern_legacy_sdk.h>
#include <visitor_pattern_legacy_sdk_impl.h>


class MockCustomEvent: public CustomEventTemplate<MockCustomEvent> {
  FRIEND_TEST(TestConversion, TestCustomEvents);
  public:
    MockCustomEvent(uint16_t customEventNumber, std::string message):
      BaseEvent(createConvertable(this)),
      customEventNumber { customEventNumber },
      message { message }
    { };

    uint16_t customEventNumber;
    std::string message;
};

TEST(TestConversion, TestCustomEvents) {
  oldEvents.clear();
  MockCustomEvent mockCustomEvent {13, "This is bad luck"};
  pushEvent(mockCustomEvent);

  OLD_Event* oldEvent = waitForOldEvent();
}

TEST(TestConversion, TestPushingUserEvent) {
  oldEvents.clear();
  UserEvent inputEvent {};
  inputEvent.userNumber = 13;
  pushEvent(inputEvent);
  
  BaseEvent &outputEvent = getEvent();

  UserEvent &outputUserEvent = dynamic_cast<UserEvent&>(outputEvent);
  ASSERT_EQ(inputEvent.userNumber, outputUserEvent.userNumber);

  delete &outputEvent;
}

TEST(TestConversion, TestPushingSystemEvent) {
  oldEvents.clear();
  SystemEvent inputEvent {};
  inputEvent.systemNumber = 13;
  pushEvent(inputEvent);
  
  BaseEvent &outputEvent = getEvent();

  SystemEvent &outputSystemEvent = dynamic_cast<SystemEvent&>(outputEvent);
  ASSERT_EQ(inputEvent.systemNumber, outputSystemEvent.systemNumber);

  delete &outputEvent;
}
