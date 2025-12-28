#include <algorithm>
#include <chrono>
#include <cstddef>
#include <memory>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "mocks/mock_event_bus.h"
#include "sdl/event.h"
#include "sdl/event_router.h"
#include "sdl/user_event.h"

using namespace sdl;
using namespace sdl::tools;
using sdl::test::MockEventBus;

// Test event types for isolated testing
class TestEvent : public Event {
public:
    explicit TestEvent(int value) : Event(std::chrono::milliseconds(0)), testValue(value) {}

    void handle(BaseEventHandler& baseEventHandler) override {
        castHandler(*this, baseEventHandler);
    }

    int testValue;
};

class AnotherTestEvent : public Event {
public:
    explicit AnotherTestEvent(std::string data) : Event(std::chrono::milliseconds(0)), testData(std::move(data)) {}

    void handle(BaseEventHandler& baseEventHandler) override {
        castHandler(*this, baseEventHandler);
    }

    std::string testData;
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
    std::vector<EventRegistration> _eventRegistrations;
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
    EXPECT_NO_THROW(_eventRegistrations.push_back(movedRouter.registerEventHandler(handler)));
}

// Handler Registration Tests
TEST_F(EventRouterTest, RegisterSingleEventHandler) {
    TestEventHandler handler;

    EXPECT_NO_THROW(_eventRegistrations.push_back(router->registerEventHandler(handler)));

    // Inject test event and verify it gets handled
    mockBus->pushEvent(std::make_unique<TestEvent>(42));
    mockBus->injectQuitEvent();

    router->run();

    EXPECT_EQ(handler.callCount, 1);
    EXPECT_EQ(handler.handledEvents.size(), 1);
    EXPECT_EQ(handler.handledEvents[0], 42);
}

TEST_F(EventRouterTest, RegisterLambdaEventHandler) {
    int callCount = 0;
    std::vector<int> handledValues;

    _eventRegistrations.push_back(router->registerEventHandler<TestEvent>([&](const TestEvent& event) {
        callCount++;
        handledValues.push_back(event.testValue);
    }));

    mockBus->pushEvent(std::make_unique<TestEvent>(123));
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
    
    _eventRegistrations.push_back(router->registerEventHandler(handler1));
    _eventRegistrations.push_back(router->registerEventHandler(handler2));
    
    mockBus->pushEvent(std::make_unique<TestEvent>(99));
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
    
    _eventRegistrations.push_back(router->registerEventHandler(testHandler));
    _eventRegistrations.push_back(router->registerEventHandler(anotherHandler));
    
    mockBus->pushEvent(std::make_unique<TestEvent>(10));
    mockBus->pushEvent(std::make_unique<AnotherTestEvent>("hello"));
    mockBus->pushEvent(std::make_unique<TestEvent>(20));
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
    _eventRegistrations.push_back(router->registerEventHandler<TestEvent>([&](const TestEvent&) {
        executionOrder.push_back(1);
    }));

    _eventRegistrations.push_back(router->registerEventHandler<TestEvent>([&](const TestEvent&) {
        executionOrder.push_back(2);
    }));

    _eventRegistrations.push_back(router->registerEventHandler<TestEvent>([&](const TestEvent&) {
        executionOrder.push_back(3);
    }));
    
    mockBus->pushEvent(std::make_unique<TestEvent>(0));
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
    _eventRegistrations.push_back(router->registerEventHandler(handler));
    
    // Inject events before and after quit - only first should be processed
    mockBus->pushEvent(std::make_unique<TestEvent>(1));
    mockBus->injectQuitEvent();
    mockBus->pushEvent(std::make_unique<TestEvent>(2)); // Should not be processed
    
    router->run();
    
    // Only the first event should be processed before quit
    EXPECT_EQ(handler.callCount, 1);
    EXPECT_EQ(handler.handledEvents[0], 1);
}

TEST_F(EventRouterTest, EmptyEventQueueStopsLoop) {
    TestEventHandler handler;
    _eventRegistrations.push_back(router->registerEventHandler(handler));
    
    // Don't inject any events - MockEventBus will return quit event by default
    router->run();
    
    // No events should be processed
    EXPECT_EQ(handler.callCount, 0);
}

