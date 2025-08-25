# ADR-0002: Pimpl Pattern for SDL Dependencies

**Date:** 2024-08-25  
**Status:** Accepted

## Context

SDL3 is a large C library with extensive headers that include platform-specific code, macro definitions, and complex type declarations. Direct inclusion of SDL headers in our public API would create several problems:

- **Compilation Dependencies**: Users would be forced to have SDL3 headers installed and properly configured
- **Header Bloat**: Large SDL headers would slow compilation of user code significantly  
- **ABI Instability**: Changes in SDL headers would require recompilation of all user code
- **Namespace Pollution**: SDL macros and definitions would leak into user code
- **Platform Coupling**: Platform-specific SDL code would expose implementation details

We needed a way to provide a clean C++ API while completely isolating SDL dependencies from our public headers.

## Decision

We implemented the **Pimpl (Pointer to Implementation) idiom** consistently across all SDL resource wrapper classes:

### Public Header Structure
- Public headers contain only forward declarations and `std::unique_ptr<ClassImpl>`
- No SDL includes in any public header file
- Clean, dependency-free API surface

**Example (`src/sdlpp/include/surface.h`):**
```cpp
#include <memory>
#include <filesystem>
// NO SDL includes!

class SurfaceImpl;  // Forward declaration

class Surface {
public:
  Surface(const std::filesystem::path& filePath);
  // ... public interface
private:
  std::unique_ptr<SurfaceImpl> _surfaceImpl;  // Pimpl
};
```

### Implementation Header Structure  
- Implementation details isolated in `src/sdlpp/src/include/*_impl.h` files
- SDL dependencies contained within implementation
- Friend relationships for internal access

**Example (`src/sdlpp/src/include/surface_impl.h`):**
```cpp
#include "surface.h"
#include <SDL3/SDL.h>  // SDL dependency isolated here

class SurfaceImpl {
  friend class Surface;
  friend class Texture;
public:
  static auto createSurface(SDL_Surface* sdlSurface) -> Surface;
private:
  SDL_Surface* _sdlSurface = nullptr;  // Raw SDL resource
};
```

### Implementation Pattern
All classes follow the same pattern:
1. **Public constructor**: Creates `std::make_unique<ClassImpl>()`
2. **Public methods**: Delegate to `_impl->method()` calls
3. **Resource management**: RAII cleanup in destructors
4. **Move semantics**: Proper transfer of `unique_ptr` ownership

## Consequences

### Positive Outcomes

**Zero Public Dependencies**: Users can use SDL++ without any SDL headers, includes, or build configuration. The public API is completely self-contained.

**Fast Compilation**: User code compiles quickly since it doesn't process large SDL headers. Our TicTacToe example builds in seconds rather than minutes.

**ABI Stability**: Internal implementation changes don't require user code recompilation. We can upgrade SDL versions without breaking user builds.

**Clean API Surface**: Public headers are readable and focused. No SDL-specific types, macros, or platform dependencies leak through.

**Implementation Flexibility**: We can change internal SDL usage patterns, add caching, or even switch SDL versions without affecting the public API.

### Negative Consequences

**Runtime Indirection**: One extra pointer dereference per method call. In practice, this is negligible for SDL operations which are already heavyweight.

**Memory Overhead**: Additional heap allocation per object (8 bytes per `unique_ptr`). Minimal impact for typical SDL resource counts.

**Implementation Complexity**: Each class requires both public and private headers. More files to maintain and keep synchronized.

**Debugging Difficulty**: Debugger must follow pointer to reach actual SDL resources. Modern debuggers handle this well with proper debug symbols.

**Friend Relationships**: Some implementation classes need `friend` access for internal operations, slightly coupling the design.

## Alternatives Considered

### Direct SDL Exposure
**Approach**: Include SDL headers directly in public API.

**Rejected because:**
- Forces SDL dependency on all users
- Compilation would be 5-10x slower
- Platform-specific SDL code would leak into user builds
- ABI would be unstable across SDL versions

### Template-Based Abstraction
**Approach**: Heavy template usage to avoid virtual calls and pimpl overhead.

**Rejected because:**
- Would still require SDL headers in public templates
- Template error messages would expose SDL implementation details
- Longer compilation times due to template instantiation
- More complex for users to understand and debug

### Opaque Handle Pattern  
**Approach**: Return opaque handles/IDs rather than objects.

**Rejected because:**
- Not idiomatic C++ - users expect object-oriented interface
- Would require global resource management tables
- Lose automatic cleanup benefits of RAII
- Type safety issues with handle mix-ups

### Interface/Implementation Split
**Approach**: Pure virtual base classes with concrete implementations.

**Rejected because:**
- Virtual function call overhead for every operation
- More complex inheritance hierarchies
- Still need pimpl or headers for concrete classes
- Doesn't solve the SDL dependency problem

## Implementation Evidence

The pattern's effectiveness is visible throughout our codebase:

**Consistent Application**: All 8 major wrapper classes (`Window`, `Renderer`, `Surface`, `Texture`, `EventBus`, etc.) follow identical pimpl structure.

**Clean Public Headers**: Zero SDL includes in `src/sdlpp/include/` - only standard library and our own headers.

**Isolated Dependencies**: All SDL includes contained within `src/sdlpp/src/include/*_impl.h` files.

**User Experience**: TicTacToe example includes only `#include <sdl.h>` and compiles without any SDL configuration.

**Performance**: Event system handles thousands of events per second with negligible pimpl overhead compared to SDL processing costs.

**Maintainability**: Recently upgraded from SDL3 preview to newer version by changing only implementation files - zero public API changes required.

The pimpl pattern successfully provides complete dependency isolation while maintaining excellent performance and usability characteristics essential for real-time applications.

---

*This ADR addresses implementation hiding. Related decisions about resource management (ADR-0004) and overall architecture (ADR-0001) provide additional context.*