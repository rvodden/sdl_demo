#include "message_box.h"
#include "include/message_box_impl.h"
#include "include/window_impl.h"
#include "window.h"
#include "exception.h"
#include "constexpr_map.h"

#include <SDL3/SDL_messagebox.h>
#include <algorithm>
#include <array>

namespace sdlpp {

MessageBoxImpl::MessageBoxImpl(std::string titleText, std::string messageText)
    : title(std::move(titleText)), message(std::move(messageText)) {
}

auto MessageBoxImpl::getSdlWindow() const -> SDL_Window* {
    if (parentWindow) {
        return parentWindow->getImpl().getSdlWindow();
    }
    return nullptr;
}

constexpr auto MessageBoxImpl::getTypeToFlagsMap() {
    constexpr auto typeToFlags = std::array{
        std::pair{MessageBox::Type::Information, SDL_MESSAGEBOX_INFORMATION},
        std::pair{MessageBox::Type::Warning, SDL_MESSAGEBOX_WARNING},
        std::pair{MessageBox::Type::Error, SDL_MESSAGEBOX_ERROR}
    };
    return vodden::Map(typeToFlags);
}

auto MessageBoxImpl::convertTypeToFlags(MessageBox::Type messageType) const -> uint32_t {
    constexpr auto typeMap = getTypeToFlagsMap();
    return typeMap[messageType];
}

auto MessageBoxImpl::show() -> MessageBox::Result {
    // If no buttons were added, add a default OK button
    if (buttons.empty()) {
        buttons.emplace_back("OK", 0);
        buttons.back().isDefault = true;
    }

    // Prepare SDL button data
    std::vector<SDL_MessageBoxButtonData> sdlButtons;
    sdlButtons.reserve(buttons.size());

    for (const auto& button : buttons) {
        SDL_MessageBoxButtonFlags flags = 0;
        if (button.isDefault) {
            flags |= SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
        }
        if (button.isCancel) {
            flags |= SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;  
        }

        sdlButtons.push_back({
            flags,
            button.id,
            button.text.c_str()
        });
    }

    // Prepare message box data
    SDL_MessageBoxData messageBoxData = {};
    messageBoxData.flags = convertTypeToFlags(type);
    messageBoxData.window = getSdlWindow();
    messageBoxData.title = title.c_str();
    messageBoxData.message = message.c_str();
    messageBoxData.numbuttons = static_cast<int>(sdlButtons.size());
    messageBoxData.buttons = sdlButtons.data();
    messageBoxData.colorScheme = nullptr; // Use system default

    int clickedButtonId = -1;
    bool success = SDL_ShowMessageBox(&messageBoxData, &clickedButtonId);

    MessageBox::Result result;
    result.success = success;
    result.buttonId = success ? clickedButtonId : -1;
    
    if (!success) {
        const char* error = SDL_GetError();
        result.error = error ? error : "Unknown error";
    }

    return result;
}

MessageBox::MessageBox(std::string title, std::string message)
    : _impl(std::make_unique<MessageBoxImpl>(std::move(title), std::move(message))) {
}

MessageBox::MessageBox(MessageBox&& other) noexcept
    : _impl(std::move(other._impl)) {
}

MessageBox::~MessageBox() = default;

auto MessageBox::operator=(MessageBox&& other) noexcept -> MessageBox& {
    if (this != &other) {
        _impl = std::move(other._impl);
    }
    return *this;
}

auto MessageBox::addButton(const std::string& text, int id) -> MessageBox& {
    int buttonId = (id == -1) ? _impl->nextButtonId++ : id;
    _impl->buttons.emplace_back(text, buttonId);
    
    // First button becomes default automatically
    if (_impl->buttons.size() == 1) {
        _impl->buttons.back().isDefault = true;
    }
    
    return *this;
}

auto MessageBox::setType(Type type) -> MessageBox& {
    _impl->type = type;
    return *this;
}

auto MessageBox::setParent(const Window* window) -> MessageBox& {
    _impl->parentWindow = window;
    return *this;
}

auto MessageBox::makeLastButtonDefault() -> MessageBox& {
    if (!_impl->buttons.empty()) {
        // Clear previous default
        for (auto& button : _impl->buttons) {
            button.isDefault = false;
        }
        // Set last button as default
        _impl->buttons.back().isDefault = true;
    }
    return *this;
}

auto MessageBox::makeLastButtonCancel() -> MessageBox& {
    if (!_impl->buttons.empty()) {
        // Clear previous cancel
        for (auto& button : _impl->buttons) {
            button.isCancel = false;
        }
        // Set last button as cancel
        _impl->buttons.back().isCancel = true;
    }
    return *this;
}

auto MessageBox::show() -> Result {
    return _impl->show();
}

} // namespace sdlpp