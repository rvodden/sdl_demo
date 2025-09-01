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
        std::pair{SDL_SCANCODE_UNKNOWN, KeyboardEvent::ScanCode::kUnknown},
        // Letters
        std::pair{SDL_SCANCODE_A, KeyboardEvent::ScanCode::kA},
        std::pair{SDL_SCANCODE_B, KeyboardEvent::ScanCode::kB},
        std::pair{SDL_SCANCODE_C, KeyboardEvent::ScanCode::kC},
        std::pair{SDL_SCANCODE_D, KeyboardEvent::ScanCode::kD},
        std::pair{SDL_SCANCODE_E, KeyboardEvent::ScanCode::kE},
        std::pair{SDL_SCANCODE_F, KeyboardEvent::ScanCode::kF},
        std::pair{SDL_SCANCODE_G, KeyboardEvent::ScanCode::kG},
        std::pair{SDL_SCANCODE_H, KeyboardEvent::ScanCode::kH},
        std::pair{SDL_SCANCODE_I, KeyboardEvent::ScanCode::kI},
        std::pair{SDL_SCANCODE_J, KeyboardEvent::ScanCode::kJ},
        std::pair{SDL_SCANCODE_K, KeyboardEvent::ScanCode::kK},
        std::pair{SDL_SCANCODE_L, KeyboardEvent::ScanCode::kL},
        std::pair{SDL_SCANCODE_M, KeyboardEvent::ScanCode::kM},
        std::pair{SDL_SCANCODE_N, KeyboardEvent::ScanCode::kN},
        std::pair{SDL_SCANCODE_O, KeyboardEvent::ScanCode::kO},
        std::pair{SDL_SCANCODE_P, KeyboardEvent::ScanCode::kP},
        std::pair{SDL_SCANCODE_Q, KeyboardEvent::ScanCode::kQ},
        std::pair{SDL_SCANCODE_R, KeyboardEvent::ScanCode::kR},
        std::pair{SDL_SCANCODE_S, KeyboardEvent::ScanCode::kS},
        std::pair{SDL_SCANCODE_T, KeyboardEvent::ScanCode::kT},
        std::pair{SDL_SCANCODE_U, KeyboardEvent::ScanCode::kU},
        std::pair{SDL_SCANCODE_V, KeyboardEvent::ScanCode::kV},
        std::pair{SDL_SCANCODE_W, KeyboardEvent::ScanCode::kW},
        std::pair{SDL_SCANCODE_X, KeyboardEvent::ScanCode::kX},
        std::pair{SDL_SCANCODE_Y, KeyboardEvent::ScanCode::kY},
        std::pair{SDL_SCANCODE_Z, KeyboardEvent::ScanCode::kZ},
        // Numbers
        std::pair{SDL_SCANCODE_1, KeyboardEvent::ScanCode::k1},
        std::pair{SDL_SCANCODE_2, KeyboardEvent::ScanCode::k2},
        std::pair{SDL_SCANCODE_3, KeyboardEvent::ScanCode::k3},
        std::pair{SDL_SCANCODE_4, KeyboardEvent::ScanCode::k4},
        std::pair{SDL_SCANCODE_5, KeyboardEvent::ScanCode::k5},
        std::pair{SDL_SCANCODE_6, KeyboardEvent::ScanCode::k6},
        std::pair{SDL_SCANCODE_7, KeyboardEvent::ScanCode::k7},
        std::pair{SDL_SCANCODE_8, KeyboardEvent::ScanCode::k8},
        std::pair{SDL_SCANCODE_9, KeyboardEvent::ScanCode::k9},
        std::pair{SDL_SCANCODE_0, KeyboardEvent::ScanCode::k0},
        // Common keys
        std::pair{SDL_SCANCODE_RETURN, KeyboardEvent::ScanCode::kReturn},
        std::pair{SDL_SCANCODE_ESCAPE, KeyboardEvent::ScanCode::kEscape},
        std::pair{SDL_SCANCODE_BACKSPACE, KeyboardEvent::ScanCode::kBackspace},
        std::pair{SDL_SCANCODE_TAB, KeyboardEvent::ScanCode::kTab},
        std::pair{SDL_SCANCODE_SPACE, KeyboardEvent::ScanCode::kSpace},
        std::pair{SDL_SCANCODE_MINUS, KeyboardEvent::ScanCode::kMinus},
        std::pair{SDL_SCANCODE_EQUALS, KeyboardEvent::ScanCode::kEquals},
        std::pair{SDL_SCANCODE_LEFTBRACKET, KeyboardEvent::ScanCode::kLeftBracket},
        std::pair{SDL_SCANCODE_RIGHTBRACKET, KeyboardEvent::ScanCode::kRightBracket},
        std::pair{SDL_SCANCODE_BACKSLASH, KeyboardEvent::ScanCode::kBackslash},
        std::pair{SDL_SCANCODE_NONUSHASH, KeyboardEvent::ScanCode::kNonUsHash},
        std::pair{SDL_SCANCODE_SEMICOLON, KeyboardEvent::ScanCode::kSemicolon},
        std::pair{SDL_SCANCODE_APOSTROPHE, KeyboardEvent::ScanCode::kApostrophe},
        std::pair{SDL_SCANCODE_GRAVE, KeyboardEvent::ScanCode::kGrave},
        std::pair{SDL_SCANCODE_COMMA, KeyboardEvent::ScanCode::kComma},
        std::pair{SDL_SCANCODE_PERIOD, KeyboardEvent::ScanCode::kPeriod},
        std::pair{SDL_SCANCODE_SLASH, KeyboardEvent::ScanCode::kSlash},
        std::pair{SDL_SCANCODE_CAPSLOCK, KeyboardEvent::ScanCode::kCapsLock},
        // Function keys
        std::pair{SDL_SCANCODE_F1, KeyboardEvent::ScanCode::kF1},
        std::pair{SDL_SCANCODE_F2, KeyboardEvent::ScanCode::kF2},
        std::pair{SDL_SCANCODE_F3, KeyboardEvent::ScanCode::kF3},
        std::pair{SDL_SCANCODE_F4, KeyboardEvent::ScanCode::kF4},
        std::pair{SDL_SCANCODE_F5, KeyboardEvent::ScanCode::kF5},
        std::pair{SDL_SCANCODE_F6, KeyboardEvent::ScanCode::kF6},
        std::pair{SDL_SCANCODE_F7, KeyboardEvent::ScanCode::kF7},
        std::pair{SDL_SCANCODE_F8, KeyboardEvent::ScanCode::kF8},
        std::pair{SDL_SCANCODE_F9, KeyboardEvent::ScanCode::kF9},
        std::pair{SDL_SCANCODE_F10, KeyboardEvent::ScanCode::kF10},
        std::pair{SDL_SCANCODE_F11, KeyboardEvent::ScanCode::kF11},
        std::pair{SDL_SCANCODE_F12, KeyboardEvent::ScanCode::kF12},
        std::pair{SDL_SCANCODE_F13, KeyboardEvent::ScanCode::kF13},
        std::pair{SDL_SCANCODE_F14, KeyboardEvent::ScanCode::kF14},
        std::pair{SDL_SCANCODE_F15, KeyboardEvent::ScanCode::kF15},
        std::pair{SDL_SCANCODE_F16, KeyboardEvent::ScanCode::kF16},
        std::pair{SDL_SCANCODE_F17, KeyboardEvent::ScanCode::kF17},
        std::pair{SDL_SCANCODE_F18, KeyboardEvent::ScanCode::kF18},
        std::pair{SDL_SCANCODE_F19, KeyboardEvent::ScanCode::kF19},
        std::pair{SDL_SCANCODE_F20, KeyboardEvent::ScanCode::kF20},
        std::pair{SDL_SCANCODE_F21, KeyboardEvent::ScanCode::kF21},
        std::pair{SDL_SCANCODE_F22, KeyboardEvent::ScanCode::kF22},
        std::pair{SDL_SCANCODE_F23, KeyboardEvent::ScanCode::kF23},
        std::pair{SDL_SCANCODE_F24, KeyboardEvent::ScanCode::kF24},
        // Navigation and editing keys
        std::pair{SDL_SCANCODE_PRINTSCREEN, KeyboardEvent::ScanCode::kPrintScreen},
        std::pair{SDL_SCANCODE_SCROLLLOCK, KeyboardEvent::ScanCode::kScrollLock},
        std::pair{SDL_SCANCODE_PAUSE, KeyboardEvent::ScanCode::kPause},
        std::pair{SDL_SCANCODE_INSERT, KeyboardEvent::ScanCode::kInsert},
        std::pair{SDL_SCANCODE_HOME, KeyboardEvent::ScanCode::kHome},
        std::pair{SDL_SCANCODE_PAGEUP, KeyboardEvent::ScanCode::kPageUp},
        std::pair{SDL_SCANCODE_DELETE, KeyboardEvent::ScanCode::kDelete},
        std::pair{SDL_SCANCODE_END, KeyboardEvent::ScanCode::kEnd},
        std::pair{SDL_SCANCODE_PAGEDOWN, KeyboardEvent::ScanCode::kPageDown},
        std::pair{SDL_SCANCODE_RIGHT, KeyboardEvent::ScanCode::kRight},
        std::pair{SDL_SCANCODE_LEFT, KeyboardEvent::ScanCode::kLeft},
        std::pair{SDL_SCANCODE_DOWN, KeyboardEvent::ScanCode::kDown},
        std::pair{SDL_SCANCODE_UP, KeyboardEvent::ScanCode::kUp},
        // Keypad keys
        std::pair{SDL_SCANCODE_NUMLOCKCLEAR, KeyboardEvent::ScanCode::kNumLockClear},
        std::pair{SDL_SCANCODE_KP_DIVIDE, KeyboardEvent::ScanCode::kKpDivide},
        std::pair{SDL_SCANCODE_KP_MULTIPLY, KeyboardEvent::ScanCode::kKpMultiply},
        std::pair{SDL_SCANCODE_KP_MINUS, KeyboardEvent::ScanCode::kKpMinus},
        std::pair{SDL_SCANCODE_KP_PLUS, KeyboardEvent::ScanCode::kKpPlus},
        std::pair{SDL_SCANCODE_KP_ENTER, KeyboardEvent::ScanCode::kKpEnter},
        std::pair{SDL_SCANCODE_KP_1, KeyboardEvent::ScanCode::kKp1},
        std::pair{SDL_SCANCODE_KP_2, KeyboardEvent::ScanCode::kKp2},
        std::pair{SDL_SCANCODE_KP_3, KeyboardEvent::ScanCode::kKp3},
        std::pair{SDL_SCANCODE_KP_4, KeyboardEvent::ScanCode::kKp4},
        std::pair{SDL_SCANCODE_KP_5, KeyboardEvent::ScanCode::kKp5},
        std::pair{SDL_SCANCODE_KP_6, KeyboardEvent::ScanCode::kKp6},
        std::pair{SDL_SCANCODE_KP_7, KeyboardEvent::ScanCode::kKp7},
        std::pair{SDL_SCANCODE_KP_8, KeyboardEvent::ScanCode::kKp8},
        std::pair{SDL_SCANCODE_KP_9, KeyboardEvent::ScanCode::kKp9},
        std::pair{SDL_SCANCODE_KP_0, KeyboardEvent::ScanCode::kKp0},
        std::pair{SDL_SCANCODE_KP_PERIOD, KeyboardEvent::ScanCode::kKpPeriod},
        std::pair{SDL_SCANCODE_KP_EQUALS, KeyboardEvent::ScanCode::kKpEquals},
        // Special keys (commonly used subset)
        std::pair{SDL_SCANCODE_NONUSBACKSLASH, KeyboardEvent::ScanCode::kNonUsBackslash},
        std::pair{SDL_SCANCODE_APPLICATION, KeyboardEvent::ScanCode::kApplication},
        std::pair{SDL_SCANCODE_POWER, KeyboardEvent::ScanCode::kPower},
        std::pair{SDL_SCANCODE_EXECUTE, KeyboardEvent::ScanCode::kExecute},
        std::pair{SDL_SCANCODE_HELP, KeyboardEvent::ScanCode::kHelp},
        std::pair{SDL_SCANCODE_MENU, KeyboardEvent::ScanCode::kMenu},
        std::pair{SDL_SCANCODE_SELECT, KeyboardEvent::ScanCode::kSelect},
        std::pair{SDL_SCANCODE_STOP, KeyboardEvent::ScanCode::kStop},
        std::pair{SDL_SCANCODE_AGAIN, KeyboardEvent::ScanCode::kAgain},
        std::pair{SDL_SCANCODE_UNDO, KeyboardEvent::ScanCode::kUndo},
        std::pair{SDL_SCANCODE_CUT, KeyboardEvent::ScanCode::kCut},
        std::pair{SDL_SCANCODE_COPY, KeyboardEvent::ScanCode::kCopy},
        std::pair{SDL_SCANCODE_PASTE, KeyboardEvent::ScanCode::kPaste},
        std::pair{SDL_SCANCODE_FIND, KeyboardEvent::ScanCode::kFind},
        std::pair{SDL_SCANCODE_MUTE, KeyboardEvent::ScanCode::kMute},
        std::pair{SDL_SCANCODE_VOLUMEUP, KeyboardEvent::ScanCode::kVolumeUp},
        std::pair{SDL_SCANCODE_VOLUMEDOWN, KeyboardEvent::ScanCode::kVolumeDown},
        std::pair{SDL_SCANCODE_CLEARAGAIN, KeyboardEvent::ScanCode::kClearAgain},
        // Modifier keys
        std::pair{SDL_SCANCODE_LCTRL, KeyboardEvent::ScanCode::kLCtrl},
        std::pair{SDL_SCANCODE_LSHIFT, KeyboardEvent::ScanCode::kLShift},
        std::pair{SDL_SCANCODE_LALT, KeyboardEvent::ScanCode::kLAlt},
        std::pair{SDL_SCANCODE_LGUI, KeyboardEvent::ScanCode::kLGui},
        std::pair{SDL_SCANCODE_RCTRL, KeyboardEvent::ScanCode::kRCtrl},
        std::pair{SDL_SCANCODE_RSHIFT, KeyboardEvent::ScanCode::kRShift},
        std::pair{SDL_SCANCODE_RALT, KeyboardEvent::ScanCode::kRAlt},
        std::pair{SDL_SCANCODE_RGUI, KeyboardEvent::ScanCode::kRGui},
        std::pair{SDL_SCANCODE_MODE, KeyboardEvent::ScanCode::kMode},
        // Media keys (subset)
        std::pair{SDL_SCANCODE_MEDIA_PLAY, KeyboardEvent::ScanCode::kMediaPlay},
        std::pair{SDL_SCANCODE_MEDIA_PAUSE, KeyboardEvent::ScanCode::kMediaPause},
        std::pair{SDL_SCANCODE_MEDIA_STOP, KeyboardEvent::ScanCode::kMediaStop},
        std::pair{SDL_SCANCODE_MEDIA_NEXT_TRACK, KeyboardEvent::ScanCode::kMediaNextTrack},
        std::pair{SDL_SCANCODE_MEDIA_PREVIOUS_TRACK, KeyboardEvent::ScanCode::kMediaPreviousTrack}
};
static constexpr auto kSdlScancodeMap = Map(kSdlScancodeMapArray);

