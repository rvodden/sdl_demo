/**
 * @file event_registration.h
 * @brief RAII token for automatic event handler deregistration
 *
 * This file contains the EventRegistration class which provides automatic
 * cleanup of event handler registrations using RAII semantics.
 */

#ifndef SDL_TOOLS_EVENT_REGISTRATION_H
#define SDL_TOOLS_EVENT_REGISTRATION_H

#include <cstdint>
#include <memory>

#include "sdl_tools_export.h"

namespace sdl::tools {

// Forward declaration
class EventRouterImpl;

/**
 * @brief RAII token that automatically deregisters an event handler on destruction
 *
 * EventRegistration provides automatic lifetime management for event handler
 * registrations. When the token is destroyed (goes out of scope), the associated
 * event handler is automatically deregistered from the EventRouter.
 *
 * This class follows RAII principles and is move-only to ensure clear ownership
 * semantics. It's designed to be used with the layer pattern where handlers need
 * to be registered in onResume() and automatically deregistered in onSuspend().
 *
 * Key features:
 * - **Automatic deregistration**: Handler is removed when token is destroyed
 * - **Move-only semantics**: Cannot be copied, only moved
 * - **Manual deregistration**: Can explicitly unregister before destruction
 * - **Safe if router destroyed**: Uses weak_ptr to safely handle router lifetime
 * - **Exception-safe**: Guaranteed cleanup even if exceptions occur
 *
 * @note This class is move-only and cannot be copied
 * @note Safe to destroy even if EventRouter was already destroyed
 * @note Multiple deregistrations (manual + destructor) are safe
 *
 * Usage example:
 * @code
 * class GameLayer {
 * public:
 *   void onResume(std::shared_ptr<EventRouter> router) {
 *     // Register handler and store token
 *     _clickHandler = router->registerEventHandler<MouseButtonEvent>([this](auto& e) {
 *       handleClick(e);
 *     });
 *   }
 *
 *   void onSuspend() {
 *     // Token auto-deregisters when cleared or goes out of scope
 *     _clickHandler = EventRegistration();  // Explicit deregister
 *     // Or just let it destruct naturally
 *   }
 *
 * private:
 *   EventRegistration _clickHandler;
 * };
 * @endcode
 */
class SDL_TOOLS_EXPORT EventRegistration {
 public:
  /**
   * @brief Default constructor creates an empty (unregistered) token
   * @note An empty token does nothing when destroyed
   */
  EventRegistration();

  /**
   * @brief Move constructor - transfers registration ownership
   * @param other The registration to move from (becomes empty)
   * @note The moved-from token is left in a valid but empty state
   * @note Only one token controls the registration after the move
   */
  EventRegistration(EventRegistration&& other) noexcept;

  /**
   * @brief Move assignment operator - transfers registration ownership
   * @param other The registration to move from (becomes empty)
   * @return Reference to this token
   * @note Deregisters current handler (if any) before taking ownership of new one
   * @note The moved-from token is left in a valid but empty state
   */
  auto operator=(EventRegistration&& other) noexcept -> EventRegistration&;

  /**
   * @brief Copy constructor is explicitly deleted
   * @note EventRegistration is move-only to ensure clear ownership
   */
  EventRegistration(const EventRegistration& other) = delete;

  /**
   * @brief Copy assignment is explicitly deleted
   * @note EventRegistration is move-only to ensure clear ownership
   */
  auto operator=(const EventRegistration& other) -> EventRegistration& = delete;

  /**
   * @brief Destructor - automatically deregisters the handler
   * @note Safe to call even if handler was already manually deregistered
   * @note Safe to call even if EventRouter was already destroyed
   * @note Guaranteed not to throw exceptions for RAII safety
   */
  ~EventRegistration();

  /**
   * @brief Manually deregister the handler before destruction
   * @note Safe to call multiple times (idempotent)
   * @note After calling, isRegistered() returns false
   * @note Destructor is still safe to call after manual deregistration
   *
   * Use this when you need to deregister before the token goes out of scope:
   * @code
   * void temporarilyDisable() {
   *   _handler.unregister();  // Stop receiving events
   *   // ... do something ...
   *   _handler = router->registerEventHandler(...);  // Re-register
   * }
   * @endcode
   */
  void unregister();

  /**
   * @brief Check if this token represents an active registration
   * @return True if handler is still registered, false otherwise
   * @note Returns false if unregister() was called
   * @note Returns false if EventRouter was destroyed
   * @note Returns false for default-constructed or moved-from tokens
   */
  [[nodiscard]] auto isRegistered() const -> bool;

 private:
  // Only EventRouter can create valid registrations
  friend class EventRouter;

  /**
   * @brief Internal constructor used by EventRouter
   * @param impl Weak pointer to the EventRouter implementation
   * @param handlerId Unique ID of the registered handler
   */
  EventRegistration(std::weak_ptr<EventRouterImpl> impl, uint64_t handlerId);

  /** @brief Weak pointer to router implementation (safe if router destroyed) */
  std::weak_ptr<EventRouterImpl> _impl;

  /** @brief Unique ID of the registered handler */
  uint64_t _handlerId;

  /** @brief Whether this token represents an active registration */
  bool _registered;
};

}  // namespace sdl::tools

#endif  // SDL_TOOLS_EVENT_REGISTRATION_H
