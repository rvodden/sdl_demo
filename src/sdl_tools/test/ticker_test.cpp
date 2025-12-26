#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>

#include <gtest/gtest.h>

#include "sdl/event.h"
#include "sdl/event_router.h"
#include "sdl/sdl.h"
#include "sdl/ticker.h"
#include "sdl/user_event.h"

using namespace sdl;
using namespace sdl::tools;

// Mock EventBus for controlled testing
class MockEventBus : public BaseEventBus {
private:
    std::queue<std::unique_ptr<BaseEvent>> eventQueue_;
    std::vector<std::unique_ptr<UserEvent>> publishedEvents_;
    mutable std::mutex mutex_;

public:
    void pushEvent(std::unique_ptr<BaseEvent> event) {
        std::lock_guard<std::mutex> lock(mutex_);
        eventQueue_.push(std::move(event));
    }
    
    void injectQuitEvent() {
        std::lock_guard<std::mutex> lock(mutex_);
        eventQueue_.push(std::make_unique<QuitEvent>(std::chrono::milliseconds(0)));
    }
    
    size_t getPublishedEventCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return publishedEvents_.size();
    }
    
    bool hasEvents() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return !eventQueue_.empty();
    }

    auto poll() -> std::optional<std::unique_ptr<BaseEvent>> override {
        std::lock_guard<std::mutex> lock(mutex_);
        if (eventQueue_.empty()) {
            return std::nullopt;
        }
        auto event = std::move(eventQueue_.front());
        eventQueue_.pop();
        return event;
    }

    auto wait() -> std::unique_ptr<BaseEvent> override {
        // For testing, just return a quit event if no events are available
        auto event = poll();
        if (event) {
            return std::move(*event);
        }
        return std::make_unique<QuitEvent>(std::chrono::milliseconds(0));
    }

    void publish(std::unique_ptr<UserEvent> userEvent) override {
        std::lock_guard<std::mutex> lock(mutex_);
        publishedEvents_.push_back(std::move(userEvent));
        // Also add to regular event queue for processing
        eventQueue_.push(std::make_unique<QuitEvent>(std::chrono::milliseconds(0))); // Dummy event
    }

    // Get the most recent published event (for testing)
    auto getLastPublishedEvent() const -> const UserEvent* {
        std::lock_guard<std::mutex> lock(mutex_);
        if (publishedEvents_.empty()) {
            return nullptr;
        }
        return publishedEvents_.back().get();
    }

    // Get all published events (for testing)
    auto getAllPublishedEvents() const -> std::vector<const UserEvent*> {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<const UserEvent*> result;
        for (const auto& event : publishedEvents_) {
            result.push_back(event.get());
        }
        return result;
    }

    void clearEvents() {
        std::lock_guard<std::mutex> lock(mutex_);
        publishedEvents_.clear();
        while (!eventQueue_.empty()) {
            eventQueue_.pop();
        }
    }

    // Abstract method implementations (not used in ticker tests but required)
    void setRouteCallback(std::function<void(std::unique_ptr<BaseEvent>)> callback) override {
        // Not used in these tests
        (void)callback;
    }

    void injectEvent(const std::any& eventData, std::type_index eventTypeId) override {
        // Not used in these tests
        (void)eventData;
        (void)eventTypeId;
    }
};

// Test fixture
class TickerTest : public ::testing::Test {
protected:
    void SetUp() override {
        sdl_ = std::make_unique<sdl::SDL>();
        mockEventBus_ = std::make_shared<MockEventBus>();
        eventRouter_ = std::make_shared<EventRouter>(mockEventBus_);
    }

    void TearDown() override {
        eventRouter_.reset();
        mockEventBus_.reset();
        sdl_.reset();
    }

    std::unique_ptr<sdl::SDL> sdl_;
    std::shared_ptr<MockEventBus> mockEventBus_;
    std::shared_ptr<EventRouter> eventRouter_;
    std::vector<EventRegistration> _eventRegistrations;
};

// Test ticker construction and destruction
TEST_F(TickerTest, Constructor) {
    EXPECT_NO_THROW(Ticker ticker(mockEventBus_, std::chrono::milliseconds(100)));
}

TEST_F(TickerTest, CopyConstructorDeleted) {
    // Verify that copy constructor is deleted
    static_assert(!std::is_copy_constructible_v<Ticker>);
}

