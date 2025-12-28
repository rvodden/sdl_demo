#include <chrono>
#include <memory>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "mocks/mock_event_bus.h"
#include <sdl/event.h>
#include <sdl/sdl.h>
#include <sdl/user_event.h>

using namespace sdl;
using sdl::test::MockEventBus;

// Test event types for isolated testing
class TestEvent : public Event {
public:
    explicit TestEvent(int value) : Event(std::chrono::milliseconds(100)), testValue(value) {}

    void handle(BaseEventHandler& baseEventHandler) override {
        castHandler(*this, baseEventHandler);
    }

    int testValue;
};

class AnotherTestEvent : public Event {
public:
    explicit AnotherTestEvent(std::string data) : Event(std::chrono::milliseconds(200)), testData(std::move(data)) {}

    void handle(BaseEventHandler& baseEventHandler) override {
        castHandler(*this, baseEventHandler);
    }

    std::string testData;
};

// Custom user event for testing
class CustomTestEvent : public CustomUserEvent<CustomTestEvent> {
public:
    explicit CustomTestEvent(int data) : customData(data) {}
    int customData;
};

// Test event handlers
class TestEventHandler : public EventHandler<TestEvent>, public BaseEventHandler {
public:
    void handle(const TestEvent& event) override {
        handledEvents.push_back(event.testValue);
        callCount++;
    }

    std::vector<int> handledEvents;
    int callCount = 0;
};

class AnotherTestEventHandler : public EventHandler<AnotherTestEvent>, public BaseEventHandler {
public:
    void handle(const AnotherTestEvent& event) override {
        handledData.push_back(event.testData);
        callCount++;
    }

    std::vector<std::string> handledData;
    int callCount = 0;
};

class CustomTestEventHandler : public EventHandler<CustomTestEvent>, public BaseEventHandler {
public:
    void handle(const CustomTestEvent& event) override {
        handledCustomData.push_back(event.customData);
        callCount++;
    }

    std::vector<int> handledCustomData;
    int callCount = 0;
};

class MultiEventHandler : public EventHandler<TestEvent>, public EventHandler<AnotherTestEvent>, public BaseEventHandler {
public:
    void handle(const TestEvent& event) override {
        testEventCalls++;
        testValues.push_back(event.testValue);
    }

    void handle(const AnotherTestEvent& event) override {
        anotherEventCalls++;
        anotherValues.push_back(event.testData);
    }

    int testEventCalls = 0;
    int anotherEventCalls = 0;
    std::vector<int> testValues;
    std::vector<std::string> anotherValues;
};

// BaseEvent Tests
TEST(BaseEventTest, DefaultConstructionAndPolymorphism) {
    auto testEvent = std::make_unique<TestEvent>(42);
    
    // Test polymorphic behavior
    BaseEvent* basePtr = testEvent.get();
    EXPECT_NE(basePtr, nullptr);
    
    // Test that polymorphic call works
    TestEventHandler handler;
    EXPECT_NO_THROW(basePtr->handle(handler));
    EXPECT_EQ(handler.callCount, 1);
    EXPECT_EQ(handler.handledEvents[0], 42);
}

TEST(BaseEventTest, MoveConstructorAndAssignment) {
    TestEvent originalEvent(123);
    
    // Test move constructor
    TestEvent movedEvent(std::move(originalEvent));
    EXPECT_EQ(movedEvent.testValue, 123);
    
    // Test move assignment
    TestEvent anotherEvent(456);
    anotherEvent = TestEvent(789);
    EXPECT_EQ(anotherEvent.testValue, 789);
}

TEST(BaseEventTest, CopyConstructorAndAssignment) {
    TestEvent originalEvent(999);
    
    // Test copy constructor
    TestEvent copiedEvent(originalEvent);
    EXPECT_EQ(copiedEvent.testValue, 999);
    EXPECT_EQ(originalEvent.testValue, 999); // Original should be unchanged
    
    // Test copy assignment
    TestEvent assignedEvent(111);
    assignedEvent = originalEvent;
    EXPECT_EQ(assignedEvent.testValue, 999);
    EXPECT_EQ(originalEvent.testValue, 999); // Original should be unchanged
}

// Event class tests
TEST(EventTest, TimestampInitialization) {
    auto timestamp = std::chrono::milliseconds(1234);
    Event event(timestamp);
    
    EXPECT_EQ(event.timestamp, timestamp);
}

