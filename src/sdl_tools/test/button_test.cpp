#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "mocks/mock_event_bus.h"
#include "sdl/button.h"
#include "sdl/event.h"
#include "sdl/event_router.h"
#include "sdl/rectangle.h"
#include "sdl/user_event.h"

using namespace sdl;
using namespace sdl::tools;
using sdl::test::MockEventBus;

// Test EventRouter wrapper that allows controlled event injection
class TestEventRouter {
public:
    TestEventRouter() : mockBus(std::make_shared<MockEventBus>()),
                        eventRouter(std::make_shared<EventRouter>(mockBus)) {}
    
    std::shared_ptr<EventRouter> getEventRouter() { return eventRouter; }
    std::shared_ptr<MockEventBus> getMockBus() { return mockBus; }
    
    // Inject single event and trigger one processing cycle
    void injectAndProcess(std::unique_ptr<BaseEvent> event) {
        mockBus->pushEvent(std::move(event));
        mockBus->injectQuitEvent(); // Ensure loop terminates
        eventRouter->run(); // Process the injected event
    }
    
    // Inject multiple events and process them all
    void injectAndProcessMultiple(std::vector<std::unique_ptr<BaseEvent>> events) {
        for (auto& event : events) {
            mockBus->pushEvent(std::move(event));
        }
        mockBus->injectQuitEvent(); // Ensure loop terminates
        eventRouter->run(); // Process all injected events
    }

private:
    std::shared_ptr<MockEventBus> mockBus;
    std::shared_ptr<EventRouter> eventRouter;
};

// Test fixture for Button tests
class ButtonTest : public ::testing::Test {
protected:
    void SetUp() override {
        testEventRouter = std::make_unique<TestEventRouter>();
        testRect = std::make_unique<sdl::Rectangle<float>>(10.0f, 10.0f, 100.0f, 50.0f);
    }

    void TearDown() override {
        button.reset();
        testRect.reset();
        testEventRouter.reset();
    }

    // Helper to create MouseButtonEvent for testing
    MouseButtonEvent createMouseEvent(float x, float y, 
                                     MouseButtonEvent::Button btn = MouseButtonEvent::Button::kLeft,
                                     bool down = true,
                                     uint8_t clicks = 1) {
        return MouseButtonEvent(
            std::chrono::milliseconds(0), // timestamp
            1, // windowId
            1, // mouseId
            x, y, btn, down, clicks
        );
    }

    std::unique_ptr<TestEventRouter> testEventRouter;
    std::unique_ptr<sdl::Rectangle<float>> testRect;
    std::unique_ptr<Button> button;
};

// Test event handler for verification
class TestButtonHandler {
public:
    void operator()(const MouseButtonEvent& event) {
        callCount++;
        lastEvent = event;
        receivedEvents.push_back(event);
    }
    
    int callCount = 0;
    MouseButtonEvent lastEvent{std::chrono::milliseconds(0), 0, 0, 0.0f, 0.0f, 
                              MouseButtonEvent::Button::kLeft, false, 1};
    std::vector<MouseButtonEvent> receivedEvents;
};

// Construction & Lifecycle Tests
TEST_F(ButtonTest, ConstructorWithValidParameters) {
    // Should not throw when constructed with valid EventRouter and rectangle
    EXPECT_NO_THROW(
        button = std::make_unique<Button>(testEventRouter->getEventRouter(), *testRect)
    );
    EXPECT_NE(button.get(), nullptr);
}

TEST_F(ButtonTest, ConstructorRegistersWithEventRouter) {
    // Button should register itself with EventRouter during construction
    // We verify this by testing that events get routed to the button
    button = std::make_unique<Button>(testEventRouter->getEventRouter(), *testRect);
    
    TestButtonHandler handler;
    button->registerEventHandler([&handler](const MouseButtonEvent& e) { handler(e); });
    
    // If button registered correctly, it should receive this event
    auto clickEvent = std::make_unique<MouseButtonEvent>(createMouseEvent(50.0f, 30.0f));
    testEventRouter->injectAndProcess(std::move(clickEvent));
    
    EXPECT_EQ(handler.callCount, 1);
}

