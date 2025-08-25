#include "application.h"
#include "event_router.h" 
#include "event.h"
#include "window.h"
#include "renderer.h"
#include <iostream>
#include <memory>

class SDL_AppEventTestApp : public sdlpp::BaseApplication {
private:
    std::unique_ptr<sdlpp::Window> window_;
    std::unique_ptr<sdlpp::Renderer> renderer_;
    int frameCount_ = 0;
    bool eventReceived_ = false;
    
public:
    auto init() -> bool override {
        std::cout << "SDL_AppEventTestApp: Initializing...\n";
        
        // Create window and renderer
        try {
            window_ = std::make_unique<sdlpp::Window>("SDL AppEvent Test", 400, 300, 0);
            renderer_ = std::make_unique<sdlpp::Renderer>(*window_);
        } catch (const std::exception& e) {
            std::cout << "❌ FAILURE: Could not create window/renderer: " << e.what() << std::endl;
            return false;
        }
        
        // Register event handler to test that events from SDL_AppEvent reach EventRouter
        auto& runner = sdlpp::ApplicationRunner::getInstance();
        if (auto eventRouter = runner.getEventRouter()) {
            std::cout << "✅ SUCCESS: EventRouter accessible during init\n";
            
            eventRouter->registerEventHandler<sdlpp::MouseButtonEvent>(
                [this](const sdlpp::MouseButtonEvent& event) {
                    std::cout << "✅ SUCCESS: Received mouse event in application handler at (" 
                              << event.x << ", " << event.y << ")\n";
                    eventReceived_ = true;
                });
                
            eventRouter->registerEventHandler<sdlpp::QuitEvent>(
                [](const sdlpp::QuitEvent&) {
                    std::cout << "✅ SUCCESS: Received quit event in application handler\n";
                });
        } else {
            std::cout << "❌ FAILURE: EventRouter not available during init\n";
            return false;
        }
        
        std::cout << "✅ SUCCESS: Initialization complete\n";
        return true;
    }
    
    auto iterate() -> bool override {
        frameCount_++;
        
        // Render something simple
        renderer_->setRenderDrawColour(sdlpp::Color(0, 0, 255, 255));  // Blue
        renderer_->clear();
        renderer_->present();
        
        // Show status every 60 frames
        if (frameCount_ % 60 == 0) {
            std::cout << "Frame " << frameCount_ << " - Events through EventRouter: " 
                      << (eventReceived_ ? "YES" : "NO") << std::endl;
        }
        
        // Exit after a reasonable test time
        if (frameCount_ > 300) {  // ~5 seconds at 60fps
            std::cout << "Test completed after " << frameCount_ << " frames\n";
            return false;
        }
        
        return true;  // Continue running
    }
    
    auto quit() -> void override {
        std::cout << "SDL_AppEventTestApp: Cleaning up...\n";
        renderer_.reset();
        window_.reset();
    }
};

int main() {
    std::cout << "Testing SDL_AppEvent Integration\n";
    std::cout << "=================================\n";
    std::cout << "This test creates a window and tests event routing from SDL_AppEvent callback.\n";
    std::cout << "Click in the window or press ESC to generate events.\n\n";
    
    // Register our test application
    sdlpp::ApplicationRunner::registerApplication(std::make_unique<SDL_AppEventTestApp>());
    
    // Run the application (this will initialize SDL internally and call callbacks)
    auto result = sdlpp::ApplicationRunner::run();
    
    std::cout << "\nTest completed with result: " << result << std::endl;
    return result;
}