TEST(EventTest, PolymorphicHandling) {
    TestEvent testEvent(555);
    TestEventHandler handler;
    
    // Cast to base Event class and ensure handling still works
    Event& eventRef = testEvent;
    eventRef.handle(handler);
    
    EXPECT_EQ(handler.callCount, 1);
    EXPECT_EQ(handler.handledEvents[0], 555);
}

// QuitEvent Tests
TEST(QuitEventTest, BasicFunctionality) {
    auto timestamp = std::chrono::milliseconds(5000);
    QuitEvent quitEvent(timestamp);
    
    EXPECT_EQ(quitEvent.timestamp, timestamp);
    
    // QuitEvents typically don't have specific handlers, but test polymorphic handling
    TestEventHandler handler; // Won't handle QuitEvent, but shouldn't crash
    EXPECT_NO_THROW(quitEvent.handle(handler));
    EXPECT_EQ(handler.callCount, 0); // Should not handle QuitEvent
}

TEST(QuitEventTest, PolymorphicUsage) {
    auto quitEvent = std::make_unique<QuitEvent>(std::chrono::milliseconds(1000));
    
    // Test through BaseEvent interface
    BaseEvent* basePtr = quitEvent.get();
    EXPECT_NE(basePtr, nullptr);
    
    TestEventHandler handler;
    EXPECT_NO_THROW(basePtr->handle(handler));
}

// MouseEvent Tests
TEST(MouseEventTest, BasicConstruction) {
    auto timestamp = std::chrono::milliseconds(2000);
    uint32_t windowId = 12345;
    uint32_t mouseId = 1;
    
    MouseEvent mouseEvent(timestamp, windowId, mouseId);
    
    EXPECT_EQ(mouseEvent.timestamp, timestamp);
    EXPECT_EQ(mouseEvent.windowId, windowId);
    EXPECT_EQ(mouseEvent.which, mouseId);
}

TEST(MouseEventTest, PolymorphicHandling) {
    MouseEvent mouseEvent(std::chrono::milliseconds(1500), 100, 1);
    TestEventHandler handler; // Won't handle MouseEvent specifically
    
    EXPECT_NO_THROW(mouseEvent.handle(handler));
    EXPECT_EQ(handler.callCount, 0); // Should not handle MouseEvent
}

// MousePositionEvent Tests
TEST(MousePositionEventTest, BasicConstruction) {
    auto timestamp = std::chrono::milliseconds(3000);
    uint32_t windowId = 54321;
    uint32_t mouseId = 2;
    float x = 123.5f;
    float y = 456.7f;
    
    MousePositionEvent posEvent(timestamp, windowId, mouseId, x, y);
    
    EXPECT_EQ(posEvent.timestamp, timestamp);
    EXPECT_EQ(posEvent.windowId, windowId);
    EXPECT_EQ(posEvent.which, mouseId);
    EXPECT_FLOAT_EQ(posEvent.x, x);
    EXPECT_FLOAT_EQ(posEvent.y, y);
}

// MouseButtonEvent Tests
TEST(MouseButtonEventTest, BasicConstruction) {
    auto timestamp = std::chrono::milliseconds(4000);
    uint32_t windowId = 11111;
    uint32_t mouseId = 3;
    float x = 200.0f;
    float y = 300.0f;
    auto button = MouseButtonEvent::Button::kLeft;
    bool down = true;
    uint8_t clicks = 2;
    
    MouseButtonEvent buttonEvent(timestamp, windowId, mouseId, x, y, button, down, clicks);
    
    EXPECT_EQ(buttonEvent.timestamp, timestamp);
    EXPECT_EQ(buttonEvent.windowId, windowId);
    EXPECT_EQ(buttonEvent.which, mouseId);
    EXPECT_FLOAT_EQ(buttonEvent.x, x);
    EXPECT_FLOAT_EQ(buttonEvent.y, y);
    EXPECT_EQ(buttonEvent.button, button);
    EXPECT_EQ(buttonEvent.down, down);
    EXPECT_EQ(buttonEvent.clicks, clicks);
}