TEST_F(ButtonTest, MoveConstructor) {
    // Create button and move it
    button = std::make_unique<Button>(testEventRouter->getEventRouter(), *testRect);
    Button movedButton = std::move(*button);
    
    // Moved button should be functional
    TestButtonHandler handler;
    EXPECT_NO_THROW(movedButton.registerEventHandler(
        [&handler](const MouseButtonEvent& e) { handler(e); }
    ));
}

TEST_F(ButtonTest, MoveAssignment) {
    // Create two buttons
    button = std::make_unique<Button>(testEventRouter->getEventRouter(), *testRect);
    sdl::Rectangle<float> otherRect(200.0f, 200.0f, 50.0f, 25.0f);
    Button otherButton(testEventRouter->getEventRouter(), otherRect);
    
    // Move assign
    otherButton = std::move(*button);
    
    // Moved-to button should be functional
    TestButtonHandler handler;
    EXPECT_NO_THROW(otherButton.registerEventHandler(
        [&handler](const MouseButtonEvent& e) { handler(e); }
    ));
}

// Event Handler Registration Tests
TEST_F(ButtonTest, RegisterSingleHandler) {
    button = std::make_unique<Button>(testEventRouter->getEventRouter(), *testRect);
    TestButtonHandler handler;
    
    EXPECT_NO_THROW(button->registerEventHandler(
        [&handler](const MouseButtonEvent& e) { handler(e); }
    ));
}

TEST_F(ButtonTest, RegisterMultipleHandlers) {
    button = std::make_unique<Button>(testEventRouter->getEventRouter(), *testRect);
    TestButtonHandler handler1, handler2, handler3;
    
    EXPECT_NO_THROW(button->registerEventHandler(
        [&handler1](const MouseButtonEvent& e) { handler1(e); }
    ));
    EXPECT_NO_THROW(button->registerEventHandler(
        [&handler2](const MouseButtonEvent& e) { handler2(e); }
    ));
    EXPECT_NO_THROW(button->registerEventHandler(
        [&handler3](const MouseButtonEvent& e) { handler3(e); }
    ));
}

// Click Detection Logic Tests
TEST_F(ButtonTest, ClickInsideButtonTriggersHandler) {
    button = std::make_unique<Button>(testEventRouter->getEventRouter(), *testRect);
    TestButtonHandler handler;
    
    button->registerEventHandler(
        [&handler](const MouseButtonEvent& e) { handler(e); }
    );
    
    // Click inside button rectangle (10,10 to 110,60)
    auto clickEvent = std::make_unique<MouseButtonEvent>(createMouseEvent(50.0f, 30.0f));
    testEventRouter->injectAndProcess(std::move(clickEvent));
    
    EXPECT_EQ(handler.callCount, 1);
    EXPECT_EQ(handler.lastEvent.x, 50.0f);
    EXPECT_EQ(handler.lastEvent.y, 30.0f);
}

TEST_F(ButtonTest, ClickOutsideButtonDoesNotTriggerHandler) {
    button = std::make_unique<Button>(testEventRouter->getEventRouter(), *testRect);
    TestButtonHandler handler;
    
    button->registerEventHandler(
        [&handler](const MouseButtonEvent& e) { handler(e); }
    );
    
    // Click outside button rectangle
    auto clickEvent = std::make_unique<MouseButtonEvent>(createMouseEvent(200.0f, 200.0f));
    testEventRouter->injectAndProcess(std::move(clickEvent));
    
    EXPECT_EQ(handler.callCount, 0);
}

