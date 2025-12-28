#ifndef SDL_TEST_MOCKS_MOCK_EVENT_BUS_H
#define SDL_TEST_MOCKS_MOCK_EVENT_BUS_H

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <vector>

#include "sdl/event.h"
#include "sdl/user_event.h"

namespace sdl::test {

/**
 * @brief Mock EventBus for controlled testing
 *
 * This class provides a thread-safe mock implementation of BaseEventBus
 * for use in unit tests. It allows:
 * - Pushing events to be returned by wait()/poll()
 * - Tracking published user events
 * - Configuring behavior (e.g., return empty, throw exceptions)
 *
 * Thread-safety: All public methods are thread-safe and can be called
 * from multiple threads (e.g., timer callbacks and main test thread).
 */
class MockEventBus : public BaseEventBus {
public:
    MockEventBus() = default;
    ~MockEventBus() override = default;

    // Non-copyable, non-movable
    MockEventBus(const MockEventBus&) = delete;
    auto operator=(const MockEventBus&) -> MockEventBus& = delete;
    MockEventBus(MockEventBus&&) = delete;
    auto operator=(MockEventBus&&) -> MockEventBus& = delete;

    // --- Event injection for testing ---

    /**
     * @brief Push an event to be returned by wait()/poll()
     */
    void pushEvent(std::unique_ptr<BaseEvent> event) {
        std::lock_guard<std::mutex> lock(mutex_);
        eventQueue_.push(std::move(event));
    }

    /**
     * @brief Alias for pushEvent (for compatibility)
     */
    void addEvent(std::unique_ptr<BaseEvent> event) {
        pushEvent(std::move(event));
    }

    /**
     * @brief Convenience method to inject a quit event
     */
    void injectQuitEvent() {
        pushEvent(std::make_unique<QuitEvent>(std::chrono::milliseconds(0)));
    }

    // --- Published event inspection ---

    /**
     * @brief Get the number of events published via publish()
     */
    [[nodiscard]] auto getPublishedEventCount() const -> size_t {
        std::lock_guard<std::mutex> lock(mutex_);
        return publishedEvents_.size();
    }

    /**
     * @brief Alias for getPublishedEventCount (for compatibility)
     */
    [[nodiscard]] auto getPublishedUserEventCount() const -> size_t {
        return getPublishedEventCount();
    }

    /**
     * @brief Get the number of times publish() was called (atomic counter)
     */
    [[nodiscard]] auto getPublishCallCount() const -> int {
        return publishCallCount_.load();
    }

    /**
     * @brief Get the most recent published event
     */
    [[nodiscard]] auto getLastPublishedEvent() const -> const UserEvent* {
        std::lock_guard<std::mutex> lock(mutex_);
        if (publishedEvents_.empty()) {
            return nullptr;
        }
        return publishedEvents_.back().get();
    }

    /**
     * @brief Get all published events (for verification)
     */
    [[nodiscard]] auto getAllPublishedEvents() const -> std::vector<const UserEvent*> {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<const UserEvent*> result;
        result.reserve(publishedEvents_.size());
        for (const auto& event : publishedEvents_) {
            result.push_back(event.get());
        }
        return result;
    }

    // --- Queue state inspection ---

    /**
     * @brief Check if there are events in the queue
     */
    [[nodiscard]] auto hasEvents() const -> bool {
        std::lock_guard<std::mutex> lock(mutex_);
        return !eventQueue_.empty();
    }

    // --- Behavior configuration ---

    /**
     * @brief Configure poll() to always return empty (default: true)
     */
    void setShouldReturnEmptyOnPoll(bool shouldReturn) {
        std::lock_guard<std::mutex> lock(mutex_);
        shouldReturnEmptyOnPoll_ = shouldReturn;
    }

    /**
     * @brief Configure wait() to throw UnknownEventException
     * @param shouldThrow Whether to throw
     * @param maxThrows Maximum number of times to throw (then stops throwing)
     */
    void setShouldThrowOnWait(bool shouldThrow, int maxThrows = 1) {
        std::lock_guard<std::mutex> lock(mutex_);
        shouldThrowOnWait_ = shouldThrow;
        throwCount_ = 0;
        maxThrows_ = maxThrows;
    }

    /**
     * @brief Clear all events (both queued and published)
     */
    void clearEvents() {
        std::lock_guard<std::mutex> lock(mutex_);
        publishedEvents_.clear();
        while (!eventQueue_.empty()) {
            eventQueue_.pop();
        }
        publishCallCount_.store(0);
    }

    // --- BaseEventBus interface implementation ---

    auto wait() -> std::unique_ptr<BaseEvent> override {
        std::lock_guard<std::mutex> lock(mutex_);

        if (shouldThrowOnWait_ && throwCount_ < maxThrows_) {
            throwCount_++;
            throw UnknownEventException("Mock exception for testing");
        }

        if (eventQueue_.empty()) {
            // Return quit event to prevent infinite loops in tests
            return std::make_unique<QuitEvent>(std::chrono::milliseconds(0));
        }

        auto event = std::move(eventQueue_.front());
        eventQueue_.pop();
        return event;
    }

    auto poll() -> std::optional<std::unique_ptr<BaseEvent>> override {
        std::lock_guard<std::mutex> lock(mutex_);

        if (shouldReturnEmptyOnPoll_ || eventQueue_.empty()) {
            return std::nullopt;
        }

        auto event = std::move(eventQueue_.front());
        eventQueue_.pop();
        return event;
    }

    void publish(std::unique_ptr<UserEvent> event) override {
        publishCallCount_.fetch_add(1);
        std::lock_guard<std::mutex> lock(mutex_);
        publishedEvents_.push_back(std::move(event));
    }

    void setRouteCallback(std::function<void(std::unique_ptr<BaseEvent>)> callback) override {
        std::lock_guard<std::mutex> lock(mutex_);
        routeCallback_ = std::move(callback);
    }

    void injectEvent(const std::any& eventData, std::type_index eventTypeId) override {
        // Mock implementation - not used in most tests
        (void)eventData;
        (void)eventTypeId;
    }

private:
    mutable std::mutex mutex_;
    std::queue<std::unique_ptr<BaseEvent>> eventQueue_;
    std::vector<std::unique_ptr<UserEvent>> publishedEvents_;
    std::function<void(std::unique_ptr<BaseEvent>)> routeCallback_;
    std::atomic<int> publishCallCount_{0};

    // Behavior configuration
    bool shouldReturnEmptyOnPoll_ = false;
    bool shouldThrowOnWait_ = false;
    int throwCount_ = 0;
    int maxThrows_ = 1;
};

}  // namespace sdl::test

#endif  // SDL_TEST_MOCKS_MOCK_EVENT_BUS_H