TEST(MouseButtonEventTest, AllButtonTypes) {
    auto timestamp = std::chrono::milliseconds(0);
    
    // Test all button types
    std::vector<MouseButtonEvent::Button> buttons = {
        MouseButtonEvent::Button::kLeft,
        MouseButtonEvent::Button::kMiddle,
        MouseButtonEvent::Button::kRight,
        MouseButtonEvent::Button::kX1,
        MouseButtonEvent::Button::kX2
    };
    
    for (auto button : buttons) {
        MouseButtonEvent buttonEvent(timestamp, 0, 0, 0.0f, 0.0f, button, true, 1);
        EXPECT_EQ(buttonEvent.button, button);
    }
}

// BaseEventHandler Tests
TEST(BaseEventHandlerTest, DefaultConstruction) {
    BaseEventHandler handler;
    // Basic construction should work - this is mostly testing compilation
    EXPECT_NO_THROW(BaseEventHandler{});
}

// EventHandler template tests
TEST(EventHandlerTest, TypedHandlerFunctionality) {
    TestEventHandler handler;
    TestEvent event(777);
    
    handler.handle(event);
    
    EXPECT_EQ(handler.callCount, 1);
    EXPECT_EQ(handler.handledEvents.size(), 1);
    EXPECT_EQ(handler.handledEvents[0], 777);
}

TEST(EventHandlerTest, MultipleEventTypes) {
    MultiEventHandler handler;
    
    TestEvent testEvent(88);
    AnotherTestEvent anotherEvent("test");
    
    handler.handle(testEvent);
    handler.handle(anotherEvent);
    
    EXPECT_EQ(handler.testEventCalls, 1);
    EXPECT_EQ(handler.anotherEventCalls, 1);
    EXPECT_EQ(handler.testValues[0], 88);
    EXPECT_EQ(handler.anotherValues[0], "test");
}

// castHandler function tests
TEST(CastHandlerTest, SuccessfulCast) {
    TestEvent event(321);
    TestEventHandler handler;
    
    // Direct call to castHandler
    castHandler(event, handler);
    
    EXPECT_EQ(handler.callCount, 1);
    EXPECT_EQ(handler.handledEvents[0], 321);
}

TEST(CastHandlerTest, UnsuccessfulCast) {
    TestEvent event(654);
    AnotherTestEventHandler handler; // Wrong handler type
    
    // Should not crash, but should not call handler
    EXPECT_NO_THROW(castHandler(event, handler));
    EXPECT_EQ(handler.callCount, 0);
}

TEST(CastHandlerTest, PolymorphicCast) {
    TestEvent event(987);
    TestEventHandler specificHandler;
    
    // Cast to BaseEventHandler and ensure it still works
    BaseEventHandler& baseHandler = specificHandler;
    castHandler(event, baseHandler);
    
    EXPECT_EQ(specificHandler.callCount, 1);
    EXPECT_EQ(specificHandler.handledEvents[0], 987);
}

// FunctionEventHandler tests
TEST(FunctionEventHandlerTest, LambdaHandler) {
    std::vector<int> capturedValues;
    int callCount = 0;
    
    auto lambda = [&](const TestEvent& event) {
        capturedValues.push_back(event.testValue);
        callCount++;
    };
    
    FunctionEventHandler<TestEvent, decltype(lambda)> handler(std::move(lambda));
    TestEvent event(444);
    
    handler.handle(event);
    
    EXPECT_EQ(callCount, 1);
    EXPECT_EQ(capturedValues.size(), 1);
    EXPECT_EQ(capturedValues[0], 444);
}

TEST(FunctionEventHandlerTest, FunctionObjectHandler) {
    struct EventProcessor {
        std::vector<std::string>& results;
        explicit EventProcessor(std::vector<std::string>& r) : results(r) {}
        
        void operator()(const AnotherTestEvent& event) {
            results.push_back(event.testData);
        }
    };
    
    std::vector<std::string> results;
    EventProcessor processor(results);
    
    FunctionEventHandler<AnotherTestEvent, EventProcessor> handler(std::move(processor));
    AnotherTestEvent event("function_object");
    
    handler.handle(event);
    
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(results[0], "function_object");
}

// UnknownEventException tests
TEST(UnknownEventExceptionTest, Construction) {
    const std::string message = "Test exception message";
    
    UnknownEventException exception(message);
    EXPECT_EQ(exception.what(), message);
}

TEST(UnknownEventExceptionTest, ThrowAndCatch) {
    EXPECT_THROW({
        throw UnknownEventException("Test throw");
    }, UnknownEventException);
    
    try {
        throw UnknownEventException("Catch test");
    } catch (const UnknownEventException& e) {
        EXPECT_EQ(std::string(e.what()), "Catch test");
    } catch (...) {
        FAIL() << "Should have caught UnknownEventException";
    }
}