TEST_F(ButtonTest, BoundaryTesting) {
    // Test rectangle boundaries (10,10 to 110,60)
    struct TestCase {
        float x, y;
        bool shouldTrigger;
        std::string description;
    };
    
    std::vector<TestCase> testCases = {
        {10.0f, 10.0f, true, "top-left corner"},
        {109.9f, 59.9f, true, "bottom-right inside"},
        {110.1f, 60.1f, false, "bottom-right edge (clearly outside)"},
        {9.9f, 10.0f, false, "left edge outside"},
        {10.0f, 9.9f, false, "top edge outside"},
        {55.0f, 35.0f, true, "center"},
    };
    
    for (const auto& testCase : testCases) {
        // Create fresh instances for each test case to avoid state contamination
        auto testRouter = std::make_unique<TestEventRouter>();
        auto testButton = std::make_unique<Button>(testRouter->getEventRouter(), *testRect);
        TestButtonHandler handler;
        
        testButton->registerEventHandler(
            [&handler](const MouseButtonEvent& e) { handler(e); }
        );
        
        auto clickEvent = std::make_unique<MouseButtonEvent>(createMouseEvent(testCase.x, testCase.y));
        testRouter->injectAndProcess(std::move(clickEvent));
        
        if (testCase.shouldTrigger) {
            EXPECT_EQ(handler.callCount, 1) << "Failed for " << testCase.description;
        } else {
            EXPECT_EQ(handler.callCount, 0) << "Failed for " << testCase.description;
        }
    }
}

TEST_F(ButtonTest, DifferentMouseButtonTypes) {
    // Test different mouse buttons
    std::vector<MouseButtonEvent::Button> buttons = {
        MouseButtonEvent::Button::kLeft,
        MouseButtonEvent::Button::kRight,
        MouseButtonEvent::Button::kMiddle,
        MouseButtonEvent::Button::kX1,
        MouseButtonEvent::Button::kX2
    };
    
    for (auto buttonType : buttons) {
        // Create fresh instances for each test to avoid state contamination
        auto testRouter = std::make_unique<TestEventRouter>();
        auto testButton = std::make_unique<Button>(testRouter->getEventRouter(), *testRect);
        TestButtonHandler handler;
        
        testButton->registerEventHandler(
            [&handler](const MouseButtonEvent& e) { handler(e); }
        );
        
        auto clickEvent = std::make_unique<MouseButtonEvent>(createMouseEvent(50.0f, 30.0f, buttonType));
        testRouter->injectAndProcess(std::move(clickEvent));
        
        EXPECT_EQ(handler.callCount, 1) << "Failed for button type " << static_cast<int>(buttonType);
        EXPECT_EQ(handler.lastEvent.button, buttonType) << "Failed for button type " << static_cast<int>(buttonType);
    }
}

TEST_F(ButtonTest, MultipleHandlersAllReceiveEvent) {
    button = std::make_unique<Button>(testEventRouter->getEventRouter(), *testRect);
    TestButtonHandler handler1, handler2, handler3;
    
    button->registerEventHandler([&handler1](const MouseButtonEvent& e) { handler1(e); });
    button->registerEventHandler([&handler2](const MouseButtonEvent& e) { handler2(e); });
    button->registerEventHandler([&handler3](const MouseButtonEvent& e) { handler3(e); });
    
    // Click inside button
    auto clickEvent = std::make_unique<MouseButtonEvent>(createMouseEvent(50.0f, 30.0f));
    testEventRouter->injectAndProcess(std::move(clickEvent));
    
    // All handlers should receive the event
    EXPECT_EQ(handler1.callCount, 1);
    EXPECT_EQ(handler2.callCount, 1);
    EXPECT_EQ(handler3.callCount, 1);
}

// Edge Cases Tests
TEST_F(ButtonTest, ZeroSizeRectangle) {
    sdl::Rectangle<float> zeroRect(10.0f, 10.0f, 0.0f, 0.0f);
    
    EXPECT_NO_THROW(
        button = std::make_unique<Button>(testEventRouter->getEventRouter(), zeroRect)
    );
    
    TestButtonHandler handler;
    button->registerEventHandler([&handler](const MouseButtonEvent& e) { handler(e); });
    
    // Zero-size rectangle at (10,10) contains its origin point due to SDL_PointInRectFloat inclusive behavior
    auto clickEvent = std::make_unique<MouseButtonEvent>(createMouseEvent(10.0f, 10.0f));
    testEventRouter->injectAndProcess(std::move(clickEvent));
    
    EXPECT_EQ(handler.callCount, 1);
    
    // Also should not contain nearby points
    auto outsideEvent = std::make_unique<MouseButtonEvent>(createMouseEvent(11.0f, 11.0f));
    testEventRouter->injectAndProcess(std::move(outsideEvent));
    
    EXPECT_EQ(handler.callCount, 1);  // Still 1 since (11,11) is outside the zero-size rectangle
}

