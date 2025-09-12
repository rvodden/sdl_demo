#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <type_traits>

#include <sdl/sdl.h>
#include <sdl/timer.h>

// Test fixture for timer tests that require SDL initialization
class TimerTest : public ::testing::Test {
protected:
    void SetUp() override {
        sdl_ = std::make_unique<sdl::SDL>();
    }

    void TearDown() override {
        sdl_.reset();
    }

    std::unique_ptr<sdl::SDL> sdl_;
};

// Test timer construction and destruction
TEST_F(TimerTest, Constructor) {
    EXPECT_NO_THROW(sdl::Timer timer);
}

TEST_F(TimerTest, CopyConstructorDeleted) {
    // Verify that copy constructor is deleted
    static_assert(!std::is_copy_constructible_v<sdl::Timer>);
}

TEST_F(TimerTest, MoveConstructorDeleted) {
    // Verify that move constructor is deleted
    static_assert(!std::is_move_constructible_v<sdl::Timer>);
}

TEST_F(TimerTest, MoveAssignmentDeleted) {
    // Verify that move assignment is deleted
    static_assert(!std::is_move_assignable_v<sdl::Timer>);
}

// Test initial state
TEST_F(TimerTest, InitialState) {
    sdl::Timer timer;
    EXPECT_FALSE(timer.isRunning());
    EXPECT_EQ(timer.getInterval(), std::chrono::milliseconds(0));
}

// Test basic timer functionality
TEST_F(TimerTest, StartAndStop) {
    sdl::Timer timer;
    std::atomic<int> callCount{0};
    
    auto callback = [&callCount]() {
        callCount++;
        return true; // Continue
    };
    
    timer.start(std::chrono::milliseconds(10), callback);
    EXPECT_TRUE(timer.isRunning());
    EXPECT_EQ(timer.getInterval(), std::chrono::milliseconds(10));
    
    timer.stop();
    EXPECT_FALSE(timer.isRunning());
}

// Test one-shot timer (callback returns false)
TEST_F(TimerTest, OneShotTimer) {
    sdl::Timer timer;
    std::atomic<int> callCount{0};
    std::mutex mtx;
    std::condition_variable cv;
    bool callbackExecuted = false;
    
    auto callback = [&]() {
        std::lock_guard<std::mutex> lock(mtx);
        callCount++;
        callbackExecuted = true;
        cv.notify_one();
        return false; // One-shot
    };
    
    timer.start(std::chrono::milliseconds(50), callback);
    EXPECT_TRUE(timer.isRunning());
    
    // Wait for callback to execute
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait_for(lock, std::chrono::milliseconds(200), [&] { return callbackExecuted; });
    }
    
    EXPECT_EQ(callCount.load(), 1);
    
    // Wait a bit more to ensure timer stopped
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_EQ(callCount.load(), 1); // Should not have incremented
}

// Test repeating timer (callback returns true)
TEST_F(TimerTest, RepeatingTimer) {
    sdl::Timer timer;
    std::atomic<int> callCount{0};
    std::mutex mtx;
    std::condition_variable cv;
    
    auto callback = [&]() {
        callCount++;
        if (callCount >= 3) {
            std::lock_guard<std::mutex> lock(mtx);
            cv.notify_one();
            return false; // Stop after 3 calls
        }
        return true; // Continue
    };
    
    timer.start(std::chrono::milliseconds(25), callback);
    EXPECT_TRUE(timer.isRunning());
    
    // Wait for multiple callbacks
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait_for(lock, std::chrono::milliseconds(500), [&] { return callCount >= 3; });
    }
    
    EXPECT_GE(callCount.load(), 3);
    
    timer.stop();
    EXPECT_FALSE(timer.isRunning());
}

// Test timer restart functionality
TEST_F(TimerTest, RestartTimer) {
    sdl::Timer timer;
    std::atomic<int> callCount{0};
    
    auto callback = [&callCount]() {
        callCount++;
        return true;
    };
    
    // Start first timer
    timer.start(std::chrono::milliseconds(100), callback);
    EXPECT_TRUE(timer.isRunning());
    EXPECT_EQ(timer.getInterval(), std::chrono::milliseconds(100));
    
    // Restart with different interval
    timer.start(std::chrono::milliseconds(50), callback);
    EXPECT_TRUE(timer.isRunning());
    EXPECT_EQ(timer.getInterval(), std::chrono::milliseconds(50));
    
    timer.stop();
    EXPECT_FALSE(timer.isRunning());
}

// Test stopping already stopped timer
TEST_F(TimerTest, StopAlreadyStopped) {
    sdl::Timer timer;
    
    EXPECT_FALSE(timer.isRunning());
    EXPECT_NO_THROW(timer.stop()); // Should be safe
    EXPECT_FALSE(timer.isRunning());
}

// Test timer with exception in callback
TEST_F(TimerTest, ExceptionInCallback) {
    sdl::Timer timer;
    std::atomic<int> callCount{0};
    std::mutex mtx;
    std::condition_variable cv;
    bool callbackExecuted = false;
    
    auto callback = [&]() -> bool {
        std::lock_guard<std::mutex> lock(mtx);
        callCount++;
        callbackExecuted = true;
        cv.notify_one();
        throw std::runtime_error("Test exception");
    };
    
    timer.start(std::chrono::milliseconds(50), callback);
    EXPECT_TRUE(timer.isRunning());
    
    // Wait for callback to execute
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait_for(lock, std::chrono::milliseconds(200), [&] { return callbackExecuted; });
    }
    
    EXPECT_EQ(callCount.load(), 1);
    
    // Timer should have stopped due to exception
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_EQ(callCount.load(), 1); // Should not have incremented
}


// Test timer precision (rough timing test)
TEST_F(TimerTest, TimingPrecision) {
    sdl::Timer timer;
    auto start = std::chrono::steady_clock::now();
    std::atomic<bool> callbackExecuted{false};
    std::mutex mtx;
    std::condition_variable cv;
    
    auto callback = [&]() {
        std::lock_guard<std::mutex> lock(mtx);
        callbackExecuted = true;
        cv.notify_one();
        return false;
    };
    
    constexpr auto expectedInterval = std::chrono::milliseconds(100);
    timer.start(expectedInterval, callback);
    
    // Wait for callback
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait_for(lock, std::chrono::milliseconds(300), [&] { return callbackExecuted.load(); });
    }
    
    auto elapsed = std::chrono::steady_clock::now() - start;
    auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed);
    
    EXPECT_TRUE(callbackExecuted);
    // Allow reasonable tolerance for timing (±50ms)
    EXPECT_GE(elapsedMs, expectedInterval - std::chrono::milliseconds(50));
    EXPECT_LE(elapsedMs, expectedInterval + std::chrono::milliseconds(150));
}

// Test multiple timers simultaneously
TEST_F(TimerTest, MultipleTimers) {
    sdl::Timer timer1;
    sdl::Timer timer2;
    std::atomic<int> callCount1{0};
    std::atomic<int> callCount2{0};
    
    auto callback1 = [&callCount1]() {
        callCount1++;
        return callCount1 < 3;
    };
    
    auto callback2 = [&callCount2]() {
        callCount2++;
        return callCount2 < 5;
    };
    
    timer1.start(std::chrono::milliseconds(30), callback1);
    timer2.start(std::chrono::milliseconds(20), callback2);
    
    EXPECT_TRUE(timer1.isRunning());
    EXPECT_TRUE(timer2.isRunning());
    
    // Wait for both timers to complete
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    EXPECT_GE(callCount1.load(), 3);
    EXPECT_GE(callCount2.load(), 5);
    
    timer1.stop();
    timer2.stop();
}