// UserEvent tests
TEST(UserEventTest, DefaultConstruction) {
    UserEvent userEvent;
    
    // Test default values and basic functionality
    EXPECT_EQ(userEvent.getWindowId(), 0);
    EXPECT_EQ(userEvent.getCode(), 0);
    EXPECT_EQ(userEvent.getData(), nullptr);
}

TEST(UserEventTest, ParameterizedConstruction) {
    auto timestamp = std::chrono::milliseconds(6000);
    uint32_t windowId = 99999;
    int32_t code = 42;
    int testData = 123;
    void* data = &testData;
    
    UserEvent userEvent(timestamp, windowId, code, data);
    
    EXPECT_EQ(userEvent.timestamp, timestamp);
    EXPECT_EQ(userEvent.getWindowId(), windowId);
    EXPECT_EQ(userEvent.getCode(), code);
    EXPECT_EQ(userEvent.getData(), data);
}

TEST(UserEventTest, SettersAndGetters) {
    UserEvent userEvent;
    
    userEvent.setWindowId(12345);
    userEvent.setCode(67);
    
    int testValue = 789;
    userEvent.setData(&testValue);
    
    EXPECT_EQ(userEvent.getWindowId(), 12345);
    EXPECT_EQ(userEvent.getCode(), 67);
    EXPECT_EQ(userEvent.getData(), &testValue);
}

TEST(UserEventTest, CopyConstructorAndAssignment) {
    UserEvent original;
    original.setWindowId(1111);
    original.setCode(2222);
    
    int data = 3333;
    original.setData(&data);
    
    // Copy constructor
    UserEvent copied(original);
    EXPECT_EQ(copied.getWindowId(), original.getWindowId());
    EXPECT_EQ(copied.getCode(), original.getCode());
    EXPECT_EQ(copied.getData(), original.getData());
    
    // Copy assignment
    UserEvent assigned;
    assigned = original;
    EXPECT_EQ(assigned.getWindowId(), original.getWindowId());
    EXPECT_EQ(assigned.getCode(), original.getCode());
    EXPECT_EQ(assigned.getData(), original.getData());
}

TEST(UserEventTest, MoveConstructorAndAssignment) {
    UserEvent original;
    original.setWindowId(4444);
    original.setCode(5555);
    
    uint32_t originalWindowId = original.getWindowId();
    int32_t originalCode = original.getCode();
    
    // Move constructor
    UserEvent moved(std::move(original));
    EXPECT_EQ(moved.getWindowId(), originalWindowId);
    EXPECT_EQ(moved.getCode(), originalCode);
    
    // Move assignment
    UserEvent assigned;
    assigned = std::move(moved);
    EXPECT_EQ(assigned.getWindowId(), originalWindowId);
    EXPECT_EQ(assigned.getCode(), originalCode);
}

// CustomUserEvent template tests
TEST(CustomUserEventTest, TypeSafetyAndHandling) {
    CustomTestEvent customEvent(555);
    CustomTestEventHandler handler;
    
    customEvent.handle(handler);
    
    EXPECT_EQ(handler.callCount, 1);
    EXPECT_EQ(handler.handledCustomData[0], 555);
}

TEST(CustomUserEventTest, PolymorphicUsage) {
    auto customEvent = std::make_unique<CustomTestEvent>(777);
    CustomTestEventHandler handler;
    
    // Test through BaseEvent interface
    BaseEvent* basePtr = customEvent.get();
    basePtr->handle(handler);
    
    EXPECT_EQ(handler.callCount, 1);
    EXPECT_EQ(handler.handledCustomData[0], 777);
}

TEST(CustomUserEventTest, EventTypeRegistration) {
    // Each CustomUserEvent type should get its own unique event type
    uint32_t type1 = CustomTestEvent::getEventType();
    uint32_t type2 = CustomTestEvent::getEventType(); // Should be same as type1
    
    EXPECT_EQ(type1, type2);
    EXPECT_GT(type1, 0); // Should be a valid event type
}

