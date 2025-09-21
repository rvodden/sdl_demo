#include <iostream>
#include <memory>
#include <string>
#include <cstring>

#include <sdl/sdl.h>
#include <sdl/sdl_tools.h>
#include <sdl/ttf.h>

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
        _window = std::make_unique<Window>("Keyboard Event Example", kScreenWidth, kScreenHeight, 0);
        _renderer = std::make_unique<Renderer>(*_window);
        
        // Create font
        _font = std::make_unique<Font>(kTinyTTF.data(), kTinyTTF.size(), kFontSize);
        
        // Register keyboard event handler
        if (auto eventRouter = getEventRouter()) {
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
        _renderer->setDrawColour(NamedColor::kBlack);
        _renderer->clear();
        
        // Render current key text in center of window
        if (textTexture_) {
            auto textureSize = textTexture_->getSize();
            auto renderPosition = Rectangle<float>{
                (static_cast<float>(kScreenWidth) - textureSize.getWidth()) / 2.0F,
                (static_cast<float>(kScreenHeight) - textureSize.getHeight()) / 2.0F,
                textureSize.getWidth(),
                textureSize.getHeight()
            };
            _renderer->copy(*textTexture_, renderPosition);
        }
        
        _renderer->present();
        return true;
    }
    
    auto quit() -> void override {
        std::cout << "KeyboardEventExample: Cleaning up..." << std::endl;
        textTexture_.reset();
        _font.reset();
        _renderer.reset();
        _window.reset();
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
            auto text = Text::renderBlended(*_font, currentKeyText_, NamedColor::kWhite);
            textTexture_ = std::make_unique<Texture>(*_renderer, text);
        } catch (const std::exception& e) {
            std::cout << "Failed to update text texture: " << e.what() << std::endl;
        }
    }
    
    std::unique_ptr<Window> _window;
    std::unique_ptr<Renderer> _renderer;
    std::unique_ptr<Font> _font;
    std::unique_ptr<Texture> textTexture_;
    std::string currentKeyText_;
};

REGISTER_APPLICATION(KeyboardEventExample)