// Exception Handling Tests
TEST_F(EventRouterTest, UnknownEventExceptionHandledGracefully) {
    TestEventHandler handler;
    _eventRegistrations.push_back(router->registerEventHandler(handler));
    
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
        _eventRegistrations.push_back(router->registerEventHandler<TestEvent>([&](const TestEvent& event) {
            capturedValues.push_back(event.testValue);
        }));
        // Lambda goes out of scope, but router should keep it alive
    }
    
    mockBus->pushEvent(std::make_unique<TestEvent>(777));
    mockBus->injectQuitEvent();
    
    router->run();
    
    // Handler should still work even after lambda went out of scope
    EXPECT_EQ(capturedValues.size(), 1);
    EXPECT_EQ(capturedValues[0], 777);
}

// Performance/Scale Test
TEST_F(EventRouterTest, HandlesManyEvents) {
    std::vector<int> receivedValues;

    _eventRegistrations.push_back(router->registerEventHandler<TestEvent>([&](const TestEvent& event) {
        receivedValues.push_back(event.testValue);
    }));
    
    // Inject many events
    const int eventCount = 100;
    for (int i = 0; i < eventCount; ++i) {
        mockBus->pushEvent(std::make_unique<TestEvent>(i));
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
    _eventRegistrations.push_back(router->registerEventHandler(objectHandler));

    // Lambda handler
    std::vector<int> lambdaResults;
    _eventRegistrations.push_back(router->registerEventHandler<TestEvent>([&](const TestEvent& event) {
        lambdaResults.push_back(event.testValue * 2);
    }));
    
    mockBus->pushEvent(std::make_unique<TestEvent>(5));
    mockBus->injectQuitEvent();
    
    router->run();
    
    // Object handler should receive original value
    EXPECT_EQ(objectHandler.handledEvents[0], 5);
    
    // Lambda handler should receive and process value
    EXPECT_EQ(lambdaResults[0], 10);
}

// ============================================================================
// Event Deregistration Tests
// ============================================================================

// Test basic RAII deregistration
TEST_F(EventRouterTest, EventRegistrationRAIIDeregistersHandler) {
    int callCount = 0;

    {
        // Register handler in inner scope
        auto registration = router->registerEventHandler<TestEvent>([&](const TestEvent&) {
            callCount++;
        });

        // Handler should work while registration is alive
        mockBus->pushEvent(std::make_unique<TestEvent>(1));
        EXPECT_TRUE(router->processNextEvent());
        EXPECT_EQ(callCount, 1);
    }
    // Registration destroyed here - handler should be deregistered

    // Handler should no longer receive events
    mockBus->pushEvent(std::make_unique<TestEvent>(2));
    EXPECT_TRUE(router->processNextEvent());
    EXPECT_EQ(callCount, 1);  // Still 1, not incremented
}

// Test manual deregistration
TEST_F(EventRouterTest, ManualUnregisterStopsHandlerCalls) {
    int callCount = 0;

    auto registration = router->registerEventHandler<TestEvent>([&](const TestEvent&) {
        callCount++;
    });

    // Handler should work initially
    mockBus->pushEvent(std::make_unique<TestEvent>(1));
    EXPECT_TRUE(router->processNextEvent());
    EXPECT_EQ(callCount, 1);

    // Manually unregister
    registration.unregister();

    // Handler should no longer receive events
    mockBus->pushEvent(std::make_unique<TestEvent>(2));
    EXPECT_TRUE(router->processNextEvent());
    EXPECT_EQ(callCount, 1);  // Still 1
}

// Test that isRegistered() works correctly
TEST_F(EventRouterTest, IsRegisteredReflectsRegistrationState) {
    auto registration = router->registerEventHandler<TestEvent>([](const TestEvent&) {});

    EXPECT_TRUE(registration.isRegistered());

    registration.unregister();

    EXPECT_FALSE(registration.isRegistered());
}

// Test double unregister is safe
TEST_F(EventRouterTest, DoubleUnregisterIsSafe) {
    auto registration = router->registerEventHandler<TestEvent>([](const TestEvent&) {});

    EXPECT_NO_THROW(registration.unregister());
    EXPECT_NO_THROW(registration.unregister());  // Should be safe

    EXPECT_FALSE(registration.isRegistered());
}

// Test move semantics
TEST_F(EventRouterTest, EventRegistrationMoveSemantics) {
    int callCount = 0;

    auto registration1 = router->registerEventHandler<TestEvent>([&](const TestEvent&) {
        callCount++;
    });

    EXPECT_TRUE(registration1.isRegistered());

    // Move registration
    auto registration2 = std::move(registration1);

    // registration2 should now own the registration
    EXPECT_TRUE(registration2.isRegistered());

    // Handler should still work
    mockBus->pushEvent(std::make_unique<TestEvent>(1));
    EXPECT_TRUE(router->processNextEvent());
    EXPECT_EQ(callCount, 1);

    // Destroying registration2 should deregister
    registration2.unregister();

    mockBus->pushEvent(std::make_unique<TestEvent>(2));
    EXPECT_TRUE(router->processNextEvent());
    EXPECT_EQ(callCount, 1);  // Still 1
}

// Test multiple handlers with selective deregistration
TEST_F(EventRouterTest, SelectiveDeregistrationLeavesOtherHandlersActive) {
    int handler1CallCount = 0;
    int handler2CallCount = 0;
    int handler3CallCount = 0;

    auto reg1 = router->registerEventHandler<TestEvent>([&](const TestEvent&) {
        handler1CallCount++;
    });

    auto reg2 = router->registerEventHandler<TestEvent>([&](const TestEvent&) {
        handler2CallCount++;
    });

    auto reg3 = router->registerEventHandler<TestEvent>([&](const TestEvent&) {
        handler3CallCount++;
    });

    // All handlers should receive event
    mockBus->pushEvent(std::make_unique<TestEvent>(1));
    EXPECT_TRUE(router->processNextEvent());
    EXPECT_EQ(handler1CallCount, 1);
    EXPECT_EQ(handler2CallCount, 1);
    EXPECT_EQ(handler3CallCount, 1);

    // Deregister handler 2
    reg2.unregister();

    // Only handlers 1 and 3 should receive event
    mockBus->pushEvent(std::make_unique<TestEvent>(2));
    EXPECT_TRUE(router->processNextEvent());
    EXPECT_EQ(handler1CallCount, 2);
    EXPECT_EQ(handler2CallCount, 1);  // Still 1
    EXPECT_EQ(handler3CallCount, 2);
}

// Test object handler deregistration
TEST_F(EventRouterTest, ObjectHandlerDeregistration) {
    TestEventHandler handler;

    auto registration = router->registerEventHandler(handler);

    mockBus->pushEvent(std::make_unique<TestEvent>(42));
    EXPECT_TRUE(router->processNextEvent());
    EXPECT_EQ(handler.callCount, 1);

    registration.unregister();

    mockBus->pushEvent(std::make_unique<TestEvent>(99));
    EXPECT_TRUE(router->processNextEvent());
    EXPECT_EQ(handler.callCount, 1);  // Still 1
}

// Test deregistration with EventRouter destroyed
TEST_F(EventRouterTest, DeregistrationAfterRouterDestroyedIsSafe) {
    auto registration = router->registerEventHandler<TestEvent>([](const TestEvent&) {});

    EXPECT_TRUE(registration.isRegistered());

    // Destroy router
    router.reset();

    // Deregistration should be safe even after router is gone
    EXPECT_NO_THROW(registration.unregister());
    EXPECT_FALSE(registration.isRegistered());
}

// Test registration destroyed after router destroyed
TEST_F(EventRouterTest, RegistrationDestructorAfterRouterDestroyedIsSafe) {
    auto registration = router->registerEventHandler<TestEvent>([](const TestEvent&) {});

    // Destroy router first
    router.reset();

    // Destroying registration should be safe
    EXPECT_NO_THROW(registration = EventRegistration{});
}

// Test high-frequency registration/deregistration
TEST_F(EventRouterTest, HighFrequencyRegistrationDeregistration) {
    int totalCalls = 0;

    for (int i = 0; i < 100; ++i) {
        auto registration = router->registerEventHandler<TestEvent>([&](const TestEvent&) {
            totalCalls++;
        });

        mockBus->pushEvent(std::make_unique<TestEvent>(i));
        EXPECT_TRUE(router->processNextEvent());

        // Registration destroyed at end of loop iteration
    }

    // Each handler should have received exactly one event
    EXPECT_EQ(totalCalls, 100);

    // No handlers should be active now
    mockBus->pushEvent(std::make_unique<TestEvent>(999));
    EXPECT_TRUE(router->processNextEvent());
    EXPECT_EQ(totalCalls, 100);  // Still 100
}