static constexpr auto kSdlKeycodeMapArray = std::array{
        std::pair{SDLK_UNKNOWN, KeyboardEvent::KeyCode::kUnknown},
        // Basic ASCII characters
        std::pair{SDLK_RETURN, KeyboardEvent::KeyCode::kReturn},
        std::pair{SDLK_ESCAPE, KeyboardEvent::KeyCode::kEscape},
        std::pair{SDLK_BACKSPACE, KeyboardEvent::KeyCode::kBackspace},
        std::pair{SDLK_TAB, KeyboardEvent::KeyCode::kTab},
        std::pair{SDLK_SPACE, KeyboardEvent::KeyCode::kSpace},
        std::pair{SDLK_EXCLAIM, KeyboardEvent::KeyCode::kExclaim},
        std::pair{SDLK_DBLAPOSTROPHE, KeyboardEvent::KeyCode::kDblApostrophe},
        std::pair{SDLK_HASH, KeyboardEvent::KeyCode::kHash},
        std::pair{SDLK_DOLLAR, KeyboardEvent::KeyCode::kDollar},
        std::pair{SDLK_PERCENT, KeyboardEvent::KeyCode::kPercent},
        std::pair{SDLK_AMPERSAND, KeyboardEvent::KeyCode::kAmpersand},
        std::pair{SDLK_APOSTROPHE, KeyboardEvent::KeyCode::kApostrophe},
        std::pair{SDLK_LEFTPAREN, KeyboardEvent::KeyCode::kLeftParen},
        std::pair{SDLK_RIGHTPAREN, KeyboardEvent::KeyCode::kRightParen},
        std::pair{SDLK_ASTERISK, KeyboardEvent::KeyCode::kAsterisk},
        std::pair{SDLK_PLUS, KeyboardEvent::KeyCode::kPlus},
        std::pair{SDLK_COMMA, KeyboardEvent::KeyCode::kComma},
        std::pair{SDLK_MINUS, KeyboardEvent::KeyCode::kMinus},
        std::pair{SDLK_PERIOD, KeyboardEvent::KeyCode::kPeriod},
        std::pair{SDLK_SLASH, KeyboardEvent::KeyCode::kSlash},
        // Numbers
        std::pair{SDLK_0, KeyboardEvent::KeyCode::k0},
        std::pair{SDLK_1, KeyboardEvent::KeyCode::k1},
        std::pair{SDLK_2, KeyboardEvent::KeyCode::k2},
        std::pair{SDLK_3, KeyboardEvent::KeyCode::k3},
        std::pair{SDLK_4, KeyboardEvent::KeyCode::k4},
        std::pair{SDLK_5, KeyboardEvent::KeyCode::k5},
        std::pair{SDLK_6, KeyboardEvent::KeyCode::k6},
        std::pair{SDLK_7, KeyboardEvent::KeyCode::k7},
        std::pair{SDLK_8, KeyboardEvent::KeyCode::k8},
        std::pair{SDLK_9, KeyboardEvent::KeyCode::k9},
        // More symbols
        std::pair{SDLK_COLON, KeyboardEvent::KeyCode::kColon},
        std::pair{SDLK_SEMICOLON, KeyboardEvent::KeyCode::kSemicolon},
        std::pair{SDLK_LESS, KeyboardEvent::KeyCode::kLess},
        std::pair{SDLK_EQUALS, KeyboardEvent::KeyCode::kEquals},
        std::pair{SDLK_GREATER, KeyboardEvent::KeyCode::kGreater},
        std::pair{SDLK_QUESTION, KeyboardEvent::KeyCode::kQuestion},
        std::pair{SDLK_AT, KeyboardEvent::KeyCode::kAt},
        // Letters (lowercase)
        std::pair{SDLK_A, KeyboardEvent::KeyCode::kA},
        std::pair{SDLK_B, KeyboardEvent::KeyCode::kB},
        std::pair{SDLK_C, KeyboardEvent::KeyCode::kC},
        std::pair{SDLK_D, KeyboardEvent::KeyCode::kD},
        std::pair{SDLK_E, KeyboardEvent::KeyCode::kE},
        std::pair{SDLK_F, KeyboardEvent::KeyCode::kF},
        std::pair{SDLK_G, KeyboardEvent::KeyCode::kG},
        std::pair{SDLK_H, KeyboardEvent::KeyCode::kH},
        std::pair{SDLK_I, KeyboardEvent::KeyCode::kI},
        std::pair{SDLK_J, KeyboardEvent::KeyCode::kJ},
        std::pair{SDLK_K, KeyboardEvent::KeyCode::kK},
        std::pair{SDLK_L, KeyboardEvent::KeyCode::kL},
        std::pair{SDLK_M, KeyboardEvent::KeyCode::kM},
        std::pair{SDLK_N, KeyboardEvent::KeyCode::kN},
        std::pair{SDLK_O, KeyboardEvent::KeyCode::kO},
        std::pair{SDLK_P, KeyboardEvent::KeyCode::kP},
        std::pair{SDLK_Q, KeyboardEvent::KeyCode::kQ},
        std::pair{SDLK_R, KeyboardEvent::KeyCode::kR},
        std::pair{SDLK_S, KeyboardEvent::KeyCode::kS},
        std::pair{SDLK_T, KeyboardEvent::KeyCode::kT},
        std::pair{SDLK_U, KeyboardEvent::KeyCode::kU},
        std::pair{SDLK_V, KeyboardEvent::KeyCode::kV},
        std::pair{SDLK_W, KeyboardEvent::KeyCode::kW},
        std::pair{SDLK_X, KeyboardEvent::KeyCode::kX},
        std::pair{SDLK_Y, KeyboardEvent::KeyCode::kY},
        std::pair{SDLK_Z, KeyboardEvent::KeyCode::kZ},
        // Brackets and symbols
        std::pair{SDLK_LEFTBRACKET, KeyboardEvent::KeyCode::kLeftBracket},
        std::pair{SDLK_BACKSLASH, KeyboardEvent::KeyCode::kBackslash},
        std::pair{SDLK_RIGHTBRACKET, KeyboardEvent::KeyCode::kRightBracket},
        std::pair{SDLK_CARET, KeyboardEvent::KeyCode::kCaret},
        std::pair{SDLK_UNDERSCORE, KeyboardEvent::KeyCode::kUnderscore},
        std::pair{SDLK_GRAVE, KeyboardEvent::KeyCode::kGrave},
        std::pair{SDLK_LEFTBRACE, KeyboardEvent::KeyCode::kLeftBrace},
        std::pair{SDLK_PIPE, KeyboardEvent::KeyCode::kPipe},
        std::pair{SDLK_RIGHTBRACE, KeyboardEvent::KeyCode::kRightBrace},
        std::pair{SDLK_TILDE, KeyboardEvent::KeyCode::kTilde},
        std::pair{SDLK_DELETE, KeyboardEvent::KeyCode::kDelete},
        // Special characters
        std::pair{SDLK_PLUSMINUS, KeyboardEvent::KeyCode::kPlusMinus},
        // Function keys
        std::pair{SDLK_CAPSLOCK, KeyboardEvent::KeyCode::kCapsLock},
        std::pair{SDLK_F1, KeyboardEvent::KeyCode::kF1},
        std::pair{SDLK_F2, KeyboardEvent::KeyCode::kF2},
        std::pair{SDLK_F3, KeyboardEvent::KeyCode::kF3},
        std::pair{SDLK_F4, KeyboardEvent::KeyCode::kF4},
        std::pair{SDLK_F5, KeyboardEvent::KeyCode::kF5},
        std::pair{SDLK_F6, KeyboardEvent::KeyCode::kF6},
        std::pair{SDLK_F7, KeyboardEvent::KeyCode::kF7},
        std::pair{SDLK_F8, KeyboardEvent::KeyCode::kF8},
        std::pair{SDLK_F9, KeyboardEvent::KeyCode::kF9},
        std::pair{SDLK_F10, KeyboardEvent::KeyCode::kF10},
        std::pair{SDLK_F11, KeyboardEvent::KeyCode::kF11},
        std::pair{SDLK_F12, KeyboardEvent::KeyCode::kF12},
        std::pair{SDLK_F13, KeyboardEvent::KeyCode::kF13},
        std::pair{SDLK_F14, KeyboardEvent::KeyCode::kF14},
        std::pair{SDLK_F15, KeyboardEvent::KeyCode::kF15},
        std::pair{SDLK_F16, KeyboardEvent::KeyCode::kF16},
        std::pair{SDLK_F17, KeyboardEvent::KeyCode::kF17},
        std::pair{SDLK_F18, KeyboardEvent::KeyCode::kF18},
        std::pair{SDLK_F19, KeyboardEvent::KeyCode::kF19},
        std::pair{SDLK_F20, KeyboardEvent::KeyCode::kF20},
        std::pair{SDLK_F21, KeyboardEvent::KeyCode::kF21},
        std::pair{SDLK_F22, KeyboardEvent::KeyCode::kF22},
        std::pair{SDLK_F23, KeyboardEvent::KeyCode::kF23},
        std::pair{SDLK_F24, KeyboardEvent::KeyCode::kF24},
        // Navigation and editing keys
        std::pair{SDLK_PRINTSCREEN, KeyboardEvent::KeyCode::kPrintScreen},
        std::pair{SDLK_SCROLLLOCK, KeyboardEvent::KeyCode::kScrollLock},
        std::pair{SDLK_PAUSE, KeyboardEvent::KeyCode::kPause},
        std::pair{SDLK_INSERT, KeyboardEvent::KeyCode::kInsert},
        std::pair{SDLK_HOME, KeyboardEvent::KeyCode::kHome},
        std::pair{SDLK_PAGEUP, KeyboardEvent::KeyCode::kPageUp},
        std::pair{SDLK_END, KeyboardEvent::KeyCode::kEnd},
        std::pair{SDLK_PAGEDOWN, KeyboardEvent::KeyCode::kPageDown},
        std::pair{SDLK_RIGHT, KeyboardEvent::KeyCode::kRight},
        std::pair{SDLK_LEFT, KeyboardEvent::KeyCode::kLeft},
        std::pair{SDLK_DOWN, KeyboardEvent::KeyCode::kDown},
        std::pair{SDLK_UP, KeyboardEvent::KeyCode::kUp},
        // Keypad keys
        std::pair{SDLK_NUMLOCKCLEAR, KeyboardEvent::KeyCode::kNumLockClear},
        std::pair{SDLK_KP_DIVIDE, KeyboardEvent::KeyCode::kKpDivide},
        std::pair{SDLK_KP_MULTIPLY, KeyboardEvent::KeyCode::kKpMultiply},
        std::pair{SDLK_KP_MINUS, KeyboardEvent::KeyCode::kKpMinus},
        std::pair{SDLK_KP_PLUS, KeyboardEvent::KeyCode::kKpPlus},
        std::pair{SDLK_KP_ENTER, KeyboardEvent::KeyCode::kKpEnter},
        std::pair{SDLK_KP_1, KeyboardEvent::KeyCode::kKp1},
        std::pair{SDLK_KP_2, KeyboardEvent::KeyCode::kKp2},
        std::pair{SDLK_KP_3, KeyboardEvent::KeyCode::kKp3},
        std::pair{SDLK_KP_4, KeyboardEvent::KeyCode::kKp4},
        std::pair{SDLK_KP_5, KeyboardEvent::KeyCode::kKp5},
        std::pair{SDLK_KP_6, KeyboardEvent::KeyCode::kKp6},
        std::pair{SDLK_KP_7, KeyboardEvent::KeyCode::kKp7},
        std::pair{SDLK_KP_8, KeyboardEvent::KeyCode::kKp8},
        std::pair{SDLK_KP_9, KeyboardEvent::KeyCode::kKp9},
        std::pair{SDLK_KP_0, KeyboardEvent::KeyCode::kKp0},
        std::pair{SDLK_KP_PERIOD, KeyboardEvent::KeyCode::kKpPeriod},
        std::pair{SDLK_KP_EQUALS, KeyboardEvent::KeyCode::kKpEquals},
        // Application keys
        std::pair{SDLK_APPLICATION, KeyboardEvent::KeyCode::kApplication},
        std::pair{SDLK_POWER, KeyboardEvent::KeyCode::kPower},
        std::pair{SDLK_EXECUTE, KeyboardEvent::KeyCode::kExecute},
        std::pair{SDLK_HELP, KeyboardEvent::KeyCode::kHelp},
        std::pair{SDLK_MENU, KeyboardEvent::KeyCode::kMenu},
        std::pair{SDLK_SELECT, KeyboardEvent::KeyCode::kSelect},
        std::pair{SDLK_STOP, KeyboardEvent::KeyCode::kStop},
        std::pair{SDLK_AGAIN, KeyboardEvent::KeyCode::kAgain},
        std::pair{SDLK_UNDO, KeyboardEvent::KeyCode::kUndo},
        std::pair{SDLK_CUT, KeyboardEvent::KeyCode::kCut},
        std::pair{SDLK_COPY, KeyboardEvent::KeyCode::kCopy},
        std::pair{SDLK_PASTE, KeyboardEvent::KeyCode::kPaste},
        std::pair{SDLK_FIND, KeyboardEvent::KeyCode::kFind},
        std::pair{SDLK_MUTE, KeyboardEvent::KeyCode::kMute},
        std::pair{SDLK_VOLUMEUP, KeyboardEvent::KeyCode::kVolumeUp},
        std::pair{SDLK_VOLUMEDOWN, KeyboardEvent::KeyCode::kVolumeDown},
        // Modifier keys
        std::pair{SDLK_LCTRL, KeyboardEvent::KeyCode::kLCtrl},
        std::pair{SDLK_LSHIFT, KeyboardEvent::KeyCode::kLShift},
        std::pair{SDLK_LALT, KeyboardEvent::KeyCode::kLAlt},
        std::pair{SDLK_LGUI, KeyboardEvent::KeyCode::kLGui},
        std::pair{SDLK_RCTRL, KeyboardEvent::KeyCode::kRCtrl},
        std::pair{SDLK_RSHIFT, KeyboardEvent::KeyCode::kRShift},
        std::pair{SDLK_RALT, KeyboardEvent::KeyCode::kRAlt},
        std::pair{SDLK_RGUI, KeyboardEvent::KeyCode::kRGui}
};
static constexpr auto kSdlKeycodeMap = Map(kSdlKeycodeMapArray);

