#ifndef SDL_TOOLS_BUTTON_H
#define SDL_TOOLS_BUTTON_H

#include <event.h>
#include <event_router.h>
#include <rectangle.h>

#include <memory>

#include "sdlpp_tools_export.h"

namespace sdlpp::tools {

class ButtonImpl;

/**
 * @brief Interactive UI button that responds to mouse clicks within a defined rectangular area
 *
 * Button creates a clickable rectangular region that automatically handles mouse
 * events and dispatches callbacks when clicked. The button integrates seamlessly
 * with the event routing system to provide type-safe, efficient event handling
 * without requiring manual event loop management.
 *
 * The button performs precise hit testing against its rectangular bounds and only
 * fires events for clicks that occur within the defined region. It supports all
 * mouse button types (left, right, middle, etc.) and provides complete event
 * information including click position, button type, and click count.
 *
 * Buttons automatically register themselves with the provided EventRouter during
 * construction and handle cleanup during destruction, ensuring no event leaks
 * or dangling handlers.
 *
 * @note Buttons cannot be copied, only moved for performance reasons
 * @note The EventRouter must remain valid for the lifetime of the button
 * @note Multiple event handlers can be registered and will all be called
 * @see EventRouter for the event distribution system
 * @see MouseButtonEvent for the event data structure
 * @see sdlpp::Rectangle<float> for the clickable area definition
 *
 * Example usage:
 * @code
 * auto eventRouter = std::make_shared<EventRouter>(eventBus);
 * 
 * // Create button covering rectangle from (100,50) with size 200x75
 * sdlpp::Rectangle<float> bounds(100.0f, 50.0f, 200.0f, 75.0f);
 * Button playButton(eventRouter, bounds);
 * 
 * // Register click handler
 * playButton.registerEventHandler([](const MouseButtonEvent& e) {
 *     if (e.down && e.button == MouseButtonEvent::Button::kLeft) {
 *         std::cout << "Play button clicked at " << e.x << ", " << e.y << std::endl;
 *         startGame();
 *     }
 * });
 * 
 * // Button automatically handles events through the event router
 * eventRouter->run(); // or processNextEvent() for manual control
 * @endcode
 */
class SDLPP_TOOLS_EXPORT Button {
 public:
  /** @brief Type alias for button event handler functions */
  using Handler = std::function<void(const MouseButtonEvent&)>;

  /**
   * @brief Create a button with the specified clickable area
   * @param eventRouter Shared pointer to the event router that will dispatch events (must remain valid)
   * @param rectangle The rectangular area that defines the clickable region
   * @throws sdlpp::Exception if button registration with the event router fails
   *
   * Creates a new interactive button that will respond to mouse clicks within
   * the specified rectangular area. The button automatically registers itself
   * with the event router to receive mouse events and performs hit testing
   * to determine if clicks fall within its bounds.
   *
   * The rectangle defines the clickable area in screen coordinates, typically
   * matching the visual representation of the button on screen. All coordinates
   * use the standard (0,0) top-left origin convention.
   *
   * @note The event router must remain valid for the lifetime of this button
   * @note Button registration happens immediately during construction
   * @note Rectangle coordinates should match your visual button representation
   */
  Button(std::shared_ptr<EventRouter> eventRouter,
         sdlpp::Rectangle<float> rectangle);
         
  /**
   * @brief Copy constructor is explicitly deleted
   * @note Buttons cannot be copied due to event router integration complexity
   */
  Button(const Button& other) = delete;
  
  /**
   * @brief Move constructor - transfers ownership of the button and its event handlers
   * @param other The button to move from (will be left in valid but unspecified state)
   * @note The moved-from button should not be used except for destruction
   * @note Event handler registrations are transferred to the new button
   */
  Button(Button&& other) noexcept;

  /**
   * @brief Copy assignment is explicitly deleted
   * @note Buttons cannot be copied due to event router integration complexity
   */
  auto operator=(const Button& other) -> Button& = delete;
  
  /**
   * @brief Move assignment operator - transfers ownership of the button and its event handlers
   * @param other The button to move from (will be left in valid but unspecified state)
   * @return Reference to this button
   * @note The moved-from button should not be used except for destruction
   * @note Event handler registrations are transferred to this button
   */
  auto operator=(Button&& other) noexcept -> Button&;

  /**
   * @brief Destructor - automatically unregisters from event router and cleans up handlers
   * @note Guaranteed not to throw exceptions for safe RAII cleanup
   * @note All registered event handlers are automatically removed
   */
  ~Button();

  /**
   * @brief Register a new event handler for button click events
   * @param handler Function or lambda to call when the button is clicked
   *
   * Adds a new event handler that will be called whenever a mouse click occurs
   * within the button's rectangular bounds. The handler receives the complete
   * MouseButtonEvent with details including mouse position, button type, and
   * click count.
   *
   * Multiple handlers can be registered and will all be called in the order
   * they were registered. Handlers are called synchronously during event
   * processing, so avoid long-running operations that could block the event loop.
   *
   * The handler function should check the event properties to determine the
   * appropriate response (e.g., only respond to left mouse button down events).
   *
   * @note Handlers are called for all mouse events within the button area
   * @note Multiple handlers are supported and called in registration order
   * @note Handler execution is synchronous - avoid blocking operations
   * @note Use event properties (button, down, clicks) to filter specific interactions
   *
   * Example handler patterns:
   * @code
   * // Simple click handler
   * button.registerEventHandler([](const MouseButtonEvent& e) {
   *     if (e.down && e.button == MouseButtonEvent::Button::kLeft) {
   *         handleClick();
   *     }
   * });
   * 
   * // Position-aware handler
   * button.registerEventHandler([](const MouseButtonEvent& e) {
   *     if (e.down) {
   *         std::cout << "Clicked at relative position: " 
   *                   << (e.x - buttonX) << ", " << (e.y - buttonY) << std::endl;
   *     }
   * });
   * 
   * // Double-click detection
   * button.registerEventHandler([](const MouseButtonEvent& e) {
   *     if (e.down && e.clicks >= 2) {
   *         handleDoubleClick();
   *     }
   * });
   * @endcode
   */
  void registerEventHandler(Handler&& handler);

 private:
  std::unique_ptr<ButtonImpl> _impl;
};

}  // namespace sdlpp::tools

#endif
