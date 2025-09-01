#include <any>
#include <atomic>
#include <chrono>
#include <forward_list>
#include <functional>
#include <typeindex>
#include <vector>

#include <SDL3/SDL.h>

#include "sdl/constexpr_map.h"
#include "sdl/event.h"
#include "sdl/user_event.h"

namespace sdl {

static constexpr auto kSdlMouseButtonEventButtonMapArray = std::array{
    std::pair{SDL_BUTTON_LEFT, MouseButtonEvent::Button::kLeft},
    std::pair{SDL_BUTTON_MIDDLE, MouseButtonEvent::Button::kMiddle},
    std::pair{SDL_BUTTON_RIGHT, MouseButtonEvent::Button::kRight},
    std::pair{SDL_BUTTON_X1, MouseButtonEvent::Button::kX1},
    std::pair{SDL_BUTTON_X2, MouseButtonEvent::Button::kX2}
};
static constexpr auto kSdlMouseButtonEventButtonMap = Map(kSdlMouseButtonEventButtonMapArray);

static constexpr auto kSdlScancodeMapArray = std::array{
        std::pair{SDL_SCANCODE_UNKNOWN, sdl::ScanCode::kUnknown},
        // Letters
        std::pair{SDL_SCANCODE_A, sdl::ScanCode::kA},
        std::pair{SDL_SCANCODE_B, sdl::ScanCode::kB},
        std::pair{SDL_SCANCODE_C, sdl::ScanCode::kC},
        std::pair{SDL_SCANCODE_D, sdl::ScanCode::kD},
        std::pair{SDL_SCANCODE_E, sdl::ScanCode::kE},
        std::pair{SDL_SCANCODE_F, sdl::ScanCode::kF},
        std::pair{SDL_SCANCODE_G, sdl::ScanCode::kG},
        std::pair{SDL_SCANCODE_H, sdl::ScanCode::kH},
        std::pair{SDL_SCANCODE_I, sdl::ScanCode::kI},
        std::pair{SDL_SCANCODE_J, sdl::ScanCode::kJ},
        std::pair{SDL_SCANCODE_K, sdl::ScanCode::kK},
        std::pair{SDL_SCANCODE_L, sdl::ScanCode::kL},
        std::pair{SDL_SCANCODE_M, sdl::ScanCode::kM},
        std::pair{SDL_SCANCODE_N, sdl::ScanCode::kN},
        std::pair{SDL_SCANCODE_O, sdl::ScanCode::kO},
        std::pair{SDL_SCANCODE_P, sdl::ScanCode::kP},
        std::pair{SDL_SCANCODE_Q, sdl::ScanCode::kQ},
        std::pair{SDL_SCANCODE_R, sdl::ScanCode::kR},
        std::pair{SDL_SCANCODE_S, sdl::ScanCode::kS},
        std::pair{SDL_SCANCODE_T, sdl::ScanCode::kT},
        std::pair{SDL_SCANCODE_U, sdl::ScanCode::kU},
        std::pair{SDL_SCANCODE_V, sdl::ScanCode::kV},
        std::pair{SDL_SCANCODE_W, sdl::ScanCode::kW},
        std::pair{SDL_SCANCODE_X, sdl::ScanCode::kX},
        std::pair{SDL_SCANCODE_Y, sdl::ScanCode::kY},
        std::pair{SDL_SCANCODE_Z, sdl::ScanCode::kZ},
        // Numbers
        std::pair{SDL_SCANCODE_1, sdl::ScanCode::k1},
        std::pair{SDL_SCANCODE_2, sdl::ScanCode::k2},
        std::pair{SDL_SCANCODE_3, sdl::ScanCode::k3},
        std::pair{SDL_SCANCODE_4, sdl::ScanCode::k4},
        std::pair{SDL_SCANCODE_5, sdl::ScanCode::k5},
        std::pair{SDL_SCANCODE_6, sdl::ScanCode::k6},
        std::pair{SDL_SCANCODE_7, sdl::ScanCode::k7},
        std::pair{SDL_SCANCODE_8, sdl::ScanCode::k8},
        std::pair{SDL_SCANCODE_9, sdl::ScanCode::k9},
        std::pair{SDL_SCANCODE_0, sdl::ScanCode::k0},
        // Common keys
        std::pair{SDL_SCANCODE_RETURN, sdl::ScanCode::kReturn},
        std::pair{SDL_SCANCODE_ESCAPE, sdl::ScanCode::kEscape},
        std::pair{SDL_SCANCODE_BACKSPACE, sdl::ScanCode::kBackspace},
        std::pair{SDL_SCANCODE_TAB, sdl::ScanCode::kTab},
        std::pair{SDL_SCANCODE_SPACE, sdl::ScanCode::kSpace},
        std::pair{SDL_SCANCODE_MINUS, sdl::ScanCode::kMinus},
        std::pair{SDL_SCANCODE_EQUALS, sdl::ScanCode::kEquals},
        std::pair{SDL_SCANCODE_LEFTBRACKET, sdl::ScanCode::kLeftBracket},
        std::pair{SDL_SCANCODE_RIGHTBRACKET, sdl::ScanCode::kRightBracket},
        std::pair{SDL_SCANCODE_BACKSLASH, sdl::ScanCode::kBackslash},
        std::pair{SDL_SCANCODE_NONUSHASH, sdl::ScanCode::kNonUsHash},
        std::pair{SDL_SCANCODE_SEMICOLON, sdl::ScanCode::kSemicolon},
        std::pair{SDL_SCANCODE_APOSTROPHE, sdl::ScanCode::kApostrophe},
        std::pair{SDL_SCANCODE_GRAVE, sdl::ScanCode::kGrave},
        std::pair{SDL_SCANCODE_COMMA, sdl::ScanCode::kComma},
        std::pair{SDL_SCANCODE_PERIOD, sdl::ScanCode::kPeriod},
        std::pair{SDL_SCANCODE_SLASH, sdl::ScanCode::kSlash},
        std::pair{SDL_SCANCODE_CAPSLOCK, sdl::ScanCode::kCapsLock},
        // Function keys
        std::pair{SDL_SCANCODE_F1, sdl::ScanCode::kF1},
        std::pair{SDL_SCANCODE_F2, sdl::ScanCode::kF2},
        std::pair{SDL_SCANCODE_F3, sdl::ScanCode::kF3},
        std::pair{SDL_SCANCODE_F4, sdl::ScanCode::kF4},
        std::pair{SDL_SCANCODE_F5, sdl::ScanCode::kF5},
        std::pair{SDL_SCANCODE_F6, sdl::ScanCode::kF6},
        std::pair{SDL_SCANCODE_F7, sdl::ScanCode::kF7},
        std::pair{SDL_SCANCODE_F8, sdl::ScanCode::kF8},
        std::pair{SDL_SCANCODE_F9, sdl::ScanCode::kF9},
        std::pair{SDL_SCANCODE_F10, sdl::ScanCode::kF10},
        std::pair{SDL_SCANCODE_F11, sdl::ScanCode::kF11},
        std::pair{SDL_SCANCODE_F12, sdl::ScanCode::kF12},
        std::pair{SDL_SCANCODE_F13, sdl::ScanCode::kF13},
        std::pair{SDL_SCANCODE_F14, sdl::ScanCode::kF14},
        std::pair{SDL_SCANCODE_F15, sdl::ScanCode::kF15},
        std::pair{SDL_SCANCODE_F16, sdl::ScanCode::kF16},
        std::pair{SDL_SCANCODE_F17, sdl::ScanCode::kF17},
        std::pair{SDL_SCANCODE_F18, sdl::ScanCode::kF18},
        std::pair{SDL_SCANCODE_F19, sdl::ScanCode::kF19},
        std::pair{SDL_SCANCODE_F20, sdl::ScanCode::kF20},
        std::pair{SDL_SCANCODE_F21, sdl::ScanCode::kF21},
        std::pair{SDL_SCANCODE_F22, sdl::ScanCode::kF22},
        std::pair{SDL_SCANCODE_F23, sdl::ScanCode::kF23},
        std::pair{SDL_SCANCODE_F24, sdl::ScanCode::kF24},
        // Navigation and editing keys
        std::pair{SDL_SCANCODE_PRINTSCREEN, sdl::ScanCode::kPrintScreen},
        std::pair{SDL_SCANCODE_SCROLLLOCK, sdl::ScanCode::kScrollLock},
        std::pair{SDL_SCANCODE_PAUSE, sdl::ScanCode::kPause},
        std::pair{SDL_SCANCODE_INSERT, sdl::ScanCode::kInsert},
        std::pair{SDL_SCANCODE_HOME, sdl::ScanCode::kHome},
        std::pair{SDL_SCANCODE_PAGEUP, sdl::ScanCode::kPageUp},
        std::pair{SDL_SCANCODE_DELETE, sdl::ScanCode::kDelete},
        std::pair{SDL_SCANCODE_END, sdl::ScanCode::kEnd},
        std::pair{SDL_SCANCODE_PAGEDOWN, sdl::ScanCode::kPageDown},
        std::pair{SDL_SCANCODE_RIGHT, sdl::ScanCode::kRight},
        std::pair{SDL_SCANCODE_LEFT, sdl::ScanCode::kLeft},
        std::pair{SDL_SCANCODE_DOWN, sdl::ScanCode::kDown},
        std::pair{SDL_SCANCODE_UP, sdl::ScanCode::kUp},
        // Keypad keys
        std::pair{SDL_SCANCODE_NUMLOCKCLEAR, sdl::ScanCode::kNumLockClear},
        std::pair{SDL_SCANCODE_KP_DIVIDE, sdl::ScanCode::kKpDivide},
        std::pair{SDL_SCANCODE_KP_MULTIPLY, sdl::ScanCode::kKpMultiply},
        std::pair{SDL_SCANCODE_KP_MINUS, sdl::ScanCode::kKpMinus},
        std::pair{SDL_SCANCODE_KP_PLUS, sdl::ScanCode::kKpPlus},
        std::pair{SDL_SCANCODE_KP_ENTER, sdl::ScanCode::kKpEnter},
        std::pair{SDL_SCANCODE_KP_1, sdl::ScanCode::kKp1},
        std::pair{SDL_SCANCODE_KP_2, sdl::ScanCode::kKp2},
        std::pair{SDL_SCANCODE_KP_3, sdl::ScanCode::kKp3},
        std::pair{SDL_SCANCODE_KP_4, sdl::ScanCode::kKp4},
        std::pair{SDL_SCANCODE_KP_5, sdl::ScanCode::kKp5},
        std::pair{SDL_SCANCODE_KP_6, sdl::ScanCode::kKp6},
        std::pair{SDL_SCANCODE_KP_7, sdl::ScanCode::kKp7},
        std::pair{SDL_SCANCODE_KP_8, sdl::ScanCode::kKp8},
        std::pair{SDL_SCANCODE_KP_9, sdl::ScanCode::kKp9},
        std::pair{SDL_SCANCODE_KP_0, sdl::ScanCode::kKp0},
        std::pair{SDL_SCANCODE_KP_PERIOD, sdl::ScanCode::kKpPeriod},
        std::pair{SDL_SCANCODE_KP_EQUALS, sdl::ScanCode::kKpEquals},
        // Special keys (commonly used subset)
        std::pair{SDL_SCANCODE_NONUSBACKSLASH, sdl::ScanCode::kNonUsBackslash},
        std::pair{SDL_SCANCODE_APPLICATION, sdl::ScanCode::kApplication},
        std::pair{SDL_SCANCODE_POWER, sdl::ScanCode::kPower},
        std::pair{SDL_SCANCODE_EXECUTE, sdl::ScanCode::kExecute},
        std::pair{SDL_SCANCODE_HELP, sdl::ScanCode::kHelp},
        std::pair{SDL_SCANCODE_MENU, sdl::ScanCode::kMenu},
        std::pair{SDL_SCANCODE_SELECT, sdl::ScanCode::kSelect},
        std::pair{SDL_SCANCODE_STOP, sdl::ScanCode::kStop},
        std::pair{SDL_SCANCODE_AGAIN, sdl::ScanCode::kAgain},
        std::pair{SDL_SCANCODE_UNDO, sdl::ScanCode::kUndo},
        std::pair{SDL_SCANCODE_CUT, sdl::ScanCode::kCut},
        std::pair{SDL_SCANCODE_COPY, sdl::ScanCode::kCopy},
        std::pair{SDL_SCANCODE_PASTE, sdl::ScanCode::kPaste},
        std::pair{SDL_SCANCODE_FIND, sdl::ScanCode::kFind},
        std::pair{SDL_SCANCODE_MUTE, sdl::ScanCode::kMute},
        std::pair{SDL_SCANCODE_VOLUMEUP, sdl::ScanCode::kVolumeUp},
        std::pair{SDL_SCANCODE_VOLUMEDOWN, sdl::ScanCode::kVolumeDown},
        std::pair{SDL_SCANCODE_CLEARAGAIN, sdl::ScanCode::kClearAgain},
        // Modifier keys
        std::pair{SDL_SCANCODE_LCTRL, sdl::ScanCode::kLCtrl},
        std::pair{SDL_SCANCODE_LSHIFT, sdl::ScanCode::kLShift},
        std::pair{SDL_SCANCODE_LALT, sdl::ScanCode::kLAlt},
        std::pair{SDL_SCANCODE_LGUI, sdl::ScanCode::kLGui},
        std::pair{SDL_SCANCODE_RCTRL, sdl::ScanCode::kRCtrl},
        std::pair{SDL_SCANCODE_RSHIFT, sdl::ScanCode::kRShift},
        std::pair{SDL_SCANCODE_RALT, sdl::ScanCode::kRAlt},
        std::pair{SDL_SCANCODE_RGUI, sdl::ScanCode::kRGui},
        std::pair{SDL_SCANCODE_MODE, sdl::ScanCode::kMode},
        // Media keys (subset)
        std::pair{SDL_SCANCODE_MEDIA_PLAY, sdl::ScanCode::kMediaPlay},
        std::pair{SDL_SCANCODE_MEDIA_PAUSE, sdl::ScanCode::kMediaPause},
        std::pair{SDL_SCANCODE_MEDIA_STOP, sdl::ScanCode::kMediaStop},
        std::pair{SDL_SCANCODE_MEDIA_NEXT_TRACK, sdl::ScanCode::kMediaNextTrack},
        std::pair{SDL_SCANCODE_MEDIA_PREVIOUS_TRACK, sdl::ScanCode::kMediaPreviousTrack}
};
static constexpr auto kSdlScancodeMap = Map(kSdlScancodeMapArray);

static constexpr auto kSdlKeycodeMapArray = std::array{
        std::pair{SDLK_UNKNOWN, sdl::KeyCode::kUnknown},
        // Basic ASCII characters
        std::pair{SDLK_RETURN, sdl::KeyCode::kReturn},
        std::pair{SDLK_ESCAPE, sdl::KeyCode::kEscape},
        std::pair{SDLK_BACKSPACE, sdl::KeyCode::kBackspace},
        std::pair{SDLK_TAB, sdl::KeyCode::kTab},
        std::pair{SDLK_SPACE, sdl::KeyCode::kSpace},
        std::pair{SDLK_EXCLAIM, sdl::KeyCode::kExclaim},
        std::pair{SDLK_DBLAPOSTROPHE, sdl::KeyCode::kDblApostrophe},
        std::pair{SDLK_HASH, sdl::KeyCode::kHash},
        std::pair{SDLK_DOLLAR, sdl::KeyCode::kDollar},
        std::pair{SDLK_PERCENT, sdl::KeyCode::kPercent},
        std::pair{SDLK_AMPERSAND, sdl::KeyCode::kAmpersand},
        std::pair{SDLK_APOSTROPHE, sdl::KeyCode::kApostrophe},
        std::pair{SDLK_LEFTPAREN, sdl::KeyCode::kLeftParen},
        std::pair{SDLK_RIGHTPAREN, sdl::KeyCode::kRightParen},
        std::pair{SDLK_ASTERISK, sdl::KeyCode::kAsterisk},
        std::pair{SDLK_PLUS, sdl::KeyCode::kPlus},
        std::pair{SDLK_COMMA, sdl::KeyCode::kComma},
        std::pair{SDLK_MINUS, sdl::KeyCode::kMinus},
        std::pair{SDLK_PERIOD, sdl::KeyCode::kPeriod},
        std::pair{SDLK_SLASH, sdl::KeyCode::kSlash},
        // Numbers
        std::pair{SDLK_0, sdl::KeyCode::k0},
        std::pair{SDLK_1, sdl::KeyCode::k1},
        std::pair{SDLK_2, sdl::KeyCode::k2},
        std::pair{SDLK_3, sdl::KeyCode::k3},
        std::pair{SDLK_4, sdl::KeyCode::k4},
        std::pair{SDLK_5, sdl::KeyCode::k5},
        std::pair{SDLK_6, sdl::KeyCode::k6},
        std::pair{SDLK_7, sdl::KeyCode::k7},
        std::pair{SDLK_8, sdl::KeyCode::k8},
        std::pair{SDLK_9, sdl::KeyCode::k9},
        // More symbols
        std::pair{SDLK_COLON, sdl::KeyCode::kColon},
        std::pair{SDLK_SEMICOLON, sdl::KeyCode::kSemicolon},
        std::pair{SDLK_LESS, sdl::KeyCode::kLess},
        std::pair{SDLK_EQUALS, sdl::KeyCode::kEquals},
        std::pair{SDLK_GREATER, sdl::KeyCode::kGreater},
        std::pair{SDLK_QUESTION, sdl::KeyCode::kQuestion},
        std::pair{SDLK_AT, sdl::KeyCode::kAt},
        // Letters (lowercase)
        std::pair{SDLK_A, sdl::KeyCode::kA},
        std::pair{SDLK_B, sdl::KeyCode::kB},
        std::pair{SDLK_C, sdl::KeyCode::kC},
        std::pair{SDLK_D, sdl::KeyCode::kD},
        std::pair{SDLK_E, sdl::KeyCode::kE},
        std::pair{SDLK_F, sdl::KeyCode::kF},
        std::pair{SDLK_G, sdl::KeyCode::kG},
        std::pair{SDLK_H, sdl::KeyCode::kH},
        std::pair{SDLK_I, sdl::KeyCode::kI},
        std::pair{SDLK_J, sdl::KeyCode::kJ},
        std::pair{SDLK_K, sdl::KeyCode::kK},
        std::pair{SDLK_L, sdl::KeyCode::kL},
        std::pair{SDLK_M, sdl::KeyCode::kM},
        std::pair{SDLK_N, sdl::KeyCode::kN},
        std::pair{SDLK_O, sdl::KeyCode::kO},
        std::pair{SDLK_P, sdl::KeyCode::kP},
        std::pair{SDLK_Q, sdl::KeyCode::kQ},
        std::pair{SDLK_R, sdl::KeyCode::kR},
        std::pair{SDLK_S, sdl::KeyCode::kS},
        std::pair{SDLK_T, sdl::KeyCode::kT},
        std::pair{SDLK_U, sdl::KeyCode::kU},
        std::pair{SDLK_V, sdl::KeyCode::kV},
        std::pair{SDLK_W, sdl::KeyCode::kW},
        std::pair{SDLK_X, sdl::KeyCode::kX},
        std::pair{SDLK_Y, sdl::KeyCode::kY},
        std::pair{SDLK_Z, sdl::KeyCode::kZ},
        // Brackets and symbols
        std::pair{SDLK_LEFTBRACKET, sdl::KeyCode::kLeftBracket},
        std::pair{SDLK_BACKSLASH, sdl::KeyCode::kBackslash},
        std::pair{SDLK_RIGHTBRACKET, sdl::KeyCode::kRightBracket},
        std::pair{SDLK_CARET, sdl::KeyCode::kCaret},
        std::pair{SDLK_UNDERSCORE, sdl::KeyCode::kUnderscore},
        std::pair{SDLK_GRAVE, sdl::KeyCode::kGrave},
        std::pair{SDLK_LEFTBRACE, sdl::KeyCode::kLeftBrace},
        std::pair{SDLK_PIPE, sdl::KeyCode::kPipe},
        std::pair{SDLK_RIGHTBRACE, sdl::KeyCode::kRightBrace},
        std::pair{SDLK_TILDE, sdl::KeyCode::kTilde},
        std::pair{SDLK_DELETE, sdl::KeyCode::kDelete},
        // Special characters
        std::pair{SDLK_PLUSMINUS, sdl::KeyCode::kPlusMinus},
        // Function keys
        std::pair{SDLK_CAPSLOCK, sdl::KeyCode::kCapsLock},
        std::pair{SDLK_F1, sdl::KeyCode::kF1},
        std::pair{SDLK_F2, sdl::KeyCode::kF2},
        std::pair{SDLK_F3, sdl::KeyCode::kF3},
        std::pair{SDLK_F4, sdl::KeyCode::kF4},
        std::pair{SDLK_F5, sdl::KeyCode::kF5},
        std::pair{SDLK_F6, sdl::KeyCode::kF6},
        std::pair{SDLK_F7, sdl::KeyCode::kF7},
        std::pair{SDLK_F8, sdl::KeyCode::kF8},
        std::pair{SDLK_F9, sdl::KeyCode::kF9},
        std::pair{SDLK_F10, sdl::KeyCode::kF10},
        std::pair{SDLK_F11, sdl::KeyCode::kF11},
        std::pair{SDLK_F12, sdl::KeyCode::kF12},
        std::pair{SDLK_F13, sdl::KeyCode::kF13},
        std::pair{SDLK_F14, sdl::KeyCode::kF14},
        std::pair{SDLK_F15, sdl::KeyCode::kF15},
        std::pair{SDLK_F16, sdl::KeyCode::kF16},
        std::pair{SDLK_F17, sdl::KeyCode::kF17},
        std::pair{SDLK_F18, sdl::KeyCode::kF18},
        std::pair{SDLK_F19, sdl::KeyCode::kF19},
        std::pair{SDLK_F20, sdl::KeyCode::kF20},
        std::pair{SDLK_F21, sdl::KeyCode::kF21},
        std::pair{SDLK_F22, sdl::KeyCode::kF22},
        std::pair{SDLK_F23, sdl::KeyCode::kF23},
        std::pair{SDLK_F24, sdl::KeyCode::kF24},
        // Navigation and editing keys
        std::pair{SDLK_PRINTSCREEN, sdl::KeyCode::kPrintScreen},
        std::pair{SDLK_SCROLLLOCK, sdl::KeyCode::kScrollLock},
        std::pair{SDLK_PAUSE, sdl::KeyCode::kPause},
        std::pair{SDLK_INSERT, sdl::KeyCode::kInsert},
        std::pair{SDLK_HOME, sdl::KeyCode::kHome},
        std::pair{SDLK_PAGEUP, sdl::KeyCode::kPageUp},
        std::pair{SDLK_END, sdl::KeyCode::kEnd},
        std::pair{SDLK_PAGEDOWN, sdl::KeyCode::kPageDown},
        std::pair{SDLK_RIGHT, sdl::KeyCode::kRight},
        std::pair{SDLK_LEFT, sdl::KeyCode::kLeft},
        std::pair{SDLK_DOWN, sdl::KeyCode::kDown},
        std::pair{SDLK_UP, sdl::KeyCode::kUp},
        // Keypad keys
        std::pair{SDLK_NUMLOCKCLEAR, sdl::KeyCode::kNumLockClear},
        std::pair{SDLK_KP_DIVIDE, sdl::KeyCode::kKpDivide},
        std::pair{SDLK_KP_MULTIPLY, sdl::KeyCode::kKpMultiply},
        std::pair{SDLK_KP_MINUS, sdl::KeyCode::kKpMinus},
        std::pair{SDLK_KP_PLUS, sdl::KeyCode::kKpPlus},
        std::pair{SDLK_KP_ENTER, sdl::KeyCode::kKpEnter},
        std::pair{SDLK_KP_1, sdl::KeyCode::kKp1},
        std::pair{SDLK_KP_2, sdl::KeyCode::kKp2},
        std::pair{SDLK_KP_3, sdl::KeyCode::kKp3},
        std::pair{SDLK_KP_4, sdl::KeyCode::kKp4},
        std::pair{SDLK_KP_5, sdl::KeyCode::kKp5},
        std::pair{SDLK_KP_6, sdl::KeyCode::kKp6},
        std::pair{SDLK_KP_7, sdl::KeyCode::kKp7},
        std::pair{SDLK_KP_8, sdl::KeyCode::kKp8},
        std::pair{SDLK_KP_9, sdl::KeyCode::kKp9},
        std::pair{SDLK_KP_0, sdl::KeyCode::kKp0},
        std::pair{SDLK_KP_PERIOD, sdl::KeyCode::kKpPeriod},
        std::pair{SDLK_KP_EQUALS, sdl::KeyCode::kKpEquals},
        // Application keys
        std::pair{SDLK_APPLICATION, sdl::KeyCode::kApplication},
        std::pair{SDLK_POWER, sdl::KeyCode::kPower},
        std::pair{SDLK_EXECUTE, sdl::KeyCode::kExecute},
        std::pair{SDLK_HELP, sdl::KeyCode::kHelp},
        std::pair{SDLK_MENU, sdl::KeyCode::kMenu},
        std::pair{SDLK_SELECT, sdl::KeyCode::kSelect},
        std::pair{SDLK_STOP, sdl::KeyCode::kStop},
        std::pair{SDLK_AGAIN, sdl::KeyCode::kAgain},
        std::pair{SDLK_UNDO, sdl::KeyCode::kUndo},
        std::pair{SDLK_CUT, sdl::KeyCode::kCut},
        std::pair{SDLK_COPY, sdl::KeyCode::kCopy},
        std::pair{SDLK_PASTE, sdl::KeyCode::kPaste},
        std::pair{SDLK_FIND, sdl::KeyCode::kFind},
        std::pair{SDLK_MUTE, sdl::KeyCode::kMute},
        std::pair{SDLK_VOLUMEUP, sdl::KeyCode::kVolumeUp},
        std::pair{SDLK_VOLUMEDOWN, sdl::KeyCode::kVolumeDown},
        // Modifier keys
        std::pair{SDLK_LCTRL, sdl::KeyCode::kLCtrl},
        std::pair{SDLK_LSHIFT, sdl::KeyCode::kLShift},
        std::pair{SDLK_LALT, sdl::KeyCode::kLAlt},
        std::pair{SDLK_LGUI, sdl::KeyCode::kLGui},
        std::pair{SDLK_RCTRL, sdl::KeyCode::kRCtrl},
        std::pair{SDLK_RSHIFT, sdl::KeyCode::kRShift},
        std::pair{SDLK_RALT, sdl::KeyCode::kRAlt},
        std::pair{SDLK_RGUI, sdl::KeyCode::kRGui}
};
static constexpr auto kSdlKeycodeMap = Map(kSdlKeycodeMapArray);

static constexpr auto kSdlKeyModifierMapArray = std::array{
        std::pair{SDL_KMOD_NONE, sdl::KeyModifier::kNone},
        std::pair{SDL_KMOD_LSHIFT, sdl::KeyModifier::kLShift},
        std::pair{SDL_KMOD_RSHIFT, sdl::KeyModifier::kRShift},
        std::pair{SDL_KMOD_LEVEL5, sdl::KeyModifier::kLevel5},
        std::pair{SDL_KMOD_LCTRL, sdl::KeyModifier::kLCtrl},
        std::pair{SDL_KMOD_RCTRL, sdl::KeyModifier::kRCtrl},
        std::pair{SDL_KMOD_LALT, sdl::KeyModifier::kLAlt},
        std::pair{SDL_KMOD_RALT, sdl::KeyModifier::kRAlt},
        std::pair{SDL_KMOD_LGUI, sdl::KeyModifier::kLGui},
        std::pair{SDL_KMOD_RGUI, sdl::KeyModifier::kRGui},
        std::pair{SDL_KMOD_NUM, sdl::KeyModifier::kNum},
        std::pair{SDL_KMOD_CAPS, sdl::KeyModifier::kCaps},
        std::pair{SDL_KMOD_MODE, sdl::KeyModifier::kMode},
        std::pair{SDL_KMOD_SCROLL, sdl::KeyModifier::kScroll},
        std::pair{SDL_KMOD_CTRL, sdl::KeyModifier::kCtrl},
        std::pair{SDL_KMOD_SHIFT, sdl::KeyModifier::kShift},
        std::pair{SDL_KMOD_ALT, sdl::KeyModifier::kAlt},
        std::pair{SDL_KMOD_GUI, sdl::KeyModifier::kGui}
};
static constexpr auto kSdlKeyModifierMap = Map(kSdlKeyModifierMapArray);

// Reverse maps for converting back to SDL types
static constexpr auto kKeycodeToSdlMapArray = std::array{
    std::pair{sdl::KeyCode::kUnknown, SDLK_UNKNOWN},
    // Basic ASCII characters
    std::pair{sdl::KeyCode::kReturn, SDLK_RETURN},
    std::pair{sdl::KeyCode::kEscape, SDLK_ESCAPE},
    std::pair{sdl::KeyCode::kBackspace, SDLK_BACKSPACE},
    std::pair{sdl::KeyCode::kTab, SDLK_TAB},
    std::pair{sdl::KeyCode::kSpace, SDLK_SPACE},
    std::pair{sdl::KeyCode::kExclaim, SDLK_EXCLAIM},
    std::pair{sdl::KeyCode::kDblApostrophe, SDLK_DBLAPOSTROPHE},
    std::pair{sdl::KeyCode::kHash, SDLK_HASH},
    std::pair{sdl::KeyCode::kDollar, SDLK_DOLLAR},
    std::pair{sdl::KeyCode::kPercent, SDLK_PERCENT},
    std::pair{sdl::KeyCode::kAmpersand, SDLK_AMPERSAND},
    std::pair{sdl::KeyCode::kApostrophe, SDLK_APOSTROPHE},
    std::pair{sdl::KeyCode::kLeftParen, SDLK_LEFTPAREN},
    std::pair{sdl::KeyCode::kRightParen, SDLK_RIGHTPAREN},
    std::pair{sdl::KeyCode::kAsterisk, SDLK_ASTERISK},
    std::pair{sdl::KeyCode::kPlus, SDLK_PLUS},
    std::pair{sdl::KeyCode::kComma, SDLK_COMMA},
    std::pair{sdl::KeyCode::kMinus, SDLK_MINUS},
    std::pair{sdl::KeyCode::kPeriod, SDLK_PERIOD},
    std::pair{sdl::KeyCode::kSlash, SDLK_SLASH},
    // Numbers
    std::pair{sdl::KeyCode::k0, SDLK_0},
    std::pair{sdl::KeyCode::k1, SDLK_1},
    std::pair{sdl::KeyCode::k2, SDLK_2},
    std::pair{sdl::KeyCode::k3, SDLK_3},
    std::pair{sdl::KeyCode::k4, SDLK_4},
    std::pair{sdl::KeyCode::k5, SDLK_5},
    std::pair{sdl::KeyCode::k6, SDLK_6},
    std::pair{sdl::KeyCode::k7, SDLK_7},
    std::pair{sdl::KeyCode::k8, SDLK_8},
    std::pair{sdl::KeyCode::k9, SDLK_9},
    // More symbols
    std::pair{sdl::KeyCode::kColon, SDLK_COLON},
    std::pair{sdl::KeyCode::kSemicolon, SDLK_SEMICOLON},
    std::pair{sdl::KeyCode::kLess, SDLK_LESS},
    std::pair{sdl::KeyCode::kEquals, SDLK_EQUALS},
    std::pair{sdl::KeyCode::kGreater, SDLK_GREATER},
    std::pair{sdl::KeyCode::kQuestion, SDLK_QUESTION},
    std::pair{sdl::KeyCode::kAt, SDLK_AT},
    // Letters (lowercase)
    std::pair{sdl::KeyCode::kA, SDLK_A},
    std::pair{sdl::KeyCode::kB, SDLK_B},
    std::pair{sdl::KeyCode::kC, SDLK_C},
    std::pair{sdl::KeyCode::kD, SDLK_D},
    std::pair{sdl::KeyCode::kE, SDLK_E},
    std::pair{sdl::KeyCode::kF, SDLK_F},
    std::pair{sdl::KeyCode::kG, SDLK_G},
    std::pair{sdl::KeyCode::kH, SDLK_H},
    std::pair{sdl::KeyCode::kI, SDLK_I},
    std::pair{sdl::KeyCode::kJ, SDLK_J},
    std::pair{sdl::KeyCode::kK, SDLK_K},
    std::pair{sdl::KeyCode::kL, SDLK_L},
    std::pair{sdl::KeyCode::kM, SDLK_M},
    std::pair{sdl::KeyCode::kN, SDLK_N},
    std::pair{sdl::KeyCode::kO, SDLK_O},
    std::pair{sdl::KeyCode::kP, SDLK_P},
    std::pair{sdl::KeyCode::kQ, SDLK_Q},
    std::pair{sdl::KeyCode::kR, SDLK_R},
    std::pair{sdl::KeyCode::kS, SDLK_S},
    std::pair{sdl::KeyCode::kT, SDLK_T},
    std::pair{sdl::KeyCode::kU, SDLK_U},
    std::pair{sdl::KeyCode::kV, SDLK_V},
    std::pair{sdl::KeyCode::kW, SDLK_W},
    std::pair{sdl::KeyCode::kX, SDLK_X},
    std::pair{sdl::KeyCode::kY, SDLK_Y},
    std::pair{sdl::KeyCode::kZ, SDLK_Z},
    // Brackets and symbols
    std::pair{sdl::KeyCode::kLeftBracket, SDLK_LEFTBRACKET},
    std::pair{sdl::KeyCode::kBackslash, SDLK_BACKSLASH},
    std::pair{sdl::KeyCode::kRightBracket, SDLK_RIGHTBRACKET},
    std::pair{sdl::KeyCode::kCaret, SDLK_CARET},
    std::pair{sdl::KeyCode::kUnderscore, SDLK_UNDERSCORE},
    std::pair{sdl::KeyCode::kGrave, SDLK_GRAVE},
    std::pair{sdl::KeyCode::kLeftBrace, SDLK_LEFTBRACE},
    std::pair{sdl::KeyCode::kPipe, SDLK_PIPE},
    std::pair{sdl::KeyCode::kRightBrace, SDLK_RIGHTBRACE},
    std::pair{sdl::KeyCode::kTilde, SDLK_TILDE},
    std::pair{sdl::KeyCode::kDelete, SDLK_DELETE},
    // Special characters
    std::pair{sdl::KeyCode::kPlusMinus, SDLK_PLUSMINUS},
    // Function keys
    std::pair{sdl::KeyCode::kCapsLock, SDLK_CAPSLOCK},
    std::pair{sdl::KeyCode::kF1, SDLK_F1},
    std::pair{sdl::KeyCode::kF2, SDLK_F2},
    std::pair{sdl::KeyCode::kF3, SDLK_F3},
    std::pair{sdl::KeyCode::kF4, SDLK_F4},
    std::pair{sdl::KeyCode::kF5, SDLK_F5},
    std::pair{sdl::KeyCode::kF6, SDLK_F6},
    std::pair{sdl::KeyCode::kF7, SDLK_F7},
    std::pair{sdl::KeyCode::kF8, SDLK_F8},
    std::pair{sdl::KeyCode::kF9, SDLK_F9},
    std::pair{sdl::KeyCode::kF10, SDLK_F10},
    std::pair{sdl::KeyCode::kF11, SDLK_F11},
    std::pair{sdl::KeyCode::kF12, SDLK_F12},
    std::pair{sdl::KeyCode::kF13, SDLK_F13},
    std::pair{sdl::KeyCode::kF14, SDLK_F14},
    std::pair{sdl::KeyCode::kF15, SDLK_F15},
    std::pair{sdl::KeyCode::kF16, SDLK_F16},
    std::pair{sdl::KeyCode::kF17, SDLK_F17},
    std::pair{sdl::KeyCode::kF18, SDLK_F18},
    std::pair{sdl::KeyCode::kF19, SDLK_F19},
    std::pair{sdl::KeyCode::kF20, SDLK_F20},
    std::pair{sdl::KeyCode::kF21, SDLK_F21},
    std::pair{sdl::KeyCode::kF22, SDLK_F22},
    std::pair{sdl::KeyCode::kF23, SDLK_F23},
    std::pair{sdl::KeyCode::kF24, SDLK_F24},
    // Navigation and editing keys
    std::pair{sdl::KeyCode::kPrintScreen, SDLK_PRINTSCREEN},
    std::pair{sdl::KeyCode::kScrollLock, SDLK_SCROLLLOCK},
    std::pair{sdl::KeyCode::kPause, SDLK_PAUSE},
    std::pair{sdl::KeyCode::kInsert, SDLK_INSERT},
    std::pair{sdl::KeyCode::kHome, SDLK_HOME},
    std::pair{sdl::KeyCode::kPageUp, SDLK_PAGEUP},
    std::pair{sdl::KeyCode::kEnd, SDLK_END},
    std::pair{sdl::KeyCode::kPageDown, SDLK_PAGEDOWN},
    std::pair{sdl::KeyCode::kRight, SDLK_RIGHT},
    std::pair{sdl::KeyCode::kLeft, SDLK_LEFT},
    std::pair{sdl::KeyCode::kDown, SDLK_DOWN},
    std::pair{sdl::KeyCode::kUp, SDLK_UP},
    // Keypad keys
    std::pair{sdl::KeyCode::kNumLockClear, SDLK_NUMLOCKCLEAR},
    std::pair{sdl::KeyCode::kKpDivide, SDLK_KP_DIVIDE},
    std::pair{sdl::KeyCode::kKpMultiply, SDLK_KP_MULTIPLY},
    std::pair{sdl::KeyCode::kKpMinus, SDLK_KP_MINUS},
    std::pair{sdl::KeyCode::kKpPlus, SDLK_KP_PLUS},
    std::pair{sdl::KeyCode::kKpEnter, SDLK_KP_ENTER},
    std::pair{sdl::KeyCode::kKp1, SDLK_KP_1},
    std::pair{sdl::KeyCode::kKp2, SDLK_KP_2},
    std::pair{sdl::KeyCode::kKp3, SDLK_KP_3},
    std::pair{sdl::KeyCode::kKp4, SDLK_KP_4},
    std::pair{sdl::KeyCode::kKp5, SDLK_KP_5},
    std::pair{sdl::KeyCode::kKp6, SDLK_KP_6},
    std::pair{sdl::KeyCode::kKp7, SDLK_KP_7},
    std::pair{sdl::KeyCode::kKp8, SDLK_KP_8},
    std::pair{sdl::KeyCode::kKp9, SDLK_KP_9},
    std::pair{sdl::KeyCode::kKp0, SDLK_KP_0},
    std::pair{sdl::KeyCode::kKpPeriod, SDLK_KP_PERIOD},
    std::pair{sdl::KeyCode::kKpEquals, SDLK_KP_EQUALS},
    // Application keys
    std::pair{sdl::KeyCode::kApplication, SDLK_APPLICATION},
    std::pair{sdl::KeyCode::kPower, SDLK_POWER},
    std::pair{sdl::KeyCode::kExecute, SDLK_EXECUTE},
    std::pair{sdl::KeyCode::kHelp, SDLK_HELP},
    std::pair{sdl::KeyCode::kMenu, SDLK_MENU},
    std::pair{sdl::KeyCode::kSelect, SDLK_SELECT},
    std::pair{sdl::KeyCode::kStop, SDLK_STOP},
    std::pair{sdl::KeyCode::kAgain, SDLK_AGAIN},
    std::pair{sdl::KeyCode::kUndo, SDLK_UNDO},
    std::pair{sdl::KeyCode::kCut, SDLK_CUT},
    std::pair{sdl::KeyCode::kCopy, SDLK_COPY},
    std::pair{sdl::KeyCode::kPaste, SDLK_PASTE},
    std::pair{sdl::KeyCode::kFind, SDLK_FIND},
    std::pair{sdl::KeyCode::kMute, SDLK_MUTE},
    std::pair{sdl::KeyCode::kVolumeUp, SDLK_VOLUMEUP},
    std::pair{sdl::KeyCode::kVolumeDown, SDLK_VOLUMEDOWN},
    // Modifier keys
    std::pair{sdl::KeyCode::kLCtrl, SDLK_LCTRL},
    std::pair{sdl::KeyCode::kLShift, SDLK_LSHIFT},
    std::pair{sdl::KeyCode::kLAlt, SDLK_LALT},
    std::pair{sdl::KeyCode::kLGui, SDLK_LGUI},
    std::pair{sdl::KeyCode::kRCtrl, SDLK_RCTRL},
    std::pair{sdl::KeyCode::kRShift, SDLK_RSHIFT},
    std::pair{sdl::KeyCode::kRAlt, SDLK_RALT},
    std::pair{sdl::KeyCode::kRGui, SDLK_RGUI}
};
static constexpr auto kKeycodeToSdlMap = Map(kKeycodeToSdlMapArray);

static constexpr auto kScancodeToSdlMapArray = std::array{
    std::pair{sdl::ScanCode::kUnknown, SDL_SCANCODE_UNKNOWN},
    // Letters
    std::pair{sdl::ScanCode::kA, SDL_SCANCODE_A},
    std::pair{sdl::ScanCode::kB, SDL_SCANCODE_B},
    std::pair{sdl::ScanCode::kC, SDL_SCANCODE_C},
    std::pair{sdl::ScanCode::kD, SDL_SCANCODE_D},
    std::pair{sdl::ScanCode::kE, SDL_SCANCODE_E},
    std::pair{sdl::ScanCode::kF, SDL_SCANCODE_F},
    std::pair{sdl::ScanCode::kG, SDL_SCANCODE_G},
    std::pair{sdl::ScanCode::kH, SDL_SCANCODE_H},
    std::pair{sdl::ScanCode::kI, SDL_SCANCODE_I},
    std::pair{sdl::ScanCode::kJ, SDL_SCANCODE_J},
    std::pair{sdl::ScanCode::kK, SDL_SCANCODE_K},
    std::pair{sdl::ScanCode::kL, SDL_SCANCODE_L},
    std::pair{sdl::ScanCode::kM, SDL_SCANCODE_M},
    std::pair{sdl::ScanCode::kN, SDL_SCANCODE_N},
    std::pair{sdl::ScanCode::kO, SDL_SCANCODE_O},
    std::pair{sdl::ScanCode::kP, SDL_SCANCODE_P},
    std::pair{sdl::ScanCode::kQ, SDL_SCANCODE_Q},
    std::pair{sdl::ScanCode::kR, SDL_SCANCODE_R},
    std::pair{sdl::ScanCode::kS, SDL_SCANCODE_S},
    std::pair{sdl::ScanCode::kT, SDL_SCANCODE_T},
    std::pair{sdl::ScanCode::kU, SDL_SCANCODE_U},
    std::pair{sdl::ScanCode::kV, SDL_SCANCODE_V},
    std::pair{sdl::ScanCode::kW, SDL_SCANCODE_W},
    std::pair{sdl::ScanCode::kX, SDL_SCANCODE_X},
    std::pair{sdl::ScanCode::kY, SDL_SCANCODE_Y},
    std::pair{sdl::ScanCode::kZ, SDL_SCANCODE_Z},
    // Numbers
    std::pair{sdl::ScanCode::k1, SDL_SCANCODE_1},
    std::pair{sdl::ScanCode::k2, SDL_SCANCODE_2},
    std::pair{sdl::ScanCode::k3, SDL_SCANCODE_3},
    std::pair{sdl::ScanCode::k4, SDL_SCANCODE_4},
    std::pair{sdl::ScanCode::k5, SDL_SCANCODE_5},
    std::pair{sdl::ScanCode::k6, SDL_SCANCODE_6},
    std::pair{sdl::ScanCode::k7, SDL_SCANCODE_7},
    std::pair{sdl::ScanCode::k8, SDL_SCANCODE_8},
    std::pair{sdl::ScanCode::k9, SDL_SCANCODE_9},
    std::pair{sdl::ScanCode::k0, SDL_SCANCODE_0},
    // Common keys
    std::pair{sdl::ScanCode::kReturn, SDL_SCANCODE_RETURN},
    std::pair{sdl::ScanCode::kEscape, SDL_SCANCODE_ESCAPE},
    std::pair{sdl::ScanCode::kBackspace, SDL_SCANCODE_BACKSPACE},
    std::pair{sdl::ScanCode::kTab, SDL_SCANCODE_TAB},
    std::pair{sdl::ScanCode::kSpace, SDL_SCANCODE_SPACE},
    std::pair{sdl::ScanCode::kMinus, SDL_SCANCODE_MINUS},
    std::pair{sdl::ScanCode::kEquals, SDL_SCANCODE_EQUALS},
    std::pair{sdl::ScanCode::kLeftBracket, SDL_SCANCODE_LEFTBRACKET},
    std::pair{sdl::ScanCode::kRightBracket, SDL_SCANCODE_RIGHTBRACKET},
    std::pair{sdl::ScanCode::kBackslash, SDL_SCANCODE_BACKSLASH},
    std::pair{sdl::ScanCode::kNonUsHash, SDL_SCANCODE_NONUSHASH},
    std::pair{sdl::ScanCode::kSemicolon, SDL_SCANCODE_SEMICOLON},
    std::pair{sdl::ScanCode::kApostrophe, SDL_SCANCODE_APOSTROPHE},
    std::pair{sdl::ScanCode::kGrave, SDL_SCANCODE_GRAVE},
    std::pair{sdl::ScanCode::kComma, SDL_SCANCODE_COMMA},
    std::pair{sdl::ScanCode::kPeriod, SDL_SCANCODE_PERIOD},
    std::pair{sdl::ScanCode::kSlash, SDL_SCANCODE_SLASH},
    std::pair{sdl::ScanCode::kCapsLock, SDL_SCANCODE_CAPSLOCK},
    // Function keys
    std::pair{sdl::ScanCode::kF1, SDL_SCANCODE_F1},
    std::pair{sdl::ScanCode::kF2, SDL_SCANCODE_F2},
    std::pair{sdl::ScanCode::kF3, SDL_SCANCODE_F3},
    std::pair{sdl::ScanCode::kF4, SDL_SCANCODE_F4},
    std::pair{sdl::ScanCode::kF5, SDL_SCANCODE_F5},
    std::pair{sdl::ScanCode::kF6, SDL_SCANCODE_F6},
    std::pair{sdl::ScanCode::kF7, SDL_SCANCODE_F7},
    std::pair{sdl::ScanCode::kF8, SDL_SCANCODE_F8},
    std::pair{sdl::ScanCode::kF9, SDL_SCANCODE_F9},
    std::pair{sdl::ScanCode::kF10, SDL_SCANCODE_F10},
    std::pair{sdl::ScanCode::kF11, SDL_SCANCODE_F11},
    std::pair{sdl::ScanCode::kF12, SDL_SCANCODE_F12},
    std::pair{sdl::ScanCode::kF13, SDL_SCANCODE_F13},
    std::pair{sdl::ScanCode::kF14, SDL_SCANCODE_F14},
    std::pair{sdl::ScanCode::kF15, SDL_SCANCODE_F15},
    std::pair{sdl::ScanCode::kF16, SDL_SCANCODE_F16},
    std::pair{sdl::ScanCode::kF17, SDL_SCANCODE_F17},
    std::pair{sdl::ScanCode::kF18, SDL_SCANCODE_F18},
    std::pair{sdl::ScanCode::kF19, SDL_SCANCODE_F19},
    std::pair{sdl::ScanCode::kF20, SDL_SCANCODE_F20},
    std::pair{sdl::ScanCode::kF21, SDL_SCANCODE_F21},
    std::pair{sdl::ScanCode::kF22, SDL_SCANCODE_F22},
    std::pair{sdl::ScanCode::kF23, SDL_SCANCODE_F23},
    std::pair{sdl::ScanCode::kF24, SDL_SCANCODE_F24},
    // Navigation and editing keys
    std::pair{sdl::ScanCode::kPrintScreen, SDL_SCANCODE_PRINTSCREEN},
    std::pair{sdl::ScanCode::kScrollLock, SDL_SCANCODE_SCROLLLOCK},
    std::pair{sdl::ScanCode::kPause, SDL_SCANCODE_PAUSE},
    std::pair{sdl::ScanCode::kInsert, SDL_SCANCODE_INSERT},
    std::pair{sdl::ScanCode::kHome, SDL_SCANCODE_HOME},
    std::pair{sdl::ScanCode::kPageUp, SDL_SCANCODE_PAGEUP},
    std::pair{sdl::ScanCode::kDelete, SDL_SCANCODE_DELETE},
    std::pair{sdl::ScanCode::kEnd, SDL_SCANCODE_END},
    std::pair{sdl::ScanCode::kPageDown, SDL_SCANCODE_PAGEDOWN},
    std::pair{sdl::ScanCode::kRight, SDL_SCANCODE_RIGHT},
    std::pair{sdl::ScanCode::kLeft, SDL_SCANCODE_LEFT},
    std::pair{sdl::ScanCode::kDown, SDL_SCANCODE_DOWN},
    std::pair{sdl::ScanCode::kUp, SDL_SCANCODE_UP},
    // Keypad keys
    std::pair{sdl::ScanCode::kNumLockClear, SDL_SCANCODE_NUMLOCKCLEAR},
    std::pair{sdl::ScanCode::kKpDivide, SDL_SCANCODE_KP_DIVIDE},
    std::pair{sdl::ScanCode::kKpMultiply, SDL_SCANCODE_KP_MULTIPLY},
    std::pair{sdl::ScanCode::kKpMinus, SDL_SCANCODE_KP_MINUS},
    std::pair{sdl::ScanCode::kKpPlus, SDL_SCANCODE_KP_PLUS},
    std::pair{sdl::ScanCode::kKpEnter, SDL_SCANCODE_KP_ENTER},
    std::pair{sdl::ScanCode::kKp1, SDL_SCANCODE_KP_1},
    std::pair{sdl::ScanCode::kKp2, SDL_SCANCODE_KP_2},
    std::pair{sdl::ScanCode::kKp3, SDL_SCANCODE_KP_3},
    std::pair{sdl::ScanCode::kKp4, SDL_SCANCODE_KP_4},
    std::pair{sdl::ScanCode::kKp5, SDL_SCANCODE_KP_5},
    std::pair{sdl::ScanCode::kKp6, SDL_SCANCODE_KP_6},
    std::pair{sdl::ScanCode::kKp7, SDL_SCANCODE_KP_7},
    std::pair{sdl::ScanCode::kKp8, SDL_SCANCODE_KP_8},
    std::pair{sdl::ScanCode::kKp9, SDL_SCANCODE_KP_9},
    std::pair{sdl::ScanCode::kKp0, SDL_SCANCODE_KP_0},
    std::pair{sdl::ScanCode::kKpPeriod, SDL_SCANCODE_KP_PERIOD},
    std::pair{sdl::ScanCode::kKpEquals, SDL_SCANCODE_KP_EQUALS},
    // Special keys
    std::pair{sdl::ScanCode::kNonUsBackslash, SDL_SCANCODE_NONUSBACKSLASH},
    std::pair{sdl::ScanCode::kApplication, SDL_SCANCODE_APPLICATION},
    std::pair{sdl::ScanCode::kPower, SDL_SCANCODE_POWER},
    std::pair{sdl::ScanCode::kExecute, SDL_SCANCODE_EXECUTE},
    std::pair{sdl::ScanCode::kHelp, SDL_SCANCODE_HELP},
    std::pair{sdl::ScanCode::kMenu, SDL_SCANCODE_MENU},
    std::pair{sdl::ScanCode::kSelect, SDL_SCANCODE_SELECT},
    std::pair{sdl::ScanCode::kStop, SDL_SCANCODE_STOP},
    std::pair{sdl::ScanCode::kAgain, SDL_SCANCODE_AGAIN},
    std::pair{sdl::ScanCode::kUndo, SDL_SCANCODE_UNDO},
    std::pair{sdl::ScanCode::kCut, SDL_SCANCODE_CUT},
    std::pair{sdl::ScanCode::kCopy, SDL_SCANCODE_COPY},
    std::pair{sdl::ScanCode::kPaste, SDL_SCANCODE_PASTE},
    std::pair{sdl::ScanCode::kFind, SDL_SCANCODE_FIND},
    std::pair{sdl::ScanCode::kMute, SDL_SCANCODE_MUTE},
    std::pair{sdl::ScanCode::kVolumeUp, SDL_SCANCODE_VOLUMEUP},
    std::pair{sdl::ScanCode::kVolumeDown, SDL_SCANCODE_VOLUMEDOWN},
    std::pair{sdl::ScanCode::kClearAgain, SDL_SCANCODE_CLEARAGAIN},
    // Modifier keys
    std::pair{sdl::ScanCode::kLCtrl, SDL_SCANCODE_LCTRL},
    std::pair{sdl::ScanCode::kLShift, SDL_SCANCODE_LSHIFT},
    std::pair{sdl::ScanCode::kLAlt, SDL_SCANCODE_LALT},
    std::pair{sdl::ScanCode::kLGui, SDL_SCANCODE_LGUI},
    std::pair{sdl::ScanCode::kRCtrl, SDL_SCANCODE_RCTRL},
    std::pair{sdl::ScanCode::kRShift, SDL_SCANCODE_RSHIFT},
    std::pair{sdl::ScanCode::kRAlt, SDL_SCANCODE_RALT},
    std::pair{sdl::ScanCode::kRGui, SDL_SCANCODE_RGUI},
    std::pair{sdl::ScanCode::kMode, SDL_SCANCODE_MODE},
    // Media keys
    std::pair{sdl::ScanCode::kMediaPlay, SDL_SCANCODE_MEDIA_PLAY},
    std::pair{sdl::ScanCode::kMediaPause, SDL_SCANCODE_MEDIA_PAUSE},
    std::pair{sdl::ScanCode::kMediaStop, SDL_SCANCODE_MEDIA_STOP},
    std::pair{sdl::ScanCode::kMediaNextTrack, SDL_SCANCODE_MEDIA_NEXT_TRACK},
    std::pair{sdl::ScanCode::kMediaPreviousTrack, SDL_SCANCODE_MEDIA_PREVIOUS_TRACK}
};
static constexpr auto kScancodeToSdlMap = Map(kScancodeToSdlMapArray);

static constexpr auto kKeyModifierToSdlMapArray = std::array{
    std::pair{sdl::KeyModifier::kNone, SDL_KMOD_NONE},
    std::pair{sdl::KeyModifier::kLShift, SDL_KMOD_LSHIFT},
    std::pair{sdl::KeyModifier::kRShift, SDL_KMOD_RSHIFT},
    std::pair{sdl::KeyModifier::kLevel5, SDL_KMOD_LEVEL5},
    std::pair{sdl::KeyModifier::kLCtrl, SDL_KMOD_LCTRL},
    std::pair{sdl::KeyModifier::kRCtrl, SDL_KMOD_RCTRL},
    std::pair{sdl::KeyModifier::kLAlt, SDL_KMOD_LALT},
    std::pair{sdl::KeyModifier::kRAlt, SDL_KMOD_RALT},
    std::pair{sdl::KeyModifier::kLGui, SDL_KMOD_LGUI},
    std::pair{sdl::KeyModifier::kRGui, SDL_KMOD_RGUI},
    std::pair{sdl::KeyModifier::kNum, SDL_KMOD_NUM},
    std::pair{sdl::KeyModifier::kCaps, SDL_KMOD_CAPS},
    std::pair{sdl::KeyModifier::kMode, SDL_KMOD_MODE},
    std::pair{sdl::KeyModifier::kScroll, SDL_KMOD_SCROLL},
    std::pair{sdl::KeyModifier::kCtrl, SDL_KMOD_CTRL},
    std::pair{sdl::KeyModifier::kShift, SDL_KMOD_SHIFT},
    std::pair{sdl::KeyModifier::kAlt, SDL_KMOD_ALT},
    std::pair{sdl::KeyModifier::kGui, SDL_KMOD_GUI}
};
static constexpr auto kKeyModifierToSdlMap = Map(kKeyModifierToSdlMapArray);

auto createQuitEvent([[maybe_unused]] const SDL_QuitEvent* sdlQuitEvent)
    -> std::unique_ptr<QuitEvent> {
  return std::make_unique<QuitEvent>(std::chrono::milliseconds(SDL_GetTicks()));
}

auto createMouseButtonEvent(const SDL_MouseButtonEvent* sdlMouseButtonEvent)
    -> std::unique_ptr<MouseButtonEvent>;

auto createKeyboardEvent(const SDL_KeyboardEvent* sdlKeyboardEvent)
    -> std::unique_ptr<KeyboardEvent>;
    
auto createUserEvent(const SDL_UserEvent* sdlUserEvent)
    -> std::unique_ptr<BaseEvent>;

/**
 * @brief Template specialization of EventAdaptor for SDL_Event
 *
 * This specialization provides the conversion logic from SDL_Event
 * to sdl event objects, reusing the existing conversion functions.
 */
 template<>
 class EventAdaptor<SDL_Event> {
 public:
  /**
   * @brief Convert SDL_Event to sdl event
   * @param event The SDL event to convert
   * @return Converted sdl event, or nullptr if conversion not supported
   */
  auto convertEvent(const SDL_Event& event) -> std::unique_ptr<BaseEvent> {
    switch (event.type) {
      case SDL_EventType::SDL_EVENT_KEY_DOWN:
      case SDL_EventType::SDL_EVENT_KEY_UP:
        return createKeyboardEvent(&event.key);
      case SDL_EventType::SDL_EVENT_MOUSE_BUTTON_DOWN:
      case SDL_EventType::SDL_EVENT_MOUSE_BUTTON_UP:
        return createMouseButtonEvent(&event.button);
      case SDL_EventType::SDL_EVENT_QUIT:
        return createQuitEvent(&event.quit);
      default:
        // Check if this is a user event
        if (event.type >= SDL_EVENT_USER && event.type < SDL_EVENT_LAST) {
          return createUserEvent(&event.user);
        }
        return nullptr;  // Unsupported event type
    }
  }
};


/**
 * @brief Concrete SDL event bus implementation using both CRTP and virtual inheritance
 * 
 * This class provides the concrete implementation for SDL event handling
 * using the CRTP-based TemplatedEventBus for zero-cost abstraction while
 * also inheriting from BaseEventBus to allow polymorphic usage.
 */
class SDLEventBus : public TemplatedEventBus<SDLEventBus, SDL_Event>, public BaseEventBus {
 public:
  SDLEventBus() = default;

