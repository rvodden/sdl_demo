/**
 * @file event_system_benchmark.cpp
 * @brief Performance benchmarks for the SDL event system
 */

#include <benchmark/benchmark.h>
#include <event.h>
#include <user_event.h>

#include <memory>
#include <vector>
#include <chrono>
#include <random>

using namespace sdl;

// Test event types for benchmarking
class BenchmarkEvent : public Event {
public:
    explicit BenchmarkEvent(int value) 
        : Event(std::chrono::milliseconds(0)), data(value) {}
    
    void handle(BaseEventHandler& baseEventHandler) override {
        castHandler(*this, baseEventHandler);
    }
    
    int data;
};

class LargeEvent : public Event {
public:
    explicit LargeEvent(const std::vector<int>& payload) 
        : Event(std::chrono::milliseconds(0)), largeData(payload) {}
    
    void handle(BaseEventHandler& baseEventHandler) override {
        castHandler(*this, baseEventHandler);
    }
    
    std::vector<int> largeData;
};

// Test handlers
class SimpleEventHandler : public EventHandler<BenchmarkEvent>, public BaseEventHandler {
public:
    void handle(const BenchmarkEvent& event) override {
        // Simple operation to prevent optimization
        volatile int result = event.data * 2;
        [[maybe_unused]] auto temp = result;
    }
};

class ComplexEventHandler : public EventHandler<BenchmarkEvent>, public BaseEventHandler {
public:
    void handle(const BenchmarkEvent& event) override {
        // More complex computation
        std::vector<int> temp;
        temp.reserve(100);
        for (int i = 0; i < event.data % 100; ++i) {
            temp.push_back(i * event.data);
        }
        result_ = temp.size();
    }
private:
    volatile size_t result_{0};
};

class MultiEventHandler : public EventHandler<BenchmarkEvent>, 
                         public EventHandler<LargeEvent>,
                         public BaseEventHandler {
public:
    void handle(const BenchmarkEvent& event) override {
        benchmarkCount_++;
        volatile int temp = event.data;
        [[maybe_unused]] auto result = temp;
    }
    
    void handle(const LargeEvent& event) override {
        largeCount_++;
        volatile size_t temp = event.largeData.size();
        [[maybe_unused]] auto result = temp;
    }
    
    size_t getBenchmarkCount() const { return benchmarkCount_; }
    size_t getLargeCount() const { return largeCount_; }

private:
    size_t benchmarkCount_{0};
    size_t largeCount_{0};
};

// Benchmark: Event creation overhead
static void BM_EventCreation_Simple(benchmark::State& state) {
    for (auto _ : state) {
        auto event = std::make_unique<BenchmarkEvent>(42);
        benchmark::DoNotOptimize(event);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_EventCreation_Simple);

// Benchmark: Event creation with large payload
static void BM_EventCreation_Large(benchmark::State& state) {
    std::vector<int> payload(state.range(0));
    std::iota(payload.begin(), payload.end(), 0);
    
    for (auto _ : state) {
        auto event = std::make_unique<LargeEvent>(payload);
        benchmark::DoNotOptimize(event);
    }
    state.SetItemsProcessed(state.iterations());
    state.SetBytesProcessed(state.iterations() * payload.size() * sizeof(int));
}
BENCHMARK(BM_EventCreation_Large)->Arg(100)->Arg(1000)->Arg(10000);

// Benchmark: Dynamic cast performance in event handling
static void BM_DynamicCast_EventHandler(benchmark::State& state) {
    auto event = std::make_unique<BenchmarkEvent>(42);
    SimpleEventHandler handler;
    BaseEventHandler& baseHandler = handler;
    
    for (auto _ : state) {
        // Simulate the castHandler function behavior
        try {
            auto& eventHandler = dynamic_cast<EventHandler<BenchmarkEvent>&>(baseHandler);
            eventHandler.handle(*event);
        } catch (const std::bad_cast&) {
            // Handler can't handle this event type
        }
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_DynamicCast_EventHandler);

// Benchmark: Event handling with multiple handlers
static void BM_EventHandling_MultipleHandlers(benchmark::State& state) {
    const int numHandlers = state.range(0);
    std::vector<std::unique_ptr<BaseEventHandler>> handlers;
    
    // Create multiple handlers
    for (int i = 0; i < numHandlers; ++i) {
        handlers.push_back(std::make_unique<SimpleEventHandler>());
    }
    
    auto event = std::make_unique<BenchmarkEvent>(42);
    
    for (auto _ : state) {
        // Dispatch to all handlers
        for (auto& handler : handlers) {
            castHandler(*event, *handler);
        }
    }
    
    state.SetItemsProcessed(state.iterations() * numHandlers);
    state.counters["Handlers"] = numHandlers;
}
BENCHMARK(BM_EventHandling_MultipleHandlers)
    ->Arg(1)->Arg(10)->Arg(100)->Arg(1000);

// Benchmark: Complex event handling
static void BM_EventHandling_Complex(benchmark::State& state) {
    ComplexEventHandler handler;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 100);
    
    for (auto _ : state) {
        auto event = std::make_unique<BenchmarkEvent>(dis(gen));
        castHandler(*event, handler);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_EventHandling_Complex);

// Benchmark: Event polymorphism overhead
static void BM_EventPolymorphism(benchmark::State& state) {
    std::vector<std::unique_ptr<BaseEvent>> events;
    MultiEventHandler handler;
    
    // Create mixed event types
    for (int i = 0; i < 1000; ++i) {
        if (i % 2 == 0) {
            events.push_back(std::make_unique<BenchmarkEvent>(i));
        } else {
            std::vector<int> data(10, i);
            events.push_back(std::make_unique<LargeEvent>(data));
        }
    }
    
    size_t eventIndex = 0;
    for (auto _ : state) {
        events[eventIndex % events.size()]->handle(handler);
        eventIndex++;
    }
    
    state.SetItemsProcessed(state.iterations());
    state.counters["BenchmarkEvents"] = handler.getBenchmarkCount();
    state.counters["LargeEvents"] = handler.getLargeCount();
}
BENCHMARK(BM_EventPolymorphism);

// Benchmark: Mouse button event creation (realistic scenario)
static void BM_MouseButtonEvent_Creation(benchmark::State& state) {
    using namespace std::chrono;
    
    for (auto _ : state) {
        auto event = std::make_unique<MouseButtonEvent>(
            duration_cast<milliseconds>(steady_clock::now().time_since_epoch()),
            1,  // window ID
            0,  // mouse ID
            100.0f, 200.0f,  // x, y
            MouseButtonEvent::Button::kLeft,
            true,  // down
            1      // clicks
        );
        benchmark::DoNotOptimize(event);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_MouseButtonEvent_Creation);

// Benchmark: Event memory allocation patterns
static void BM_EventAllocation_Batch(benchmark::State& state) {
    const int batchSize = state.range(0);
    
    for (auto _ : state) {
        std::vector<std::unique_ptr<BenchmarkEvent>> events;
        events.reserve(batchSize);
        
        for (int i = 0; i < batchSize; ++i) {
            events.push_back(std::make_unique<BenchmarkEvent>(i));
        }
        
        benchmark::DoNotOptimize(events);
    }
    
    state.SetItemsProcessed(state.iterations() * batchSize);
    state.counters["BatchSize"] = batchSize;
}
BENCHMARK(BM_EventAllocation_Batch)
    ->Arg(10)->Arg(100)->Arg(1000);