// BaseEventBus interface tests
TEST(BaseEventBusTest, MockEventBusBasicFunctionality) {
    MockEventBus mockBus;
    
    // Test adding and retrieving events
    mockBus.addEvent(std::make_unique<TestEvent>(123));
    mockBus.setShouldReturnEmptyOnPoll(false);
    
    auto polledEvent = mockBus.poll();
    EXPECT_TRUE(polledEvent.has_value());
    
    // Test user event publishing
    auto userEvent = std::make_unique<UserEvent>();
    mockBus.publish(std::move(userEvent));
    EXPECT_EQ(mockBus.getPublishedUserEventCount(), 1);
}

TEST(BaseEventBusTest, MockEventBusWaitFunctionality) {
    MockEventBus mockBus;
    
    mockBus.addEvent(std::make_unique<TestEvent>(456));
    
    auto event = mockBus.wait();
    EXPECT_NE(event, nullptr);
    
    // When no events, should return QuitEvent (default behavior)
    auto quitEvent = mockBus.wait();
    EXPECT_NE(quitEvent, nullptr);
}

TEST(BaseEventBusTest, RouteCallback) {
    MockEventBus mockBus;
    bool callbackCalled = false;
    std::unique_ptr<BaseEvent> receivedEvent;
    
    mockBus.setRouteCallback([&](std::unique_ptr<BaseEvent> event) {
        callbackCalled = true;
        receivedEvent = std::move(event);
    });
    
    // The callback is set, but MockEventBus doesn't automatically call it
    // This test just verifies the interface works
    EXPECT_FALSE(callbackCalled); // Callback not called until an event is routed
}

// Integration tests
TEST(EventSystemIntegrationTest, CompleteEventFlow) {
    // Test a complete flow: create events, handlers, and process them
    std::vector<int> processedTestValues;
    std::vector<std::string> processedStringValues;
    
    // Create handlers
    auto testHandler = std::make_unique<TestEventHandler>();
    auto anotherHandler = std::make_unique<AnotherTestEventHandler>();
    
    // Create events
    std::vector<std::unique_ptr<BaseEvent>> events;
    events.push_back(std::make_unique<TestEvent>(100));
    events.push_back(std::make_unique<AnotherTestEvent>("first"));
    events.push_back(std::make_unique<TestEvent>(200));
    events.push_back(std::make_unique<AnotherTestEvent>("second"));
    
    // Process events
    for (auto& event : events) {
        event->handle(*testHandler);
        event->handle(*anotherHandler);
    }
    
    // Verify results
    EXPECT_EQ(testHandler->callCount, 2);
    EXPECT_EQ(testHandler->handledEvents.size(), 2);
    EXPECT_EQ(testHandler->handledEvents[0], 100);
    EXPECT_EQ(testHandler->handledEvents[1], 200);
    
    EXPECT_EQ(anotherHandler->callCount, 2);
    EXPECT_EQ(anotherHandler->handledData.size(), 2);
    EXPECT_EQ(anotherHandler->handledData[0], "first");
    EXPECT_EQ(anotherHandler->handledData[1], "second");
}

TEST(EventSystemIntegrationTest, MixedHandlerTypes) {
    // Test mixing class-based and lambda-based handlers
    TestEventHandler classHandler;
    
    int lambdaCalls = 0;
    std::vector<int> lambdaValues;
    
    auto lambda = [&](const TestEvent& event) {
        lambdaCalls++;
        lambdaValues.push_back(event.testValue * 2);
    };
    
    FunctionEventHandler<TestEvent, decltype(lambda)> lambdaHandler(std::move(lambda));
    
    TestEvent event(50);
    
    // Handle with class handler
    event.handle(classHandler);
    
    // Handle with lambda handler
    event.handle(lambdaHandler);
    
    EXPECT_EQ(classHandler.callCount, 1);
    EXPECT_EQ(classHandler.handledEvents[0], 50);
    
    EXPECT_EQ(lambdaCalls, 1);
    EXPECT_EQ(lambdaValues[0], 100);
}

// KeyboardEvent Tests
TEST(KeyboardEventTest, BasicConstruction) {
    auto timestamp = std::chrono::milliseconds(5000);
    uint32_t windowId = 22222;
    uint32_t keyboardId = 1;
    auto scancode = sdl::ScanCode::kA;
    auto keycode = sdl::KeyCode::kA;
    bool down = true;
    bool repeat = false;
    auto keymod = sdl::KeyModifier::kLShift;
    
    KeyboardEvent keyEvent(timestamp, windowId, keyboardId, scancode, keycode, down, repeat, keymod);
    
    EXPECT_EQ(keyEvent.timestamp, timestamp);
    EXPECT_EQ(keyEvent.windowId, windowId);
    EXPECT_EQ(keyEvent.which, keyboardId);
    EXPECT_EQ(keyEvent.scancode, scancode);
    EXPECT_EQ(keyEvent.keycode, keycode);
    EXPECT_EQ(keyEvent.keymod, keymod);
    EXPECT_EQ(keyEvent.down, down);
    EXPECT_EQ(keyEvent.isRepeat, repeat);
}

