#ifndef SDL_MESSAGE_BOX_H
#define SDL_MESSAGE_BOX_H

#include <memory>
#include <string>
#include <vector>

#include "sdl_export.h"

namespace sdl {

class Window;
class MessageBoxImpl;

/**
 * @brief A fluent builder for creating SDL message boxes
 *
 * MessageBox provides a convenient fluent interface for creating modal message boxes
 * using SDL's native dialog functionality. It supports custom buttons, different
 * message types (error, warning, information), and flexible configuration.
 *
 * The builder pattern allows for easy configuration:
 * @code
 * auto result = MessageBox("Title", "Message")
 *     .addButton("OK")
 *     .addButton("Cancel") 
 *     .setType(MessageBox::Type::Warning)
 *     .show();
 * @endcode
 *
 * @note MessageBoxes are modal and will block execution until the user responds
 * @note The underlying SDL implementation is cross-platform and uses native dialogs where possible
 * @see Window for the parent window context
 */
class SDL_EXPORT MessageBox {
public:
    /**
     * @brief Message box type constants for different dialog styles
     */
    enum class Type {
        Information,  /**< Informational dialog with info icon */
        Warning,      /**< Warning dialog with warning icon */  
        Error         /**< Error dialog with error icon */
    };

    /**
     * @brief Button configuration for message box buttons
     */
    struct Button {
        std::string text;        /**< Button text displayed to user */
        int id;                  /**< Unique button identifier returned when clicked */
        bool isDefault = false;  /**< True if this button is activated by Return key */
        bool isCancel = false;   /**< True if this button is activated by Escape key */
        
        Button(std::string buttonText, int buttonId) : text(std::move(buttonText)), id(buttonId) {}
    };

    /**
     * @brief Result of showing a message box
     */
    struct Result {
        bool success;      /**< True if message box was shown successfully */
        int buttonId;      /**< ID of the button that was clicked (-1 if dialog was closed) */
        std::string error; /**< Error message if success is false */
    };

    /**
     * @brief Create a message box with title and message
     * @param title The dialog title displayed in the title bar
     * @param message The main message text displayed to the user
     */
    MessageBox(std::string title, std::string message);

    /**
     * @brief Move constructor
     */
    MessageBox(MessageBox&& other) noexcept;

    /**
     * @brief Copy constructor is deleted
     */
    MessageBox(const MessageBox& other) = delete;

    /**
     * @brief Destructor
     */
    ~MessageBox();

    /**
     * @brief Move assignment operator  
     */
    auto operator=(MessageBox&& other) noexcept -> MessageBox&;

    /**
     * @brief Copy assignment is deleted
     */
    auto operator=(const MessageBox& other) -> MessageBox& = delete;

    /**
     * @brief Add a button to the message box
     * @param text The button text
     * @param id Optional button ID (auto-generated if not provided)
     * @return Reference to this MessageBox for method chaining
     * 
     * Buttons are displayed in the order they are added. The first button
     * added becomes the default button (activated by Return key).
     */
    auto addButton(const std::string& text, int id = -1) -> MessageBox&;

    /**
     * @brief Set the message box type (affects icon and styling)
     * @param type The message box type
     * @return Reference to this MessageBox for method chaining
     */
    auto setType(Type type) -> MessageBox&;

    /**
     * @brief Set the parent window for the message box
     * @param window The parent window (can be null for no parent)
     * @return Reference to this MessageBox for method chaining
     * 
     * The message box will be modal relative to this window.
     */
    auto setParent(const Window* window) -> MessageBox&;

    /**
     * @brief Mark the last added button as the default (Return key) button
     * @return Reference to this MessageBox for method chaining
     * 
     * Only one button can be the default. Calling this multiple times
     * will change which button is the default.
     */
    auto makeLastButtonDefault() -> MessageBox&;

    /**
     * @brief Mark the last added button as the cancel (Escape key) button  
     * @return Reference to this MessageBox for method chaining
     * 
     * Only one button can be the cancel button. Calling this multiple times
     * will change which button handles Escape.
     */
    auto makeLastButtonCancel() -> MessageBox&;

    /**
     * @brief Display the message box and wait for user response
     * @return Result containing success status and clicked button ID
     * 
     * This method blocks execution until the user clicks a button or
     * closes the dialog. If no buttons were added, an "OK" button is
     * automatically added.
     * 
     * @note Should be called on the main thread or the thread that created the parent window
     */
    auto show() -> Result;

private:
    std::unique_ptr<MessageBoxImpl> _impl;
};

} // namespace sdl

#endif // SDL_MESSAGE_BOX_H
