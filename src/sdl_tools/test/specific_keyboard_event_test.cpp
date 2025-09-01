#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>
#include <vector>
#include <chrono>

#include <sdl/event.h>
#include <sdl/event_router.h>

using namespace sdl;
using namespace sdl::tools;

class SpecificKeyboardEventTest : public ::testing::Test {
protected:
    void SetUp() override {
        eventBus = createSDLEventBus();
        eventRouter = std::make_shared<EventRouter>(eventBus);
        receivedEvents.clear();
    }

    void TearDown() override {
        eventRouter.reset();
        eventBus.reset();
    }

    std::shared_ptr<BaseEventBus> eventBus;
    std::shared_ptr<EventRouter> eventRouter;
    std::vector<std::string> receivedEvents;
};

TEST_F(SpecificKeyboardEventTest, GeneralKeyboardEventHandlerReceivesAllEvents) {
    // Register handler for all keyboard events
    eventRouter->registerEventHandler<KeyboardEvent>([&](const KeyboardEvent& event) {
        receivedEvents.push_back("General:" + std::to_string(static_cast<int>(event.keycode)) + ":" + (event.down ? "down" : "up"));
    });

    // Create and route different keyboard events
    auto aKeyDown = std::make_unique<KeyboardEvent>(
        std::chrono::milliseconds(100), 0, 0, ScanCode::kA, KeyCode::kA, true, false, KeyModifier::kNone);
    eventRouter->routeEvent(std::move(aKeyDown));

    auto aKeyUp = std::make_unique<KeyboardEvent>(
        std::chrono::milliseconds(200), 0, 0, ScanCode::kA, KeyCode::kA, false, false, KeyModifier::kNone);
    eventRouter->routeEvent(std::move(aKeyUp));

    auto spaceKeyDown = std::make_unique<KeyboardEvent>(
        std::chrono::milliseconds(300), 0, 0, ScanCode::kSpace, KeyCode::kSpace, true, false, KeyModifier::kNone);
    eventRouter->routeEvent(std::move(spaceKeyDown));

    // General handler should receive all 3 events
    EXPECT_EQ(receivedEvents.size(), 3);
    EXPECT_THAT(receivedEvents, ::testing::Contains("General:" + std::to_string(static_cast<int>(KeyCode::kA)) + ":down"));
    EXPECT_THAT(receivedEvents, ::testing::Contains("General:" + std::to_string(static_cast<int>(KeyCode::kA)) + ":up"));
    EXPECT_THAT(receivedEvents, ::testing::Contains("General:" + std::to_string(static_cast<int>(KeyCode::kSpace)) + ":down"));
}

TEST_F(SpecificKeyboardEventTest, SpecificKeyEventHandlerReceivesOnlyMatchingKey) {
    // Register handler for specific A key events (both up and down)
    eventRouter->registerEventHandler<SpecificKeyboardEvent<KeyCode::kA>>([&](const SpecificKeyboardEvent<KeyCode::kA>& event) {
        receivedEvents.push_back(std::string("SpecificA:") + (event.down ? "down" : "up"));
    });

    // Create and route different keyboard events
    auto aKeyDown = std::make_unique<KeyboardEvent>(
        std::chrono::milliseconds(100), 0, 0, ScanCode::kA, KeyCode::kA, true, false, KeyModifier::kNone);
    eventRouter->routeEvent(std::move(aKeyDown));

    auto bKeyDown = std::make_unique<KeyboardEvent>(
        std::chrono::milliseconds(200), 0, 0, ScanCode::kB, KeyCode::kB, true, false, KeyModifier::kNone);
    eventRouter->routeEvent(std::move(bKeyDown));

    auto aKeyUp = std::make_unique<KeyboardEvent>(
        std::chrono::milliseconds(300), 0, 0, ScanCode::kA, KeyCode::kA, false, false, KeyModifier::kNone);
    eventRouter->routeEvent(std::move(aKeyUp));

    // Specific A handler should receive only A key events (2 events)
    EXPECT_EQ(receivedEvents.size(), 2);
    EXPECT_THAT(receivedEvents, ::testing::Contains("SpecificA:down"));
    EXPECT_THAT(receivedEvents, ::testing::Contains("SpecificA:up"));
    EXPECT_THAT(receivedEvents, ::testing::Not(::testing::Contains(::testing::HasSubstr("B"))));
}