static constexpr auto kSdlKeyModifierMapArray = std::array{
        std::pair{SDL_KMOD_NONE, KeyboardEvent::KeyModifier::kNone},
        std::pair{SDL_KMOD_LSHIFT, KeyboardEvent::KeyModifier::kLShift},
        std::pair{SDL_KMOD_RSHIFT, KeyboardEvent::KeyModifier::kRShift},
        std::pair{SDL_KMOD_LEVEL5, KeyboardEvent::KeyModifier::kLevel5},
        std::pair{SDL_KMOD_LCTRL, KeyboardEvent::KeyModifier::kLCtrl},
        std::pair{SDL_KMOD_RCTRL, KeyboardEvent::KeyModifier::kRCtrl},
        std::pair{SDL_KMOD_LALT, KeyboardEvent::KeyModifier::kLAlt},
        std::pair{SDL_KMOD_RALT, KeyboardEvent::KeyModifier::kRAlt},
        std::pair{SDL_KMOD_LGUI, KeyboardEvent::KeyModifier::kLGui},
        std::pair{SDL_KMOD_RGUI, KeyboardEvent::KeyModifier::kRGui},
        std::pair{SDL_KMOD_NUM, KeyboardEvent::KeyModifier::kNum},
        std::pair{SDL_KMOD_CAPS, KeyboardEvent::KeyModifier::kCaps},
        std::pair{SDL_KMOD_MODE, KeyboardEvent::KeyModifier::kMode},
        std::pair{SDL_KMOD_SCROLL, KeyboardEvent::KeyModifier::kScroll},
        std::pair{SDL_KMOD_CTRL, KeyboardEvent::KeyModifier::kCtrl},
        std::pair{SDL_KMOD_SHIFT, KeyboardEvent::KeyModifier::kShift},
        std::pair{SDL_KMOD_ALT, KeyboardEvent::KeyModifier::kAlt},
        std::pair{SDL_KMOD_GUI, KeyboardEvent::KeyModifier::kGui}
};
static constexpr auto kSdlKeyModifierMap = Map(kSdlKeyModifierMapArray);