TEST(KeyboardEventTest, GetKeyNameBasicKeys) {
    auto timestamp = std::chrono::milliseconds(0);
    
    // Test basic alphabetic keys
    KeyboardEvent keyA(timestamp, 0, 0, sdl::ScanCode::kA, sdl::KeyCode::kA, true, false, sdl::KeyModifier::kNone);
    EXPECT_EQ(keyA.getKeyName(), "A");
    
    KeyboardEvent keyZ(timestamp, 0, 0, sdl::ScanCode::kZ, sdl::KeyCode::kZ, true, false, sdl::KeyModifier::kNone);
    EXPECT_EQ(keyZ.getKeyName(), "Z");
    
    // Test numeric keys
    KeyboardEvent key1(timestamp, 0, 0, sdl::ScanCode::k1, sdl::KeyCode::k1, true, false, sdl::KeyModifier::kNone);
    EXPECT_EQ(key1.getKeyName(), "1");
    
    KeyboardEvent key0(timestamp, 0, 0, sdl::ScanCode::k0, sdl::KeyCode::k0, true, false, sdl::KeyModifier::kNone);
    EXPECT_EQ(key0.getKeyName(), "0");
}

TEST(KeyboardEventTest, GetKeyNameSpecialKeys) {
    auto timestamp = std::chrono::milliseconds(0);
    
    // Test special keys
    KeyboardEvent keySpace(timestamp, 0, 0, sdl::ScanCode::kSpace, sdl::KeyCode::kSpace, true, false, sdl::KeyModifier::kNone);
    EXPECT_EQ(keySpace.getKeyName(), "Space");
    
    KeyboardEvent keyEnter(timestamp, 0, 0, sdl::ScanCode::kReturn, sdl::KeyCode::kReturn, true, false, sdl::KeyModifier::kNone);
    EXPECT_EQ(keyEnter.getKeyName(), "Return");
    
    KeyboardEvent keyEscape(timestamp, 0, 0, sdl::ScanCode::kEscape, sdl::KeyCode::kEscape, true, false, sdl::KeyModifier::kNone);
    EXPECT_EQ(keyEscape.getKeyName(), "Escape");
    
    KeyboardEvent keyTab(timestamp, 0, 0, sdl::ScanCode::kTab, sdl::KeyCode::kTab, true, false, sdl::KeyModifier::kNone);
    EXPECT_EQ(keyTab.getKeyName(), "Tab");
}

TEST(KeyboardEventTest, GetKeyNameFunctionKeys) {
    auto timestamp = std::chrono::milliseconds(0);
    
    // Test function keys
    KeyboardEvent keyF1(timestamp, 0, 0, sdl::ScanCode::kF1, sdl::KeyCode::kF1, true, false, sdl::KeyModifier::kNone);
    EXPECT_EQ(keyF1.getKeyName(), "F1");
    
    KeyboardEvent keyF12(timestamp, 0, 0, sdl::ScanCode::kF12, sdl::KeyCode::kF12, true, false, sdl::KeyModifier::kNone);
    EXPECT_EQ(keyF12.getKeyName(), "F12");
}

TEST(KeyboardEventTest, GetKeyNameSymbols) {
    auto timestamp = std::chrono::milliseconds(0);
    
    // Test symbol keys
    KeyboardEvent keyMinus(timestamp, 0, 0, sdl::ScanCode::kMinus, sdl::KeyCode::kMinus, true, false, sdl::KeyModifier::kNone);
    EXPECT_EQ(keyMinus.getKeyName(), "-");
    
    KeyboardEvent keyEquals(timestamp, 0, 0, sdl::ScanCode::kEquals, sdl::KeyCode::kEquals, true, false, sdl::KeyModifier::kNone);
    EXPECT_EQ(keyEquals.getKeyName(), "=");
    
    KeyboardEvent keyLeftBracket(timestamp, 0, 0, sdl::ScanCode::kLeftBracket, sdl::KeyCode::kLeftBracket, true, false, sdl::KeyModifier::kNone);
    EXPECT_EQ(keyLeftBracket.getKeyName(), "[");
}