TEST_F(ButtonTest, NegativeCoordinateRectangle) {
    sdl::Rectangle<float> negativeRect(-50.0f, -30.0f, 100.0f, 60.0f);
    
    EXPECT_NO_THROW(
        button = std::make_unique<Button>(testEventRouter->getEventRouter(), negativeRect)
    );
    
    TestButtonHandler handler;
    button->registerEventHandler([&handler](const MouseButtonEvent& e) { handler(e); });
    
    // Click inside negative coordinate rectangle
    auto clickEvent = std::make_unique<MouseButtonEvent>(createMouseEvent(-25.0f, -15.0f));
    testEventRouter->injectAndProcess(std::move(clickEvent));
    
    EXPECT_EQ(handler.callCount, 1);
}

TEST_F(ButtonTest, MultipleRapidClicks) {
    button = std::make_unique<Button>(testEventRouter->getEventRouter(), *testRect);
    TestButtonHandler handler;
    
    button->registerEventHandler([&handler](const MouseButtonEvent& e) { handler(e); });
    
    // Simulate rapid clicking
    const int clickCount = 10;
    std::vector<std::unique_ptr<BaseEvent>> events;
    for (int i = 0; i < clickCount; ++i) {
        events.push_back(std::make_unique<MouseButtonEvent>(createMouseEvent(50.0f, 30.0f, 
                                         MouseButtonEvent::Button::kLeft, 
                                         true, 
                                         static_cast<uint8_t>(i + 1))));
    }
    testEventRouter->injectAndProcessMultiple(std::move(events));
    
    EXPECT_EQ(handler.callCount, clickCount);
    EXPECT_EQ(handler.receivedEvents.size(), static_cast<size_t>(clickCount));
}

TEST_F(ButtonTest, HandlerExceptionDoesNotCrashButton) {
    button = std::make_unique<Button>(testEventRouter->getEventRouter(), *testRect);
    TestButtonHandler normalHandler;
    
    // Register handler that throws
    button->registerEventHandler([](const MouseButtonEvent&) {
        throw std::runtime_error("Test exception");
    });
    
    // Register normal handler after throwing handler
    button->registerEventHandler([&normalHandler](const MouseButtonEvent& e) { 
        normalHandler(e); 
    });
    
    // Click should throw exception from first handler - this tests current behavior
    auto clickEvent = std::make_unique<MouseButtonEvent>(createMouseEvent(50.0f, 30.0f));
    
    // Current implementation does not catch exceptions within handlers
    // This test documents the current behavior rather than ideal behavior
    EXPECT_THROW(testEventRouter->injectAndProcess(std::move(clickEvent)), std::runtime_error);
    
    // In ideal implementation, normal handler would still receive event
    // but current implementation stops at first exception
}

// Performance/Scale Test
TEST_F(ButtonTest, HandlesManyEventsEfficiently) {
    button = std::make_unique<Button>(testEventRouter->getEventRouter(), *testRect);
    TestButtonHandler handler;
    
    button->registerEventHandler([&handler](const MouseButtonEvent& e) { handler(e); });
    
    // Send many events
    const int eventCount = 100; // Reduced for faster testing
    std::vector<std::unique_ptr<BaseEvent>> events;
    for (int i = 0; i < eventCount; ++i) {
        float x = 10.0f + static_cast<float>(i % 100); // Keep within button bounds
        float y = 10.0f + static_cast<float>(i % 50);
        
        events.push_back(std::make_unique<MouseButtonEvent>(createMouseEvent(x, y)));
    }
    testEventRouter->injectAndProcessMultiple(std::move(events));
    
    EXPECT_EQ(handler.callCount, eventCount);
    EXPECT_EQ(handler.receivedEvents.size(), static_cast<size_t>(eventCount));
}
