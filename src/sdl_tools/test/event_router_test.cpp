#include <gtest/gtest.h>
#include <event_router.h>
#include <event.h>
#include <user_event.h>

#include <queue>
#include <vector>
#include <memory>
#include <chrono>
#include <optional>

using namespace sdl;
using namespace sdl::tools;

// Test event types for isolated testing
class TestEvent : public Event {
public:
    TestEvent(int value) : Event(std::chrono::milliseconds(0)), testValue(value) {}
    
    void handle(BaseEventHandler& baseEventHandler) override {
        castHandler(*this, baseEventHandler);
    }
    
    int testValue;
};

class AnotherTestEvent : public Event {
public:
    AnotherTestEvent(std::string data) : Event(std::chrono::milliseconds(0)), testData(std::move(data)) {}
    
    void handle(BaseEventHandler& baseEventHandler) override {
        castHandler(*this, baseEventHandler);
    }
    
    std::string testData;
};

// Mock EventBus for controlled testing
class MockEventBus : public BaseEventBus {
private:
    std::queue<std::unique_ptr<BaseEvent>> eventQueue_;
    std::vector<std::unique_ptr<UserEvent>> publishedEvents_;
    bool shouldThrowOnWait_ = false;
    int throwCount_ = 0;
    int maxThrows_ = 1;

public:
    void injectEvent(std::unique_ptr<BaseEvent> event) {
        eventQueue_.push(std::move(event));
    }
    
    void injectQuitEvent() {
        eventQueue_.push(std::make_unique<QuitEvent>(std::chrono::milliseconds(0)));
    }
    
    void setShouldThrowOnWait(bool shouldThrow, int maxThrows = 1) {
        shouldThrowOnWait_ = shouldThrow;
        throwCount_ = 0;
        maxThrows_ = maxThrows;
    }
    
    size_t getPublishedEventCount() const {
        return publishedEvents_.size();
    }
    
    bool hasEvents() const {
        return !eventQueue_.empty();
    }

    // BaseEventBus interface implementation
    auto wait() -> std::unique_ptr<BaseEvent> override {
        if (shouldThrowOnWait_ && throwCount_ < maxThrows_) {
            throwCount_++;
            throw UnknownEventException("Mock exception for testing");
        }
        
        if (eventQueue_.empty()) {
            // If no events queued, return a quit event to prevent infinite loops in tests
            return std::make_unique<QuitEvent>(std::chrono::milliseconds(0));
        }
        
        auto event = std::move(eventQueue_.front());
        eventQueue_.pop();
        return event;
    }
    
    auto poll() -> std::optional<std::unique_ptr<BaseEvent>> override {
        if (eventQueue_.empty()) {
            return std::nullopt;
        }
        
        auto event = std::move(eventQueue_.front());
        eventQueue_.pop();
        return event;
    }
    
    void publish(std::unique_ptr<UserEvent> event) override {
        publishedEvents_.push_back(std::move(event));
    }

    void setRouteCallback(std::function<void(std::unique_ptr<BaseEvent>)> callback) override {
        // Mock implementation - store callback if needed for testing
        routeCallback_ = std::move(callback);
    }

private:
    std::function<void(std::unique_ptr<BaseEvent>)> routeCallback_;
};

// Test fixture for EventRouter tests
class EventRouterTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockBus = std::make_shared<MockEventBus>();
        router = std::make_unique<EventRouter>(mockBus);
    }

    void TearDown() override {
        router.reset();
        mockBus.reset();
    }

    std::shared_ptr<MockEventBus> mockBus;
    std::unique_ptr<EventRouter> router;
};

// Test event handlers for verification
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

// Basic Construction Tests
TEST_F(EventRouterTest, ConstructorWithEventBus) {
    // Router should be successfully constructed with valid EventBus
    EXPECT_NE(router.get(), nullptr);
}

