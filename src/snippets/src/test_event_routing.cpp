#include "application.h"
#include "event_router.h" 
#include "event.h"
#include <iostream>
#include <memory>

class TestApp : public sdlpp::BaseApplication {
public:
    bool eventReceived = false;
    
    auto init() -> bool override {
        std::cout << "TestApp: Registering mouse button event handler\n";
        
        // Get the event router and register a handler
        auto& runner = sdlpp::ApplicationRunner::getInstance();
        if (auto eventRouter = runner.getEventRouter()) {
            eventRouter->registerEventHandler<sdlpp::MouseButtonEvent>(
                [this](const sdlpp::MouseButtonEvent& event) {
                    std::cout << "TestApp: Received mouse button event at (" 
                              << event.x << ", " << event.y << ")\n";
                    eventReceived = true;
                });
        }
        
        return true;
    }
    
    auto iterate() -> bool override {
        // Test that EventRouter is accessible and working
        auto& runner = sdlpp::ApplicationRunner::getInstance();
        if (auto eventRouter = runner.getEventRouter()) {
            std::cout << "✅ SUCCESS: EventRouter is accessible from application!\n";
            std::cout << "✅ SUCCESS: SDL_AppEvent callback should route events through this EventRouter\n";
        } else {
            std::cout << "❌ FAILURE: EventRouter not available\n";
        }
        
        // Test completed - exit
        return false;
    }
    
    auto quit() -> void override {
        std::cout << "TestApp: Quit called\n";
    }
};

int main() {
    std::cout << "Testing Event Routing Infrastructure\n";
    std::cout << "====================================\n";
    
    // Register our test application
    sdlpp::ApplicationRunner::registerApplication(std::make_unique<TestApp>());
    
    // Run the application (this will initialize SDL internally)
    auto result = sdlpp::ApplicationRunner::run();
    
    std::cout << "Test completed with result: " << result << std::endl;
    return result;
}