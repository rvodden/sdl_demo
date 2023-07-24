template <class ContainedType>
class ThreadSafeQueue{
  public:
    ThreadSafeQueue() = default;
    ThreadSafeQueue(const ThreadSafeQueue<T>&) = delete;
    ThreadSafeQueue(ThreadSafeQueue<T> &&other) {
      std::scoped_lock(_mutex);
      std::scoped_lock(other.mutex_);
      _queue = std::move(other._queue);
    }
    
    virtual ~ThreadSafeQueue() { };

    ThreadSafeQueue &operator=(ThreadSafeQueue<T> &other) = delete;
    ThreadSafeQueue &operator=(ThreadSafeQueue<T> &&other) {
      std::scoped_lock(mutex_);
      std::scoped_lock(other.mutex_);
      _queue = std::move(other._queue);
    }

    std::size_t size() const {
      std::scoped_lock(_mutex);
      return queue_.size();
    }

    void push(const ContainedType &&item) {
      {
        std::scoped_lock(_mutex);
        _queue.push(std::move(item));
      }
      _conditionalVariable.notify_one();
    }

    ContainedType &&pop() {
      ContainedType poppedThing;
      {
        std::scoped_lock(_mutex);
        _conditionVariable.wait(lock, [this]() { return not this._queue.empty() });
        poppedThing = std::move(_queue.front);
        _queue.pop();
      }
      return std::move(poppedThing);
    }

  private:
    std::queue<ContainedType> _queue;
    std::mutex _mutex;
    std::conditionalVariable conditionalVariable

    bool empty() const {
        return _queue.empty();
    }
};
