#include <iostream>
#include <memory>
#include <string>
#include <cstring>

#include <sdl/application.h>
#include <sdl/color.h>
#include <sdl/event.h>
#include <sdl/event_router.h>
#include <sdl/font.h>
#include <sdl/renderer.h>
#include <sdl/sdl.h>
#include <sdl/text.h>
#include <sdl/ttf.h>
#include <sdl/ttf_service.h>
#include <sdl/window.h>

#include "tiny_ttf.h"

using namespace sdl;
using namespace sdl::ttf;

const uint32_t kScreenWidth = 800;
const uint32_t kScreenHeight = 600;
const float kFontSize = 18.0F;

class KeyboardEventExample : public BaseApplication {
public:
    auto init() -> bool override {
        std::cout << "KeyboardEventExample: Initializing...\n";
        
        // Initialize SDL subsystems
        auto& sdl = requestSDL();
        [[maybe_unused]] auto& ttf = requestService<TTF>();
        sdl.initSubSystem(SDL::kVideo);
        sdl.initSubSystem(SDL::kEvents);
        
        // Create window and renderer
        try {
            window_ = std::make_unique<Window>("Keyboard Event Example", kScreenWidth, kScreenHeight, 0);
            renderer_ = std::make_unique<Renderer>(*window_);
        } catch (const std::exception& e) {
            std::cout << "Failed to create window/renderer: " << e.what() << std::endl;
            return false;
        }
        
        // Create font
        font_ = std::make_unique<Font>(tiny_ttf.data(), tiny_ttf.size(), kFontSize);
        
        // Register keyboard event handler
        auto& runner = ApplicationRunner::getInstance();
        if (auto eventRouter = runner.getEventRouter()) {
            eventRouter->registerEventHandler<KeyboardEvent>(
                [this](const KeyboardEvent& event) {
                    if (event.down) {  // Only handle key down events, not key up
                        handleKeyPress(event);
                    }
                });
        } else {
            std::cout << "EventRouter not available during init" << std::endl;
            return false;
        }
        
        // Initial message
        currentKeyText_ = "Press any key...";
        updateTextTexture();
        
        std::cout << "Initialization complete" << std::endl;
        return true;
    }
    
    auto iterate() -> bool override {
        // Clear screen with black background
        renderer_->setDrawColour(NamedColor::kBlack);
        renderer_->clear();
        
        // Render current key text in center of window
        if (textTexture_) {
            auto textureSize = textTexture_->getSize();
            auto renderPosition = Rectangle<float>{
                (static_cast<float>(kScreenWidth) - textureSize.getWidth()) / 2.0F,
                (static_cast<float>(kScreenHeight) - textureSize.getHeight()) / 2.0F,
                textureSize.getWidth(),
                textureSize.getHeight()
            };
            renderer_->copy(*textTexture_, renderPosition);
        }
        
        renderer_->present();
        return true;
    }
    
    auto quit() -> void override {
        std::cout << "KeyboardEventExample: Cleaning up..." << std::endl;
        textTexture_.reset();
        font_.reset();
        renderer_.reset();
        window_.reset();
    }
    
private:
    void handleKeyPress(const KeyboardEvent& event) {
        // Use the event's built-in key name method
        std::string keyName = event.getKeyName();
        if (!keyName.empty()) {
            currentKeyText_ = std::string("Key pressed: ") + keyName;
        } else {
            currentKeyText_ = std::string("Key pressed: Unknown");
        }
        
        std::cout << currentKeyText_ << std::endl;
        updateTextTexture();
    }
    
    void updateTextTexture() {
        try {
            auto text = Text::renderBlended(*font_, currentKeyText_, NamedColor::kWhite);
            textTexture_ = std::make_unique<Texture>(*renderer_, text);
        } catch (const std::exception& e) {
            std::cout << "Failed to update text texture: " << e.what() << std::endl;
        }
    }
    
    std::unique_ptr<Window> window_;
    std::unique_ptr<Renderer> renderer_;
    std::unique_ptr<Font> font_;
    std::unique_ptr<Texture> textTexture_;
    std::string currentKeyText_;
};

REGISTER_APPLICATION(KeyboardEventExample)