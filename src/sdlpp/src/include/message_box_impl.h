#ifndef SDLPP_MESSAGE_BOX_IMPL_H
#define SDLPP_MESSAGE_BOX_IMPL_H

#include <string>
#include <vector>
#include "message_box.h"

struct SDL_Window;

namespace sdlpp {

class MessageBoxImpl {
public:
    std::string title;
    std::string message; 
    std::vector<MessageBox::Button> buttons;
    MessageBox::Type type = MessageBox::Type::Information;
    const Window* parentWindow = nullptr;
    int nextButtonId = 0;

    MessageBoxImpl(std::string titleText, std::string messageText);
    ~MessageBoxImpl() = default;

    auto show() -> MessageBox::Result;

private:
    auto getSdlWindow() const -> SDL_Window*;
    static constexpr auto getTypeToFlagsMap();
    auto convertTypeToFlags(MessageBox::Type messageType) const -> uint32_t;
};

} // namespace sdlpp

#endif // SDLPP_MESSAGE_BOX_IMPL_H