TEST_F(SpecificKeyboardEventTest, SpecificKeyDirectionHandlerReceivesOnlyMatchingDirection) {
    // Register handler for specific A key DOWN events only
    eventRouter->registerEventHandler<SpecificKeyboardEvent<KeyCode::kA, KeyDirection::Down>>([&](const SpecificKeyboardEvent<KeyCode::kA, KeyDirection::Down>& event) {
        receivedEvents.push_back("SpecificADown");
        EXPECT_TRUE(event.down); // Should always be down for this handler
    });

    // Register handler for specific A key UP events only
    eventRouter->registerEventHandler<SpecificKeyboardEvent<KeyCode::kA, KeyDirection::Up>>([&](const SpecificKeyboardEvent<KeyCode::kA, KeyDirection::Up>& event) {
        receivedEvents.push_back("SpecificAUp");
        EXPECT_FALSE(event.down); // Should always be up for this handler
    });

    // Create and route A key events
    auto aKeyDown = std::make_unique<KeyboardEvent>(
        std::chrono::milliseconds(100), 0, 0, ScanCode::kA, KeyCode::kA, true, false, KeyModifier::kNone);
    eventRouter->routeEvent(std::move(aKeyDown));

    auto aKeyUp = std::make_unique<KeyboardEvent>(
        std::chrono::milliseconds(200), 0, 0, ScanCode::kA, KeyCode::kA, false, false, KeyModifier::kNone);
    eventRouter->routeEvent(std::move(aKeyUp));

    // Should receive exactly one down event and one up event
    EXPECT_EQ(receivedEvents.size(), 2);
    EXPECT_THAT(receivedEvents, ::testing::Contains("SpecificADown"));
    EXPECT_THAT(receivedEvents, ::testing::Contains("SpecificAUp"));
}

TEST_F(SpecificKeyboardEventTest, MultipleHandlersReceiveEventsPolymorphically) {
    int generalEventCount = 0;
    int specificAEventCount = 0;
    int specificADownEventCount = 0;

    // Register general handler
    eventRouter->registerEventHandler<KeyboardEvent>([&]([[maybe_unused]] const KeyboardEvent& event) {
        generalEventCount++;
    });

    // Register specific A key handler (both up and down)
    eventRouter->registerEventHandler<SpecificKeyboardEvent<KeyCode::kA>>([&]([[maybe_unused]] const SpecificKeyboardEvent<KeyCode::kA>& event) {
        specificAEventCount++;
    });

    // Register specific A key down handler
    eventRouter->registerEventHandler<SpecificKeyboardEvent<KeyCode::kA, KeyDirection::Down>>([&]([[maybe_unused]] const SpecificKeyboardEvent<KeyCode::kA, KeyDirection::Down>& event) {
        specificADownEventCount++;
    });

    // Route an A key down event
    auto aKeyDown = std::make_unique<KeyboardEvent>(
        std::chrono::milliseconds(100), 0, 0, ScanCode::kA, KeyCode::kA, true, false, KeyModifier::kNone);
    eventRouter->routeEvent(std::move(aKeyDown));

    // All three handlers should receive the A key down event due to polymorphism
    EXPECT_EQ(generalEventCount, 1);       // General handler receives all keyboard events
    EXPECT_EQ(specificAEventCount, 1);     // Specific A handler receives A key events
    EXPECT_EQ(specificADownEventCount, 1); // Specific A down handler receives A key down events

    // Route an A key up event
    auto aKeyUp = std::make_unique<KeyboardEvent>(
        std::chrono::milliseconds(200), 0, 0, ScanCode::kA, KeyCode::kA, false, false, KeyModifier::kNone);
    eventRouter->routeEvent(std::move(aKeyUp));

    // General and specific A handlers should receive the up event, but not the down-only handler
    EXPECT_EQ(generalEventCount, 2);       // General handler receives all keyboard events
    EXPECT_EQ(specificAEventCount, 2);     // Specific A handler receives A key events (both up and down)
    EXPECT_EQ(specificADownEventCount, 1); // Specific A down handler should NOT receive up events

    // Route a B key down event
    auto bKeyDown = std::make_unique<KeyboardEvent>(
        std::chrono::milliseconds(300), 0, 0, ScanCode::kB, KeyCode::kB, true, false, KeyModifier::kNone);
    eventRouter->routeEvent(std::move(bKeyDown));

    // Only the general handler should receive the B key event
    EXPECT_EQ(generalEventCount, 3);       // General handler receives all keyboard events
    EXPECT_EQ(specificAEventCount, 2);     // Specific A handler should NOT receive B key events
    EXPECT_EQ(specificADownEventCount, 1); // Specific A down handler should NOT receive B key events
}