TEST_F(TickerTest, MoveConstructorDeleted) {
    // Verify that move constructor is deleted
    static_assert(!std::is_move_constructible_v<Ticker>);
}

TEST_F(TickerTest, MoveAssignmentDeleted) {
    // Verify that move assignment is deleted
    static_assert(!std::is_move_assignable_v<Ticker>);
}

// Test initial state
TEST_F(TickerTest, InitialState) {
    Ticker ticker(mockEventBus_, std::chrono::milliseconds(100));
    
    EXPECT_FALSE(ticker.isRunning());
    EXPECT_EQ(ticker.getInterval(), std::chrono::milliseconds(100));
    EXPECT_EQ(ticker.getTickCount(), 0);
}

// Test basic start/stop functionality
TEST_F(TickerTest, StartAndStop) {
    Ticker ticker(mockEventBus_, std::chrono::milliseconds(50));
    
    // Start ticker
    EXPECT_NO_THROW(ticker.start());
    EXPECT_TRUE(ticker.isRunning());
    
    // Stop ticker
    EXPECT_NO_THROW(ticker.stop());
    EXPECT_FALSE(ticker.isRunning());
}

// Test stop when already stopped
TEST_F(TickerTest, StopAlreadyStopped) {
    Ticker ticker(mockEventBus_, std::chrono::milliseconds(50));
    
    EXPECT_FALSE(ticker.isRunning());
    EXPECT_NO_THROW(ticker.stop()); // Should be safe
    EXPECT_FALSE(ticker.isRunning());
}

// Test start when already started
TEST_F(TickerTest, StartAlreadyStarted) {
    Ticker ticker(mockEventBus_, std::chrono::milliseconds(50));
    
    ticker.start();
    EXPECT_TRUE(ticker.isRunning());
    
    // Starting again should be safe (no effect)
    EXPECT_NO_THROW(ticker.start());
    EXPECT_TRUE(ticker.isRunning());
    
    ticker.stop();
}

// Test interval getter
TEST_F(TickerTest, IntervalGetter) {
    auto interval1 = std::chrono::milliseconds(250);
    auto interval2 = std::chrono::milliseconds(100);
    
    Ticker ticker1(mockEventBus_, interval1);
    Ticker ticker2(mockEventBus_, interval2);
    
    EXPECT_EQ(ticker1.getInterval(), interval1);
    EXPECT_EQ(ticker2.getInterval(), interval2);
}

// Test tick event generation
TEST_F(TickerTest, TickEventGeneration) {
    Ticker ticker(mockEventBus_, std::chrono::milliseconds(30));
    
    std::atomic<uint32_t> tickCount{0};
    std::mutex mtx;
    std::condition_variable cv;
    
    // Register handler for tick events
    _eventRegistrations.push_back(eventRouter_->registerEventHandler<TickEvent>([&](const TickEvent& e) {
        std::lock_guard<std::mutex> lock(mtx);
        tickCount.store(e.tickNumber);
        cv.notify_one();
    }));
    
    // Start ticker
    ticker.start();
    
    // Wait for first tick event to be published
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Check that events were published
    EXPECT_GT(mockEventBus_->getPublishedEventCount(), 0);
    
    // Stop ticker
    ticker.stop();
    EXPECT_FALSE(ticker.isRunning());
}

// Test sequential tick numbering
TEST_F(TickerTest, SequentialTickNumbers) {
    Ticker ticker(mockEventBus_, std::chrono::milliseconds(25));
    
    std::vector<uint32_t> receivedTicks;
    std::mutex mtx;
    std::condition_variable cv;
    
    // Register handler to collect tick numbers
    _eventRegistrations.push_back(eventRouter_->registerEventHandler<TickEvent>([&](const TickEvent& e) {
        std::lock_guard<std::mutex> lock(mtx);
        receivedTicks.push_back(e.tickNumber);
        if (receivedTicks.size() >= 3) {
            cv.notify_one();
        }
    }));
    
    ticker.start();
    
    // Wait for several ticks
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait_for(lock, std::chrono::milliseconds(200), [&] { return receivedTicks.size() >= 3; });
    }
    
    ticker.stop();
    
    // Process events by manually routing published events
    auto publishedEvents = mockEventBus_->getAllPublishedEvents();
    for (const auto* userEvent : publishedEvents) {
        // Cast to TickEvent and route it
        if (const auto* tickEvent = dynamic_cast<const TickEvent*>(userEvent)) {
            eventRouter_->routeEvent(*tickEvent);
        }
    }
    
    // Verify we got sequential tick numbers starting from 1
    ASSERT_GE(receivedTicks.size(), 3);
    EXPECT_EQ(receivedTicks[0], 1);
    EXPECT_EQ(receivedTicks[1], 2);
    EXPECT_EQ(receivedTicks[2], 3);
}

