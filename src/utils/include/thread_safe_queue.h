#include <condition_variable>
#include <mutex>
#include <queue>

template <class ContainedType>
class ThreadSafeQueue {
 public:
  ThreadSafeQueue() = default;
  ThreadSafeQueue(const ThreadSafeQueue<ContainedType>&) = delete;
  ThreadSafeQueue(ThreadSafeQueue<ContainedType>&& other) noexcept {
    std::scoped_lock(_mutex);
    std::scoped_lock(other.mutex_);
    _queue = std::move(other._queue);
  }

  virtual ~ThreadSafeQueue() {};

  auto operator=(const ThreadSafeQueue<ContainedType>& other) -> ThreadSafeQueue& = delete;
  auto operator=(ThreadSafeQueue<ContainedType>&& other) noexcept -> ThreadSafeQueue& {
    std::scoped_lock(mutex_);
    std::scoped_lock(other.mutex_);
    _queue = std::move(other._queue);
  }

  [[nodiscard]] auto size() const -> std::size_t {
    std::scoped_lock(_mutex);
    return queue_.size();
  }

  void push(const ContainedType&& item) {
    {
      std::scoped_lock(_mutex);
      _queue.push(std::move(item));
    }
    _conditionalVariable.notify_one();
  }

  auto pop() -> ContainedType&& {
    ContainedType poppedThing;
    {
      std::scoped_lock(_mutex);
      _conditionVariable.wait(lock,
                              [this]() { return not this._queue.empty() });
      poppedThing = std::move(_queue.front);
      _queue.pop();
    }
    return std::move(poppedThing);
  }

 private:
  std::queue<ContainedType> _queue {};
  std::mutex _mutex {};
  std::condition_variable conditionalVariable;

  [[nodiscard]] auto empty() const -> bool {
    return _queue.empty();
  }
};
