#include <iostream>
#include <memory>

#include "sdl/application.h"
#include "sdl/event_router.h" 
#include "sdl/event.h"
#include "sdl/window.h"
#include "sdl/renderer.h"

class SDL_AppEventTestApp : public sdl::BaseApplication {
private:
    std::unique_ptr<sdl::Window> window_;
    std::unique_ptr<sdl::Renderer> renderer_;
    int frameCount_ = 0;
    bool eventReceived_ = false;
    
public:
    auto init() -> bool override {
        std::cout << "SDL_AppEventTestApp: Initializing...\n";
        
        // Create window and renderer
        try {
            window_ = std::make_unique<sdl::Window>("SDL AppEvent Test", 400, 300, 0);
            renderer_ = std::make_unique<sdl::Renderer>(*window_);
        } catch (const std::exception& e) {
            std::cout << "❌ FAILURE: Could not create window/renderer: " << e.what() << std::endl;
            return false;
        }
        
        // Register event handler to test that events from SDL_AppEvent reach EventRouter
        auto& runner = sdl::ApplicationRunner::getInstance();
        if (auto eventRouter = runner.getEventRouter()) {
            std::cout << "✅ SUCCESS: EventRouter accessible during init\n";
            
            eventRouter->registerEventHandler<sdl::MouseButtonEvent>(
                [this](const sdl::MouseButtonEvent& event) {
                    std::cout << "✅ SUCCESS: Received mouse event in application handler at (" 
                              << event.x << ", " << event.y << ")\n";
                    eventReceived_ = true;
                });
                
            eventRouter->registerEventHandler<sdl::QuitEvent>(
                [](const sdl::QuitEvent&) {
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
        renderer_->setDrawColour(sdl::Color(0, 0, 255, 255));  // Blue
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
    sdl::ApplicationRunner::registerApplication(std::make_unique<SDL_AppEventTestApp>());
    
    // Run the application (this will initialize SDL internally and call callbacks)
    auto result = sdl::ApplicationRunner::run();
    
    std::cout << "\nTest completed with result: " << result << std::endl;
    return result;
}