  // Implementation methods for CRTP base
  auto waitImpl() -> std::unique_ptr<BaseEvent>;
  auto pollImpl() -> std::optional<std::unique_ptr<BaseEvent>>;
  void publishImpl(std::unique_ptr<UserEvent> event);

  // BaseEventBus interface implementation (delegates to CRTP methods)
  auto wait() -> std::unique_ptr<BaseEvent> override {
    return TemplatedEventBus<SDLEventBus, SDL_Event>::wait();
  }

  auto poll() -> std::optional<std::unique_ptr<BaseEvent>> override {
    return TemplatedEventBus<SDLEventBus, SDL_Event>::poll();
  }

  void publish(std::unique_ptr<UserEvent> event) override {
    TemplatedEventBus<SDLEventBus, SDL_Event>::publish(std::move(event));
  }

  void setRouteCallback(std::function<void(std::unique_ptr<BaseEvent>)> callback) override {
    TemplatedEventBus<SDLEventBus, SDL_Event>::setRouteCallback(std::move(callback));
  }

  void injectEvent(const std::any& eventData, std::type_index eventTypeId) override;
};

/**
 * @brief Factory function to create an SDL event bus
 * 
 * This function creates an SDLEventBus instance that uses CRTP internally
 * for zero-cost abstraction while providing the BaseEventBus interface.
 * 
 * @return A shared pointer to an SDL event bus as BaseEventBus
 */
auto createSDLEventBus() -> std::shared_ptr<BaseEventBus>;

}  // namespace sdl