// Reverse maps for converting back to SDL types
static constexpr auto kKeycodeToSdlMapArray = std::array{
    std::pair{KeyboardEvent::KeyCode::kUnknown, SDLK_UNKNOWN},
    // Basic ASCII characters
    std::pair{KeyboardEvent::KeyCode::kReturn, SDLK_RETURN},
    std::pair{KeyboardEvent::KeyCode::kEscape, SDLK_ESCAPE},
    std::pair{KeyboardEvent::KeyCode::kBackspace, SDLK_BACKSPACE},
    std::pair{KeyboardEvent::KeyCode::kTab, SDLK_TAB},
    std::pair{KeyboardEvent::KeyCode::kSpace, SDLK_SPACE},
    std::pair{KeyboardEvent::KeyCode::kExclaim, SDLK_EXCLAIM},
    std::pair{KeyboardEvent::KeyCode::kDblApostrophe, SDLK_DBLAPOSTROPHE},
    std::pair{KeyboardEvent::KeyCode::kHash, SDLK_HASH},
    std::pair{KeyboardEvent::KeyCode::kDollar, SDLK_DOLLAR},
    std::pair{KeyboardEvent::KeyCode::kPercent, SDLK_PERCENT},
    std::pair{KeyboardEvent::KeyCode::kAmpersand, SDLK_AMPERSAND},
    std::pair{KeyboardEvent::KeyCode::kApostrophe, SDLK_APOSTROPHE},
    std::pair{KeyboardEvent::KeyCode::kLeftParen, SDLK_LEFTPAREN},
    std::pair{KeyboardEvent::KeyCode::kRightParen, SDLK_RIGHTPAREN},
    std::pair{KeyboardEvent::KeyCode::kAsterisk, SDLK_ASTERISK},
    std::pair{KeyboardEvent::KeyCode::kPlus, SDLK_PLUS},
    std::pair{KeyboardEvent::KeyCode::kComma, SDLK_COMMA},
    std::pair{KeyboardEvent::KeyCode::kMinus, SDLK_MINUS},
    std::pair{KeyboardEvent::KeyCode::kPeriod, SDLK_PERIOD},
    std::pair{KeyboardEvent::KeyCode::kSlash, SDLK_SLASH},
    // Numbers
    std::pair{KeyboardEvent::KeyCode::k0, SDLK_0},
    std::pair{KeyboardEvent::KeyCode::k1, SDLK_1},
    std::pair{KeyboardEvent::KeyCode::k2, SDLK_2},
    std::pair{KeyboardEvent::KeyCode::k3, SDLK_3},
    std::pair{KeyboardEvent::KeyCode::k4, SDLK_4},
    std::pair{KeyboardEvent::KeyCode::k5, SDLK_5},
    std::pair{KeyboardEvent::KeyCode::k6, SDLK_6},
    std::pair{KeyboardEvent::KeyCode::k7, SDLK_7},
    std::pair{KeyboardEvent::KeyCode::k8, SDLK_8},
    std::pair{KeyboardEvent::KeyCode::k9, SDLK_9},
    // More symbols
    std::pair{KeyboardEvent::KeyCode::kColon, SDLK_COLON},
    std::pair{KeyboardEvent::KeyCode::kSemicolon, SDLK_SEMICOLON},
    std::pair{KeyboardEvent::KeyCode::kLess, SDLK_LESS},
    std::pair{KeyboardEvent::KeyCode::kEquals, SDLK_EQUALS},
    std::pair{KeyboardEvent::KeyCode::kGreater, SDLK_GREATER},
    std::pair{KeyboardEvent::KeyCode::kQuestion, SDLK_QUESTION},
    std::pair{KeyboardEvent::KeyCode::kAt, SDLK_AT},
    // Letters (lowercase)
    std::pair{KeyboardEvent::KeyCode::kA, SDLK_A},
    std::pair{KeyboardEvent::KeyCode::kB, SDLK_B},
    std::pair{KeyboardEvent::KeyCode::kC, SDLK_C},
    std::pair{KeyboardEvent::KeyCode::kD, SDLK_D},
    std::pair{KeyboardEvent::KeyCode::kE, SDLK_E},
    std::pair{KeyboardEvent::KeyCode::kF, SDLK_F},
    std::pair{KeyboardEvent::KeyCode::kG, SDLK_G},
    std::pair{KeyboardEvent::KeyCode::kH, SDLK_H},
    std::pair{KeyboardEvent::KeyCode::kI, SDLK_I},
    std::pair{KeyboardEvent::KeyCode::kJ, SDLK_J},
    std::pair{KeyboardEvent::KeyCode::kK, SDLK_K},
    std::pair{KeyboardEvent::KeyCode::kL, SDLK_L},
    std::pair{KeyboardEvent::KeyCode::kM, SDLK_M},
    std::pair{KeyboardEvent::KeyCode::kN, SDLK_N},
    std::pair{KeyboardEvent::KeyCode::kO, SDLK_O},
    std::pair{KeyboardEvent::KeyCode::kP, SDLK_P},
    std::pair{KeyboardEvent::KeyCode::kQ, SDLK_Q},
    std::pair{KeyboardEvent::KeyCode::kR, SDLK_R},
    std::pair{KeyboardEvent::KeyCode::kS, SDLK_S},
    std::pair{KeyboardEvent::KeyCode::kT, SDLK_T},
    std::pair{KeyboardEvent::KeyCode::kU, SDLK_U},
    std::pair{KeyboardEvent::KeyCode::kV, SDLK_V},
    std::pair{KeyboardEvent::KeyCode::kW, SDLK_W},
    std::pair{KeyboardEvent::KeyCode::kX, SDLK_X},
    std::pair{KeyboardEvent::KeyCode::kY, SDLK_Y},
    std::pair{KeyboardEvent::KeyCode::kZ, SDLK_Z},
    // Brackets and symbols
    std::pair{KeyboardEvent::KeyCode::kLeftBracket, SDLK_LEFTBRACKET},
    std::pair{KeyboardEvent::KeyCode::kBackslash, SDLK_BACKSLASH},
    std::pair{KeyboardEvent::KeyCode::kRightBracket, SDLK_RIGHTBRACKET},
    std::pair{KeyboardEvent::KeyCode::kCaret, SDLK_CARET},
    std::pair{KeyboardEvent::KeyCode::kUnderscore, SDLK_UNDERSCORE},
    std::pair{KeyboardEvent::KeyCode::kGrave, SDLK_GRAVE},
    std::pair{KeyboardEvent::KeyCode::kLeftBrace, SDLK_LEFTBRACE},
    std::pair{KeyboardEvent::KeyCode::kPipe, SDLK_PIPE},
    std::pair{KeyboardEvent::KeyCode::kRightBrace, SDLK_RIGHTBRACE},
    std::pair{KeyboardEvent::KeyCode::kTilde, SDLK_TILDE},
    std::pair{KeyboardEvent::KeyCode::kDelete, SDLK_DELETE},
    // Special characters
    std::pair{KeyboardEvent::KeyCode::kPlusMinus, SDLK_PLUSMINUS},
    // Function keys
    std::pair{KeyboardEvent::KeyCode::kCapsLock, SDLK_CAPSLOCK},
    std::pair{KeyboardEvent::KeyCode::kF1, SDLK_F1},
    std::pair{KeyboardEvent::KeyCode::kF2, SDLK_F2},
    std::pair{KeyboardEvent::KeyCode::kF3, SDLK_F3},
    std::pair{KeyboardEvent::KeyCode::kF4, SDLK_F4},
    std::pair{KeyboardEvent::KeyCode::kF5, SDLK_F5},
    std::pair{KeyboardEvent::KeyCode::kF6, SDLK_F6},
    std::pair{KeyboardEvent::KeyCode::kF7, SDLK_F7},
    std::pair{KeyboardEvent::KeyCode::kF8, SDLK_F8},
    std::pair{KeyboardEvent::KeyCode::kF9, SDLK_F9},
    std::pair{KeyboardEvent::KeyCode::kF10, SDLK_F10},
    std::pair{KeyboardEvent::KeyCode::kF11, SDLK_F11},
    std::pair{KeyboardEvent::KeyCode::kF12, SDLK_F12},
    std::pair{KeyboardEvent::KeyCode::kF13, SDLK_F13},
    std::pair{KeyboardEvent::KeyCode::kF14, SDLK_F14},
    std::pair{KeyboardEvent::KeyCode::kF15, SDLK_F15},
    std::pair{KeyboardEvent::KeyCode::kF16, SDLK_F16},
    std::pair{KeyboardEvent::KeyCode::kF17, SDLK_F17},
    std::pair{KeyboardEvent::KeyCode::kF18, SDLK_F18},
    std::pair{KeyboardEvent::KeyCode::kF19, SDLK_F19},
    std::pair{KeyboardEvent::KeyCode::kF20, SDLK_F20},
    std::pair{KeyboardEvent::KeyCode::kF21, SDLK_F21},
    std::pair{KeyboardEvent::KeyCode::kF22, SDLK_F22},
    std::pair{KeyboardEvent::KeyCode::kF23, SDLK_F23},
    std::pair{KeyboardEvent::KeyCode::kF24, SDLK_F24},
    // Navigation and editing keys
    std::pair{KeyboardEvent::KeyCode::kPrintScreen, SDLK_PRINTSCREEN},
    std::pair{KeyboardEvent::KeyCode::kScrollLock, SDLK_SCROLLLOCK},
    std::pair{KeyboardEvent::KeyCode::kPause, SDLK_PAUSE},
    std::pair{KeyboardEvent::KeyCode::kInsert, SDLK_INSERT},
    std::pair{KeyboardEvent::KeyCode::kHome, SDLK_HOME},
    std::pair{KeyboardEvent::KeyCode::kPageUp, SDLK_PAGEUP},
    std::pair{KeyboardEvent::KeyCode::kEnd, SDLK_END},
    std::pair{KeyboardEvent::KeyCode::kPageDown, SDLK_PAGEDOWN},
    std::pair{KeyboardEvent::KeyCode::kRight, SDLK_RIGHT},
    std::pair{KeyboardEvent::KeyCode::kLeft, SDLK_LEFT},
    std::pair{KeyboardEvent::KeyCode::kDown, SDLK_DOWN},
    std::pair{KeyboardEvent::KeyCode::kUp, SDLK_UP},
    // Keypad keys
    std::pair{KeyboardEvent::KeyCode::kNumLockClear, SDLK_NUMLOCKCLEAR},
    std::pair{KeyboardEvent::KeyCode::kKpDivide, SDLK_KP_DIVIDE},
    std::pair{KeyboardEvent::KeyCode::kKpMultiply, SDLK_KP_MULTIPLY},
    std::pair{KeyboardEvent::KeyCode::kKpMinus, SDLK_KP_MINUS},
    std::pair{KeyboardEvent::KeyCode::kKpPlus, SDLK_KP_PLUS},
    std::pair{KeyboardEvent::KeyCode::kKpEnter, SDLK_KP_ENTER},
    std::pair{KeyboardEvent::KeyCode::kKp1, SDLK_KP_1},
    std::pair{KeyboardEvent::KeyCode::kKp2, SDLK_KP_2},
    std::pair{KeyboardEvent::KeyCode::kKp3, SDLK_KP_3},
    std::pair{KeyboardEvent::KeyCode::kKp4, SDLK_KP_4},
    std::pair{KeyboardEvent::KeyCode::kKp5, SDLK_KP_5},
    std::pair{KeyboardEvent::KeyCode::kKp6, SDLK_KP_6},
    std::pair{KeyboardEvent::KeyCode::kKp7, SDLK_KP_7},
    std::pair{KeyboardEvent::KeyCode::kKp8, SDLK_KP_8},
    std::pair{KeyboardEvent::KeyCode::kKp9, SDLK_KP_9},
    std::pair{KeyboardEvent::KeyCode::kKp0, SDLK_KP_0},
    std::pair{KeyboardEvent::KeyCode::kKpPeriod, SDLK_KP_PERIOD},
    std::pair{KeyboardEvent::KeyCode::kKpEquals, SDLK_KP_EQUALS},
    // Application keys
    std::pair{KeyboardEvent::KeyCode::kApplication, SDLK_APPLICATION},
    std::pair{KeyboardEvent::KeyCode::kPower, SDLK_POWER},
    std::pair{KeyboardEvent::KeyCode::kExecute, SDLK_EXECUTE},
    std::pair{KeyboardEvent::KeyCode::kHelp, SDLK_HELP},
    std::pair{KeyboardEvent::KeyCode::kMenu, SDLK_MENU},
    std::pair{KeyboardEvent::KeyCode::kSelect, SDLK_SELECT},
    std::pair{KeyboardEvent::KeyCode::kStop, SDLK_STOP},
    std::pair{KeyboardEvent::KeyCode::kAgain, SDLK_AGAIN},
    std::pair{KeyboardEvent::KeyCode::kUndo, SDLK_UNDO},
    std::pair{KeyboardEvent::KeyCode::kCut, SDLK_CUT},
    std::pair{KeyboardEvent::KeyCode::kCopy, SDLK_COPY},
    std::pair{KeyboardEvent::KeyCode::kPaste, SDLK_PASTE},
    std::pair{KeyboardEvent::KeyCode::kFind, SDLK_FIND},
    std::pair{KeyboardEvent::KeyCode::kMute, SDLK_MUTE},
    std::pair{KeyboardEvent::KeyCode::kVolumeUp, SDLK_VOLUMEUP},
    std::pair{KeyboardEvent::KeyCode::kVolumeDown, SDLK_VOLUMEDOWN},
    // Modifier keys
    std::pair{KeyboardEvent::KeyCode::kLCtrl, SDLK_LCTRL},
    std::pair{KeyboardEvent::KeyCode::kLShift, SDLK_LSHIFT},
    std::pair{KeyboardEvent::KeyCode::kLAlt, SDLK_LALT},
    std::pair{KeyboardEvent::KeyCode::kLGui, SDLK_LGUI},
    std::pair{KeyboardEvent::KeyCode::kRCtrl, SDLK_RCTRL},
    std::pair{KeyboardEvent::KeyCode::kRShift, SDLK_RSHIFT},
    std::pair{KeyboardEvent::KeyCode::kRAlt, SDLK_RALT},
    std::pair{KeyboardEvent::KeyCode::kRGui, SDLK_RGUI}
};
static constexpr auto kKeycodeToSdlMap = Map(kKeycodeToSdlMapArray);