TEST_F(EventRouterTest, MoveConstructor) {
    // Test move constructor
    auto movedRouter = std::move(*router);
    
    // Original should be in moved-from state, moved-to should be valid
    // Note: We can't test much about moved-from state due to implementation details
    
    // Test that moved router can still register handlers (basic functionality)
    TestEventHandler handler;
    EXPECT_NO_THROW(movedRouter.registerEventHandler(handler));
}

// Handler Registration Tests
TEST_F(EventRouterTest, RegisterSingleEventHandler) {
    TestEventHandler handler;
    
    EXPECT_NO_THROW(router->registerEventHandler(handler));
    
    // Inject test event and verify it gets handled
    mockBus->injectEvent(std::make_unique<TestEvent>(42));
    mockBus->injectQuitEvent();
    
    router->run();
    
    EXPECT_EQ(handler.callCount, 1);
    EXPECT_EQ(handler.handledEvents.size(), 1);
    EXPECT_EQ(handler.handledEvents[0], 42);
}

TEST_F(EventRouterTest, RegisterLambdaEventHandler) {
    int callCount = 0;
    std::vector<int> handledValues;
    
    router->registerEventHandler<TestEvent>([&](const TestEvent& event) {
        callCount++;
        handledValues.push_back(event.testValue);
    });
    
    mockBus->injectEvent(std::make_unique<TestEvent>(123));
    mockBus->injectQuitEvent();
    
    router->run();
    
    EXPECT_EQ(callCount, 1);
    EXPECT_EQ(handledValues.size(), 1);
    EXPECT_EQ(handledValues[0], 123);
}

// Event Routing Tests
TEST_F(EventRouterTest, SingleEventToMultipleHandlers) {
    TestEventHandler handler1;
    TestEventHandler handler2;
    
    router->registerEventHandler(handler1);
    router->registerEventHandler(handler2);
    
    mockBus->injectEvent(std::make_unique<TestEvent>(99));
    mockBus->injectQuitEvent();
    
    router->run();
    
    // Both handlers should receive the event
    EXPECT_EQ(handler1.callCount, 1);
    EXPECT_EQ(handler1.handledEvents[0], 99);
    EXPECT_EQ(handler2.callCount, 1);
    EXPECT_EQ(handler2.handledEvents[0], 99);
}

TEST_F(EventRouterTest, MultipleEventsToAppropriateHandlers) {
    TestEventHandler testHandler;
    AnotherTestEventHandler anotherHandler;
    
    router->registerEventHandler(testHandler);
    router->registerEventHandler(anotherHandler);
    
    mockBus->injectEvent(std::make_unique<TestEvent>(10));
    mockBus->injectEvent(std::make_unique<AnotherTestEvent>("hello"));
    mockBus->injectEvent(std::make_unique<TestEvent>(20));
    mockBus->injectQuitEvent();
    
    router->run();
    
    // TestEvent handler should get both TestEvents
    EXPECT_EQ(testHandler.callCount, 2);
    EXPECT_EQ(testHandler.handledEvents.size(), 2);
    EXPECT_EQ(testHandler.handledEvents[0], 10);
    EXPECT_EQ(testHandler.handledEvents[1], 20);
    
    // AnotherTestEvent handler should get only the AnotherTestEvent
    EXPECT_EQ(anotherHandler.callCount, 1);
    EXPECT_EQ(anotherHandler.handledData.size(), 1);
    EXPECT_EQ(anotherHandler.handledData[0], "hello");
}

// Critical Test: Handler Execution Order
TEST_F(EventRouterTest, HandlersExecuteInRegistrationOrder) {
    std::vector<int> executionOrder;
    
    // Register handlers in specific order
    router->registerEventHandler<TestEvent>([&](const TestEvent&) {
        executionOrder.push_back(1);
    });
    
    router->registerEventHandler<TestEvent>([&](const TestEvent&) {
        executionOrder.push_back(2);
    });
    
    router->registerEventHandler<TestEvent>([&](const TestEvent&) {
        executionOrder.push_back(3);
    });
    
    mockBus->injectEvent(std::make_unique<TestEvent>(0));
    mockBus->injectQuitEvent();
    
    router->run();
    
    // Verify handlers executed in registration order (our critical fix!)
    EXPECT_EQ(executionOrder.size(), 3);
    EXPECT_EQ(executionOrder[0], 1);
    EXPECT_EQ(executionOrder[1], 2);
    EXPECT_EQ(executionOrder[2], 3);
}

