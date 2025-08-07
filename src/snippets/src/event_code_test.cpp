/**
 * @file event_code_test.cpp
 * @brief Test to verify that event codes are properly shared across instances
 * 
 * This test demonstrates that multiple instances of the same CustomUserEvent
 * type share the same event code, and that SDL_RegisterEvents is called
 * only once per type.
 */

#include <iostream>
#include <memory>
#include <chrono>

#include "event.h"
#include "user_event.h"

using namespace sdl;

class TestEvent1 : public CustomUserEvent<TestEvent1> {
  public:
    TestEvent1() : CustomUserEvent() {}
    TestEvent1(int d) : CustomUserEvent(), testData(d) {}
    
    int testData = 0;
  };

class TestEvent2 : public CustomUserEvent<TestEvent2> {
  public:
    TestEvent2() : CustomUserEvent() {}
    TestEvent2(float d) : CustomUserEvent(), testData(d) {}
    
    float testData = 0.0f;
};

int main() {
    std::cout << "Event Code Registration Test" << std::endl;
    std::cout << "============================" << std::endl;
    
    // Test 1: Multiple instances of same type should have same code
    std::cout << "\nTest 1: Multiple instances of same type" << std::endl;
    
    TestEvent1 event1a;
    TestEvent1 event1b(42);
    TestEvent1 event1c(100);
    
    std::cout << "TestEvent1 instance 1 code: " << event1a.getEventType() << std::endl;
    std::cout << "TestEvent1 instance 2 code: " << event1b.getEventType() << std::endl;
    std::cout << "TestEvent1 instance 3 code: " << event1c.getEventType() << std::endl;
    std::cout << "Static getEventCode(): " << TestEvent1::getEventType() << std::endl;
    
    bool test1_passed = (event1a.getEventType() == event1b.getEventType() && 
                        event1b.getEventType() == event1c.getEventType() &&
                        event1c.getEventType() == TestEvent1::getEventType());
    
    std::cout << "Test 1: " << (test1_passed ? "PASSED" : "FAILED") << std::endl;
    
    // Test 2: Different types should have different codes
    std::cout << "\nTest 2: Different types should have different codes" << std::endl;
    
    TestEvent2 event2a;
    TestEvent2 event2b(3.14f);
    
    std::cout << "TestEvent1 code: " << TestEvent1::getEventType() << std::endl;
    std::cout << "TestEvent2 instance 1 code: " << event2a.getEventType() << std::endl;
    std::cout << "TestEvent2 instance 2 code: " << event2b.getEventType() << std::endl;
    std::cout << "TestEvent2 static code: " << TestEvent2::getEventType() << std::endl;
    
    bool test2_passed = (event2a.getEventType() == event2b.getEventType() &&
                        event2b.getEventType() == TestEvent2::getEventType() &&
                        TestEvent1::getEventType() != TestEvent2::getEventType());
    
    std::cout << "Test 2: " << (test2_passed ? "PASSED" : "FAILED") << std::endl;
    
    // Test 3: Codes should be consecutive (32768, 32769, etc.)
    std::cout << "\nTest 3: Event codes should be sequential" << std::endl;
    uint32_t type1 = TestEvent1::getEventType();
    uint32_t type2 = TestEvent2::getEventType();
    
    // Note: We can't guarantee which type gets registered first, so check both possibilities
    bool test3_passed = ((type2 == type1 + 1) || (type1 == type2 + 1));
    
    std::cout << "Codes are sequential: " << (test3_passed ? "YES" : "NO") << std::endl;
    std::cout << "Test 3: " << (test3_passed ? "PASSED" : "FAILED") << std::endl;
    
    // Summary
    std::cout << "\n=== Test Summary ===" << std::endl;
    std::cout << "Test 1 (Same type, same code): " << (test1_passed ? "PASSED" : "FAILED") << std::endl;
    std::cout << "Test 2 (Different types, different codes): " << (test2_passed ? "PASSED" : "FAILED") << std::endl;
    std::cout << "Test 3 (Sequential allocation): " << (test3_passed ? "PASSED" : "FAILED") << std::endl;
    
    bool all_passed = test1_passed && test2_passed && test3_passed;
    std::cout << "\nAll tests: " << (all_passed ? "PASSED" : "FAILED") << std::endl;
    
    return all_passed ? 0 : 1;
}