TEST_F(SpecificKeyboardEventTest, MultipleSpecificKeyHandlers) {
    std::vector<std::string> spaceEvents;
    std::vector<std::string> escapeEvents;

    // Register handler for Space key events
    eventRouter->registerEventHandler<SpecificKeyboardEvent<KeyCode::kSpace>>([&](const SpecificKeyboardEvent<KeyCode::kSpace>& event) {
        spaceEvents.push_back(event.down ? "down" : "up");
    });

    // Register handler for Escape key events
    eventRouter->registerEventHandler<SpecificKeyboardEvent<KeyCode::kEscape>>([&](const SpecificKeyboardEvent<KeyCode::kEscape>& event) {
        escapeEvents.push_back(event.down ? "down" : "up");
    });

    // Route Space key events
    auto spaceKeyDown = std::make_unique<KeyboardEvent>(
        std::chrono::milliseconds(100), 0, 0, ScanCode::kSpace, KeyCode::kSpace, true, false, KeyModifier::kNone);
    eventRouter->routeEvent(std::move(spaceKeyDown));

    auto spaceKeyUp = std::make_unique<KeyboardEvent>(
        std::chrono::milliseconds(200), 0, 0, ScanCode::kSpace, KeyCode::kSpace, false, false, KeyModifier::kNone);
    eventRouter->routeEvent(std::move(spaceKeyUp));

    // Route Escape key events
    auto escapeKeyDown = std::make_unique<KeyboardEvent>(
        std::chrono::milliseconds(300), 0, 0, ScanCode::kEscape, KeyCode::kEscape, true, false, KeyModifier::kNone);
    eventRouter->routeEvent(std::move(escapeKeyDown));

    // Verify each handler received only its specific key events
    EXPECT_EQ(spaceEvents.size(), 2);
    EXPECT_THAT(spaceEvents, ::testing::ElementsAre("down", "up"));

    EXPECT_EQ(escapeEvents.size(), 1);
    EXPECT_THAT(escapeEvents, ::testing::ElementsAre("down"));
}