TEST(KeyboardEventTest, GetKeyNameArrowKeys) {
    auto timestamp = std::chrono::milliseconds(0);
    
    // Test arrow keys
    KeyboardEvent keyUp(timestamp, 0, 0, sdl::ScanCode::kUp, sdl::KeyCode::kUp, true, false, sdl::KeyModifier::kNone);
    EXPECT_EQ(keyUp.getKeyName(), "Up");
    
    KeyboardEvent keyDown(timestamp, 0, 0, sdl::ScanCode::kDown, sdl::KeyCode::kDown, true, false, sdl::KeyModifier::kNone);
    EXPECT_EQ(keyDown.getKeyName(), "Down");
    
    KeyboardEvent keyLeft(timestamp, 0, 0, sdl::ScanCode::kLeft, sdl::KeyCode::kLeft, true, false, sdl::KeyModifier::kNone);
    EXPECT_EQ(keyLeft.getKeyName(), "Left");
    
    KeyboardEvent keyRight(timestamp, 0, 0, sdl::ScanCode::kRight, sdl::KeyCode::kRight, true, false, sdl::KeyModifier::kNone);
    EXPECT_EQ(keyRight.getKeyName(), "Right");
}

TEST(KeyboardEventTest, GetKeyNameUnknownKey) {
    auto timestamp = std::chrono::milliseconds(0);
    
    // Test unknown key
    KeyboardEvent keyUnknown(timestamp, 0, 0, sdl::ScanCode::kUnknown, sdl::KeyCode::kUnknown, true, false, sdl::KeyModifier::kNone);
    std::string keyName = keyUnknown.getKeyName();
    // SDL_GetKeyName for SDLK_UNKNOWN returns an empty string, which our function converts to "Unknown"
    EXPECT_TRUE(keyName == "Unknown" || keyName == "");
}

TEST(KeyboardEventTest, KeyModifierEnumValues) {
    auto timestamp = std::chrono::milliseconds(0);
    
    // Test different key modifier combinations
    std::vector<sdl::KeyModifier> modifiers = {
        sdl::KeyModifier::kNone,
        sdl::KeyModifier::kLShift,
        sdl::KeyModifier::kRShift,
        sdl::KeyModifier::kLCtrl,
        sdl::KeyModifier::kRCtrl,
        sdl::KeyModifier::kLAlt,
        sdl::KeyModifier::kRAlt,
        sdl::KeyModifier::kLGui,
        sdl::KeyModifier::kRGui,
        sdl::KeyModifier::kCaps,
        sdl::KeyModifier::kNum,
        sdl::KeyModifier::kMode,
        sdl::KeyModifier::kScroll
    };
    
    for (auto modifier : modifiers) {
        KeyboardEvent keyEvent(timestamp, 0, 0, sdl::ScanCode::kA, sdl::KeyCode::kA, true, false, modifier);
        EXPECT_EQ(keyEvent.keymod, modifier);
    }
}

TEST(KeyboardEventTest, ScanCodeAndKeyCodeEnumValues) {
    auto timestamp = std::chrono::milliseconds(0);
    
    // Test that we can construct keyboard events with various scancode/keycode combinations
    std::vector<std::pair<sdl::ScanCode, sdl::KeyCode>> keyPairs = {
        {sdl::ScanCode::kA, sdl::KeyCode::kA},
        {sdl::ScanCode::kReturn, sdl::KeyCode::kReturn},
        {sdl::ScanCode::kSpace, sdl::KeyCode::kSpace},
        {sdl::ScanCode::kF1, sdl::KeyCode::kF1},
        {sdl::ScanCode::kKp1, sdl::KeyCode::kKp1},
        {sdl::ScanCode::kLeftBracket, sdl::KeyCode::kLeftBracket}
    };
    
    for (const auto& [scancode, keycode] : keyPairs) {
        KeyboardEvent keyEvent(timestamp, 0, 0, scancode, keycode, true, false, sdl::KeyModifier::kNone);
        EXPECT_EQ(keyEvent.scancode, scancode);
        EXPECT_EQ(keyEvent.keycode, keycode);
        
        // Verify getKeyName doesn't crash
        std::string keyName = keyEvent.getKeyName();
        EXPECT_FALSE(keyName.empty());
    }
}