// Event Loop Control Tests
TEST_F(EventRouterTest, QuitEventStopsEventLoop) {
    TestEventHandler handler;
    router->registerEventHandler(handler);
    
    // Inject events before and after quit - only first should be processed
    mockBus->injectEvent(std::make_unique<TestEvent>(1));
    mockBus->injectQuitEvent();
    mockBus->injectEvent(std::make_unique<TestEvent>(2)); // Should not be processed
    
    router->run();
    
    // Only the first event should be processed before quit
    EXPECT_EQ(handler.callCount, 1);
    EXPECT_EQ(handler.handledEvents[0], 1);
}

TEST_F(EventRouterTest, EmptyEventQueueStopsLoop) {
    TestEventHandler handler;
    router->registerEventHandler(handler);
    
    // Don't inject any events - MockEventBus will return quit event by default
    router->run();
    
    // No events should be processed
    EXPECT_EQ(handler.callCount, 0);
}

// Exception Handling Tests
TEST_F(EventRouterTest, UnknownEventExceptionHandledGracefully) {
    TestEventHandler handler;
    router->registerEventHandler(handler);
    
    // Make it throw once, then allow normal operation (which will return quit event)
    mockBus->setShouldThrowOnWait(true, 1);
    
    // Should not crash - unknown events are caught and ignored, then it gets quit event
    EXPECT_NO_THROW(router->run());
    
    // No events should be handled due to exception
    EXPECT_EQ(handler.callCount, 0);
}

// Handler Lifetime Management Tests  
TEST_F(EventRouterTest, FunctionHandlersLifetimeManagedByRouter) {
    std::vector<int> capturedValues;
    
    {
        // Register lambda handler - router should manage its lifetime
        router->registerEventHandler<TestEvent>([&](const TestEvent& event) {
            capturedValues.push_back(event.testValue);
        });
        // Lambda goes out of scope, but router should keep it alive
    }
    
    mockBus->injectEvent(std::make_unique<TestEvent>(777));
    mockBus->injectQuitEvent();
    
    router->run();
    
    // Handler should still work even after lambda went out of scope
    EXPECT_EQ(capturedValues.size(), 1);
    EXPECT_EQ(capturedValues[0], 777);
}

// Performance/Scale Test
TEST_F(EventRouterTest, HandlesManyEvents) {
    std::vector<int> receivedValues;
    
    router->registerEventHandler<TestEvent>([&](const TestEvent& event) {
        receivedValues.push_back(event.testValue);
    });
    
    // Inject many events
    const int eventCount = 100;
    for (int i = 0; i < eventCount; ++i) {
        mockBus->injectEvent(std::make_unique<TestEvent>(i));
    }
    mockBus->injectQuitEvent();
    
    router->run();
    
    EXPECT_EQ(receivedValues.size(), static_cast<size_t>(eventCount));
    for (size_t i = 0; i < static_cast<size_t>(eventCount); ++i) {
        EXPECT_EQ(receivedValues[i], static_cast<int>(i));
    }
}

// Multiple Handler Types Test
TEST_F(EventRouterTest, MixedHandlerTypesWorkTogether) {
    // Object handler
    TestEventHandler objectHandler;
    router->registerEventHandler(objectHandler);
    
    // Lambda handler
    std::vector<int> lambdaResults;
    router->registerEventHandler<TestEvent>([&](const TestEvent& event) {
        lambdaResults.push_back(event.testValue * 2);
    });
    
    mockBus->injectEvent(std::make_unique<TestEvent>(5));
    mockBus->injectQuitEvent();
    
    router->run();
    
    // Object handler should receive original value
    EXPECT_EQ(objectHandler.handledEvents[0], 5);
    
    // Lambda handler should receive and process value  
    EXPECT_EQ(lambdaResults[0], 10);
}