TEST_F(SpecificKeyboardEventTest, GeneralAndSpecificHandlersWorkIndependently) {
    std::vector<std::string> generalEvents;
    std::vector<std::string> specificAEvents;
    std::vector<std::string> specificADownEvents;

    // Register handler for general KeyboardEvent - receives only original KeyboardEvents
    eventRouter->registerEventHandler<KeyboardEvent>([&](const KeyboardEvent& event) {
        std::string eventStr = "KeyboardEvent:" + std::to_string(static_cast<int>(event.keycode)) + ":" + (event.down ? "down" : "up");
        generalEvents.push_back(eventStr);
    });

    // Register handler for specific A key events  
    eventRouter->registerEventHandler<SpecificKeyboardEvent<KeyCode::kA>>([&](const SpecificKeyboardEvent<KeyCode::kA>& event) {
        specificAEvents.push_back(std::string("SpecificA:") + (event.down ? "down" : "up"));
    });

    // Register handler for specific A key down events
    eventRouter->registerEventHandler<SpecificKeyboardEvent<KeyCode::kA, KeyDirection::Down>>([&](const SpecificKeyboardEvent<KeyCode::kA, KeyDirection::Down>& event) {
        specificADownEvents.push_back("SpecificADown");
        EXPECT_TRUE(event.down); // Should always be down for this handler
    });

    // Route an A key down event
    auto aKeyDown = std::make_unique<KeyboardEvent>(
        std::chrono::milliseconds(100), 0, 0, ScanCode::kA, KeyCode::kA, true, false, KeyModifier::kNone);
    eventRouter->routeEvent(std::move(aKeyDown));

    // Route an A key up event  
    auto aKeyUp = std::make_unique<KeyboardEvent>(
        std::chrono::milliseconds(200), 0, 0, ScanCode::kA, KeyCode::kA, false, false, KeyModifier::kNone);
    eventRouter->routeEvent(std::move(aKeyUp));

    // Route a B key down event (should only go to general handler)
    auto bKeyDown = std::make_unique<KeyboardEvent>(
        std::chrono::milliseconds(300), 0, 0, ScanCode::kB, KeyCode::kB, true, false, KeyModifier::kNone);
    eventRouter->routeEvent(std::move(bKeyDown));

    // CORRECTED: General handler receives EXACTLY the 3 original KeyboardEvents we sent
    // It does NOT receive the generated SpecificKeyboardEvents
    EXPECT_EQ(generalEvents.size(), 3) << "General KeyboardEvent handler should receive exactly 3 original events";
    
    // Verify the original KeyboardEvents are received
    EXPECT_THAT(generalEvents, ::testing::Contains("KeyboardEvent:" + std::to_string(static_cast<int>(KeyCode::kA)) + ":down"));
    EXPECT_THAT(generalEvents, ::testing::Contains("KeyboardEvent:" + std::to_string(static_cast<int>(KeyCode::kA)) + ":up"));  
    EXPECT_THAT(generalEvents, ::testing::Contains("KeyboardEvent:" + std::to_string(static_cast<int>(KeyCode::kB)) + ":down"));

    // Verify specific handlers received their specific generated events independently
    EXPECT_EQ(specificAEvents.size(), 2) << "SpecificKeyboardEvent<A> handler should receive 2 events (A down + A up)";
    EXPECT_THAT(specificAEvents, ::testing::Contains("SpecificA:down"));
    EXPECT_THAT(specificAEvents, ::testing::Contains("SpecificA:up"));

    EXPECT_EQ(specificADownEvents.size(), 1) << "SpecificKeyboardEvent<A,Down> handler should receive 1 event (A down only)";
    EXPECT_THAT(specificADownEvents, ::testing::Contains("SpecificADown"));
}

TEST_F(SpecificKeyboardEventTest, KeyboardEventHandlerCalledOncePerEvent) {
    int keyboardEventCallCount = 0;
    int specificAEventCallCount = 0;

    // Register handler for general KeyboardEvent - should be called exactly once per original event
    eventRouter->registerEventHandler<KeyboardEvent>([&](const KeyboardEvent& event) {
        keyboardEventCallCount++;
        // Log what we received for debugging
        std::cout << "KeyboardEvent handler called with keycode: " << static_cast<int>(event.keycode) 
                  << ", down: " << event.down << std::endl;
    });

    // Register handler for specific A key events
    eventRouter->registerEventHandler<SpecificKeyboardEvent<KeyCode::kA>>([&](const SpecificKeyboardEvent<KeyCode::kA>& event) {
        specificAEventCallCount++;
        std::cout << "SpecificKeyboardEvent<A> handler called, down: " << event.down << std::endl;
    });

    // Route exactly one A key down event
    auto aKeyDown = std::make_unique<KeyboardEvent>(
        std::chrono::milliseconds(100), 0, 0, ScanCode::kA, KeyCode::kA, true, false, KeyModifier::kNone);
    eventRouter->routeEvent(std::move(aKeyDown));

    // Route exactly one A key up event  
    auto aKeyUp = std::make_unique<KeyboardEvent>(
        std::chrono::milliseconds(200), 0, 0, ScanCode::kA, KeyCode::kA, false, false, KeyModifier::kNone);
    eventRouter->routeEvent(std::move(aKeyUp));

    std::cout << "KeyboardEvent handler call count: " << keyboardEventCallCount << std::endl;
    std::cout << "SpecificKeyboardEvent<A> handler call count: " << specificAEventCallCount << std::endl;

    // CRITICAL TEST: KeyboardEvent handler should be called exactly once per original event we sent
    // We sent 2 events, so it should be called exactly 2 times, not more
    EXPECT_EQ(keyboardEventCallCount, 2) << "KeyboardEvent handler should be called exactly once per original event, not for generated specific events";
    
    // Specific handler should be called for each matching event (A key events)
    EXPECT_EQ(specificAEventCallCount, 2) << "SpecificKeyboardEvent<A> handler should be called for both A key events";
}