// Test tick count tracking
TEST_F(TickerTest, TickCountTracking) {
    Ticker ticker(mockEventBus_, std::chrono::milliseconds(20));
    
    EXPECT_EQ(ticker.getTickCount(), 0);
    
    ticker.start();
    
    // Wait for some ticks
    std::this_thread::sleep_for(std::chrono::milliseconds(80));
    
    ticker.stop();
    
    // Tick count should have increased
    uint32_t finalCount = ticker.getTickCount();
    EXPECT_GT(finalCount, 0);
    EXPECT_LE(finalCount, 10); // Reasonable upper bound
    
    // After stopping, count should remain the same
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_EQ(ticker.getTickCount(), finalCount);
}

// Test pause and resume (tick count continues)
TEST_F(TickerTest, PauseAndResume) {
    Ticker ticker(mockEventBus_, std::chrono::milliseconds(30));
    
    // Start, let it tick a few times, then stop
    ticker.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ticker.stop();
    
    uint32_t countAfterFirstRun = ticker.getTickCount();
    EXPECT_GT(countAfterFirstRun, 0);
    
    // Clear events and restart
    mockEventBus_->clearEvents();
    ticker.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(80));
    ticker.stop();
    
    // Tick count should have continued from where it left off
    uint32_t countAfterSecondRun = ticker.getTickCount();
    EXPECT_GT(countAfterSecondRun, countAfterFirstRun);
}

// Test multiple tickers simultaneously
TEST_F(TickerTest, MultipleTickers) {
    auto eventBus1 = std::make_shared<MockEventBus>();
    auto eventBus2 = std::make_shared<MockEventBus>();
    
    Ticker ticker1(eventBus1, std::chrono::milliseconds(40));
    Ticker ticker2(eventBus2, std::chrono::milliseconds(60));
    
    ticker1.start();
    ticker2.start();
    
    EXPECT_TRUE(ticker1.isRunning());
    EXPECT_TRUE(ticker2.isRunning());
    
    // Let both run for a while
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    
    ticker1.stop();
    ticker2.stop();
    
    EXPECT_FALSE(ticker1.isRunning());
    EXPECT_FALSE(ticker2.isRunning());
    
    // Both should have published events
    EXPECT_GT(eventBus1->getPublishedEventCount(), 0);
    EXPECT_GT(eventBus2->getPublishedEventCount(), 0);
    
    // Both should have different tick counts
    EXPECT_GT(ticker1.getTickCount(), 0);
    EXPECT_GT(ticker2.getTickCount(), 0);
}

// Test ticker destruction while running
TEST_F(TickerTest, DestructionWhileRunning) {
    auto ticker = std::make_unique<Ticker>(mockEventBus_, std::chrono::milliseconds(50));
    
    ticker->start();
    EXPECT_TRUE(ticker->isRunning());
    
    // Destruction should automatically stop the ticker
    EXPECT_NO_THROW(ticker.reset());
}

// Test very short intervals
TEST_F(TickerTest, ShortInterval) {
    Ticker ticker(mockEventBus_, std::chrono::milliseconds(10));
    
    ticker.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(60));
    ticker.stop();
    
    // Should have generated several ticks
    EXPECT_GT(ticker.getTickCount(), 3);
    EXPECT_GT(mockEventBus_->getPublishedEventCount(), 3);
}

// Test that TickEvent is properly typed
TEST_F(TickerTest, TickEventType) {
    // Verify TickEvent inherits from CustomUserEvent
    static_assert(std::is_base_of_v<CustomUserEvent<TickEvent>, TickEvent>);
    static_assert(std::is_base_of_v<UserEvent, TickEvent>);
    static_assert(std::is_base_of_v<BaseEvent, TickEvent>);
    
    // Create a tick event and verify its properties
    TickEvent event(42);
    EXPECT_EQ(event.tickNumber, 42);
}