static constexpr auto kScancodeToSdlMapArray = std::array{
    std::pair{KeyboardEvent::ScanCode::kUnknown, SDL_SCANCODE_UNKNOWN},
    // Letters
    std::pair{KeyboardEvent::ScanCode::kA, SDL_SCANCODE_A},
    std::pair{KeyboardEvent::ScanCode::kB, SDL_SCANCODE_B},
    std::pair{KeyboardEvent::ScanCode::kC, SDL_SCANCODE_C},
    std::pair{KeyboardEvent::ScanCode::kD, SDL_SCANCODE_D},
    std::pair{KeyboardEvent::ScanCode::kE, SDL_SCANCODE_E},
    std::pair{KeyboardEvent::ScanCode::kF, SDL_SCANCODE_F},
    std::pair{KeyboardEvent::ScanCode::kG, SDL_SCANCODE_G},
    std::pair{KeyboardEvent::ScanCode::kH, SDL_SCANCODE_H},
    std::pair{KeyboardEvent::ScanCode::kI, SDL_SCANCODE_I},
    std::pair{KeyboardEvent::ScanCode::kJ, SDL_SCANCODE_J},
    std::pair{KeyboardEvent::ScanCode::kK, SDL_SCANCODE_K},
    std::pair{KeyboardEvent::ScanCode::kL, SDL_SCANCODE_L},
    std::pair{KeyboardEvent::ScanCode::kM, SDL_SCANCODE_M},
    std::pair{KeyboardEvent::ScanCode::kN, SDL_SCANCODE_N},
    std::pair{KeyboardEvent::ScanCode::kO, SDL_SCANCODE_O},
    std::pair{KeyboardEvent::ScanCode::kP, SDL_SCANCODE_P},
    std::pair{KeyboardEvent::ScanCode::kQ, SDL_SCANCODE_Q},
    std::pair{KeyboardEvent::ScanCode::kR, SDL_SCANCODE_R},
    std::pair{KeyboardEvent::ScanCode::kS, SDL_SCANCODE_S},
    std::pair{KeyboardEvent::ScanCode::kT, SDL_SCANCODE_T},
    std::pair{KeyboardEvent::ScanCode::kU, SDL_SCANCODE_U},
    std::pair{KeyboardEvent::ScanCode::kV, SDL_SCANCODE_V},
    std::pair{KeyboardEvent::ScanCode::kW, SDL_SCANCODE_W},
    std::pair{KeyboardEvent::ScanCode::kX, SDL_SCANCODE_X},
    std::pair{KeyboardEvent::ScanCode::kY, SDL_SCANCODE_Y},
    std::pair{KeyboardEvent::ScanCode::kZ, SDL_SCANCODE_Z},
    // Numbers
    std::pair{KeyboardEvent::ScanCode::k1, SDL_SCANCODE_1},
    std::pair{KeyboardEvent::ScanCode::k2, SDL_SCANCODE_2},
    std::pair{KeyboardEvent::ScanCode::k3, SDL_SCANCODE_3},
    std::pair{KeyboardEvent::ScanCode::k4, SDL_SCANCODE_4},
    std::pair{KeyboardEvent::ScanCode::k5, SDL_SCANCODE_5},
    std::pair{KeyboardEvent::ScanCode::k6, SDL_SCANCODE_6},
    std::pair{KeyboardEvent::ScanCode::k7, SDL_SCANCODE_7},
    std::pair{KeyboardEvent::ScanCode::k8, SDL_SCANCODE_8},
    std::pair{KeyboardEvent::ScanCode::k9, SDL_SCANCODE_9},
    std::pair{KeyboardEvent::ScanCode::k0, SDL_SCANCODE_0},
    // Common keys
    std::pair{KeyboardEvent::ScanCode::kReturn, SDL_SCANCODE_RETURN},
    std::pair{KeyboardEvent::ScanCode::kEscape, SDL_SCANCODE_ESCAPE},
    std::pair{KeyboardEvent::ScanCode::kBackspace, SDL_SCANCODE_BACKSPACE},
    std::pair{KeyboardEvent::ScanCode::kTab, SDL_SCANCODE_TAB},
    std::pair{KeyboardEvent::ScanCode::kSpace, SDL_SCANCODE_SPACE},
    std::pair{KeyboardEvent::ScanCode::kMinus, SDL_SCANCODE_MINUS},
    std::pair{KeyboardEvent::ScanCode::kEquals, SDL_SCANCODE_EQUALS},
    std::pair{KeyboardEvent::ScanCode::kLeftBracket, SDL_SCANCODE_LEFTBRACKET},
    std::pair{KeyboardEvent::ScanCode::kRightBracket, SDL_SCANCODE_RIGHTBRACKET},
    std::pair{KeyboardEvent::ScanCode::kBackslash, SDL_SCANCODE_BACKSLASH},
    std::pair{KeyboardEvent::ScanCode::kNonUsHash, SDL_SCANCODE_NONUSHASH},
    std::pair{KeyboardEvent::ScanCode::kSemicolon, SDL_SCANCODE_SEMICOLON},
    std::pair{KeyboardEvent::ScanCode::kApostrophe, SDL_SCANCODE_APOSTROPHE},
    std::pair{KeyboardEvent::ScanCode::kGrave, SDL_SCANCODE_GRAVE},
    std::pair{KeyboardEvent::ScanCode::kComma, SDL_SCANCODE_COMMA},
    std::pair{KeyboardEvent::ScanCode::kPeriod, SDL_SCANCODE_PERIOD},
    std::pair{KeyboardEvent::ScanCode::kSlash, SDL_SCANCODE_SLASH},
    std::pair{KeyboardEvent::ScanCode::kCapsLock, SDL_SCANCODE_CAPSLOCK},
    // Function keys
    std::pair{KeyboardEvent::ScanCode::kF1, SDL_SCANCODE_F1},
    std::pair{KeyboardEvent::ScanCode::kF2, SDL_SCANCODE_F2},
    std::pair{KeyboardEvent::ScanCode::kF3, SDL_SCANCODE_F3},
    std::pair{KeyboardEvent::ScanCode::kF4, SDL_SCANCODE_F4},
    std::pair{KeyboardEvent::ScanCode::kF5, SDL_SCANCODE_F5},
    std::pair{KeyboardEvent::ScanCode::kF6, SDL_SCANCODE_F6},
    std::pair{KeyboardEvent::ScanCode::kF7, SDL_SCANCODE_F7},
    std::pair{KeyboardEvent::ScanCode::kF8, SDL_SCANCODE_F8},
    std::pair{KeyboardEvent::ScanCode::kF9, SDL_SCANCODE_F9},
    std::pair{KeyboardEvent::ScanCode::kF10, SDL_SCANCODE_F10},
    std::pair{KeyboardEvent::ScanCode::kF11, SDL_SCANCODE_F11},
    std::pair{KeyboardEvent::ScanCode::kF12, SDL_SCANCODE_F12},
    std::pair{KeyboardEvent::ScanCode::kF13, SDL_SCANCODE_F13},
    std::pair{KeyboardEvent::ScanCode::kF14, SDL_SCANCODE_F14},
    std::pair{KeyboardEvent::ScanCode::kF15, SDL_SCANCODE_F15},
    std::pair{KeyboardEvent::ScanCode::kF16, SDL_SCANCODE_F16},
    std::pair{KeyboardEvent::ScanCode::kF17, SDL_SCANCODE_F17},
    std::pair{KeyboardEvent::ScanCode::kF18, SDL_SCANCODE_F18},
    std::pair{KeyboardEvent::ScanCode::kF19, SDL_SCANCODE_F19},
    std::pair{KeyboardEvent::ScanCode::kF20, SDL_SCANCODE_F20},
    std::pair{KeyboardEvent::ScanCode::kF21, SDL_SCANCODE_F21},
    std::pair{KeyboardEvent::ScanCode::kF22, SDL_SCANCODE_F22},
    std::pair{KeyboardEvent::ScanCode::kF23, SDL_SCANCODE_F23},
    std::pair{KeyboardEvent::ScanCode::kF24, SDL_SCANCODE_F24},
    // Navigation and editing keys
    std::pair{KeyboardEvent::ScanCode::kPrintScreen, SDL_SCANCODE_PRINTSCREEN},
    std::pair{KeyboardEvent::ScanCode::kScrollLock, SDL_SCANCODE_SCROLLLOCK},
    std::pair{KeyboardEvent::ScanCode::kPause, SDL_SCANCODE_PAUSE},
    std::pair{KeyboardEvent::ScanCode::kInsert, SDL_SCANCODE_INSERT},
    std::pair{KeyboardEvent::ScanCode::kHome, SDL_SCANCODE_HOME},
    std::pair{KeyboardEvent::ScanCode::kPageUp, SDL_SCANCODE_PAGEUP},
    std::pair{KeyboardEvent::ScanCode::kDelete, SDL_SCANCODE_DELETE},
    std::pair{KeyboardEvent::ScanCode::kEnd, SDL_SCANCODE_END},
    std::pair{KeyboardEvent::ScanCode::kPageDown, SDL_SCANCODE_PAGEDOWN},
    std::pair{KeyboardEvent::ScanCode::kRight, SDL_SCANCODE_RIGHT},
    std::pair{KeyboardEvent::ScanCode::kLeft, SDL_SCANCODE_LEFT},
    std::pair{KeyboardEvent::ScanCode::kDown, SDL_SCANCODE_DOWN},
    std::pair{KeyboardEvent::ScanCode::kUp, SDL_SCANCODE_UP},
    // Keypad keys
    std::pair{KeyboardEvent::ScanCode::kNumLockClear, SDL_SCANCODE_NUMLOCKCLEAR},
    std::pair{KeyboardEvent::ScanCode::kKpDivide, SDL_SCANCODE_KP_DIVIDE},
    std::pair{KeyboardEvent::ScanCode::kKpMultiply, SDL_SCANCODE_KP_MULTIPLY},
    std::pair{KeyboardEvent::ScanCode::kKpMinus, SDL_SCANCODE_KP_MINUS},
    std::pair{KeyboardEvent::ScanCode::kKpPlus, SDL_SCANCODE_KP_PLUS},
    std::pair{KeyboardEvent::ScanCode::kKpEnter, SDL_SCANCODE_KP_ENTER},
    std::pair{KeyboardEvent::ScanCode::kKp1, SDL_SCANCODE_KP_1},
    std::pair{KeyboardEvent::ScanCode::kKp2, SDL_SCANCODE_KP_2},
    std::pair{KeyboardEvent::ScanCode::kKp3, SDL_SCANCODE_KP_3},
    std::pair{KeyboardEvent::ScanCode::kKp4, SDL_SCANCODE_KP_4},
    std::pair{KeyboardEvent::ScanCode::kKp5, SDL_SCANCODE_KP_5},
    std::pair{KeyboardEvent::ScanCode::kKp6, SDL_SCANCODE_KP_6},
    std::pair{KeyboardEvent::ScanCode::kKp7, SDL_SCANCODE_KP_7},
    std::pair{KeyboardEvent::ScanCode::kKp8, SDL_SCANCODE_KP_8},
    std::pair{KeyboardEvent::ScanCode::kKp9, SDL_SCANCODE_KP_9},
    std::pair{KeyboardEvent::ScanCode::kKp0, SDL_SCANCODE_KP_0},
    std::pair{KeyboardEvent::ScanCode::kKpPeriod, SDL_SCANCODE_KP_PERIOD},
    std::pair{KeyboardEvent::ScanCode::kKpEquals, SDL_SCANCODE_KP_EQUALS},
    // Special keys
    std::pair{KeyboardEvent::ScanCode::kNonUsBackslash, SDL_SCANCODE_NONUSBACKSLASH},
    std::pair{KeyboardEvent::ScanCode::kApplication, SDL_SCANCODE_APPLICATION},
    std::pair{KeyboardEvent::ScanCode::kPower, SDL_SCANCODE_POWER},
    std::pair{KeyboardEvent::ScanCode::kExecute, SDL_SCANCODE_EXECUTE},
    std::pair{KeyboardEvent::ScanCode::kHelp, SDL_SCANCODE_HELP},
    std::pair{KeyboardEvent::ScanCode::kMenu, SDL_SCANCODE_MENU},
    std::pair{KeyboardEvent::ScanCode::kSelect, SDL_SCANCODE_SELECT},
    std::pair{KeyboardEvent::ScanCode::kStop, SDL_SCANCODE_STOP},
    std::pair{KeyboardEvent::ScanCode::kAgain, SDL_SCANCODE_AGAIN},
    std::pair{KeyboardEvent::ScanCode::kUndo, SDL_SCANCODE_UNDO},
    std::pair{KeyboardEvent::ScanCode::kCut, SDL_SCANCODE_CUT},
    std::pair{KeyboardEvent::ScanCode::kCopy, SDL_SCANCODE_COPY},
    std::pair{KeyboardEvent::ScanCode::kPaste, SDL_SCANCODE_PASTE},
    std::pair{KeyboardEvent::ScanCode::kFind, SDL_SCANCODE_FIND},
    std::pair{KeyboardEvent::ScanCode::kMute, SDL_SCANCODE_MUTE},
    std::pair{KeyboardEvent::ScanCode::kVolumeUp, SDL_SCANCODE_VOLUMEUP},
    std::pair{KeyboardEvent::ScanCode::kVolumeDown, SDL_SCANCODE_VOLUMEDOWN},
    std::pair{KeyboardEvent::ScanCode::kClearAgain, SDL_SCANCODE_CLEARAGAIN},
    // Modifier keys
    std::pair{KeyboardEvent::ScanCode::kLCtrl, SDL_SCANCODE_LCTRL},
    std::pair{KeyboardEvent::ScanCode::kLShift, SDL_SCANCODE_LSHIFT},
    std::pair{KeyboardEvent::ScanCode::kLAlt, SDL_SCANCODE_LALT},
    std::pair{KeyboardEvent::ScanCode::kLGui, SDL_SCANCODE_LGUI},
    std::pair{KeyboardEvent::ScanCode::kRCtrl, SDL_SCANCODE_RCTRL},
    std::pair{KeyboardEvent::ScanCode::kRShift, SDL_SCANCODE_RSHIFT},
    std::pair{KeyboardEvent::ScanCode::kRAlt, SDL_SCANCODE_RALT},
    std::pair{KeyboardEvent::ScanCode::kRGui, SDL_SCANCODE_RGUI},
    std::pair{KeyboardEvent::ScanCode::kMode, SDL_SCANCODE_MODE},
    // Media keys
    std::pair{KeyboardEvent::ScanCode::kMediaPlay, SDL_SCANCODE_MEDIA_PLAY},
    std::pair{KeyboardEvent::ScanCode::kMediaPause, SDL_SCANCODE_MEDIA_PAUSE},
    std::pair{KeyboardEvent::ScanCode::kMediaStop, SDL_SCANCODE_MEDIA_STOP},
    std::pair{KeyboardEvent::ScanCode::kMediaNextTrack, SDL_SCANCODE_MEDIA_NEXT_TRACK},
    std::pair{KeyboardEvent::ScanCode::kMediaPreviousTrack, SDL_SCANCODE_MEDIA_PREVIOUS_TRACK}
};
static constexpr auto kScancodeToSdlMap = Map(kScancodeToSdlMapArray);

static constexpr auto kKeyModifierToSdlMapArray = std::array{
    std::pair{KeyboardEvent::KeyModifier::kNone, SDL_KMOD_NONE},
    std::pair{KeyboardEvent::KeyModifier::kLShift, SDL_KMOD_LSHIFT},
    std::pair{KeyboardEvent::KeyModifier::kRShift, SDL_KMOD_RSHIFT},
    std::pair{KeyboardEvent::KeyModifier::kLevel5, SDL_KMOD_LEVEL5},
    std::pair{KeyboardEvent::KeyModifier::kLCtrl, SDL_KMOD_LCTRL},
    std::pair{KeyboardEvent::KeyModifier::kRCtrl, SDL_KMOD_RCTRL},
    std::pair{KeyboardEvent::KeyModifier::kLAlt, SDL_KMOD_LALT},
    std::pair{KeyboardEvent::KeyModifier::kRAlt, SDL_KMOD_RALT},
    std::pair{KeyboardEvent::KeyModifier::kLGui, SDL_KMOD_LGUI},
    std::pair{KeyboardEvent::KeyModifier::kRGui, SDL_KMOD_RGUI},
    std::pair{KeyboardEvent::KeyModifier::kNum, SDL_KMOD_NUM},
    std::pair{KeyboardEvent::KeyModifier::kCaps, SDL_KMOD_CAPS},
    std::pair{KeyboardEvent::KeyModifier::kMode, SDL_KMOD_MODE},
    std::pair{KeyboardEvent::KeyModifier::kScroll, SDL_KMOD_SCROLL},
    std::pair{KeyboardEvent::KeyModifier::kCtrl, SDL_KMOD_CTRL},
    std::pair{KeyboardEvent::KeyModifier::kShift, SDL_KMOD_SHIFT},
    std::pair{KeyboardEvent::KeyModifier::kAlt, SDL_KMOD_ALT},
    std::pair{KeyboardEvent::KeyModifier::kGui, SDL_KMOD_GUI}
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
