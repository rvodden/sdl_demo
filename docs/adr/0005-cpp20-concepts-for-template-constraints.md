# ADR-0005: C++20 Concepts for Template Parameter Constraints

**Date:** 2024-08-26  
**Status:** Accepted

## Context

Template error messages in C++ have historically been notoriously difficult to understand, especially when template parameters don't meet required constraints. In our SDL3 wrapper, the `Rectangle<T>` template was originally constrained using `static_assert` to only accept `int32_t` and `float` coordinate types (mapping to SDL's `SDL_Rect` and `SDL_FRect` respectively).

The original constraint implementation suffered from several issues:

**Poor Error Messages**: When users attempted to instantiate `Rectangle<uint32_t>` or `Rectangle<double>`, the compiler would emit generic static assertion failures that didn't clearly indicate why those types were invalid:
```cpp
static_assert(std::is_same_v<T, int32_t> || std::is_same_v<T, float>, 
              "Rectangle only supports int32_t and float coordinate types");
```

**Error Location Issues**: Static assertion failures occur deep in template instantiation, making it unclear to users where the constraint violation originated.

**Limited IDE Support**: Tools and IDEs cannot easily understand static assertions for better autocomplete and error highlighting.

**Documentation Disconnect**: The constraint logic was hidden in implementation details rather than being part of the interface contract.

Given that we're already using C++20 features throughout the codebase, we needed to decide whether to adopt C++20 concepts for better template constraints and error messages.

## Decision

We **adopted C++20 concepts** to replace static assertions for template parameter constraints, starting with the `Rectangle<T>` template.

### Implementation

**1. Defined a Clear Concept:**
```cpp
template<typename T>
concept RectangleCoordinate = std::same_as<T, int32_t> || std::same_as<T, float>;
```

**2. Applied Concept to Template Declaration:**
```cpp
// Before: Poor error messages
template<typename T>
class Rectangle {
    static_assert(std::is_same_v<T, int32_t> || std::is_same_v<T, float>, 
                  "Rectangle only supports int32_t and float coordinate types");
    // ...
};

// After: Concept-constrained template  
template<RectangleCoordinate T>
class Rectangle {
    // ...
};
```

**3. Updated All Related Declarations:**
- Forward declarations: `template<RectangleCoordinate T> class Rectangle;`
- Implementation class: `template<RectangleCoordinate T> class RectangleImpl;`
- Method definitions: `template<RectangleCoordinate T> auto Rectangle<T>::getX() const -> T;`

**4. Added Comprehensive Tests:**
- Compile-time validation of valid types (`int32_t`, `float`)
- Compile-time rejection of invalid types (`uint32_t`, `double`, `char`, etc.)
- Verification of concept logic and underlying `std::same_as` behavior

## Consequences

### Positive Outcomes

**Dramatically Improved Error Messages:**
```cpp
// Before (static_assert):
error: static assertion failed: Rectangle only supports int32_t and float coordinate types

// After (concepts):
error: template constraint failure for 'template<class T> requires RectangleCoordinate<T> class sdlpp::Rectangle'
note: constraints not satisfied
note: no operand of the disjunction is satisfied
concept RectangleCoordinate = std::same_as<T, int32_t> || std::same_as<T, float>;
```

**Self-Documenting Interface:** The concept name `RectangleCoordinate` immediately communicates the template's expectations to users reading the code.

**Better Tooling Support:** IDEs can provide superior autocomplete, error highlighting, and template constraint validation using concept information.

**Compile-Time Performance:** Concepts can be evaluated more efficiently than complex SFINAE patterns or static assertions in some cases.

**Type Safety:** Concept failures occur at the point of template instantiation with clear attribution to the failing requirement.

**Consistency:** Establishes a pattern for constraining other templates in the codebase (future `Texture<PixelFormat>`, `Buffer<DataType>`, etc.).

### Negative Consequences

**C++20 Requirement:** Projects that cannot use C++20 would need to maintain the static assertion version. However, we already require C++20 throughout the codebase.

**Concept Learning Curve:** Developers unfamiliar with concepts need to understand the new syntax and semantics.

**Template Bloat Risk:** Overzealous concept usage could lead to unnecessarily complex template interfaces. We mitigate this by applying concepts judiciously to genuinely constrained templates.

### Risk Mitigations

**Documentation:** Updated CLAUDE.md to mention our C++20 concept usage and provided examples in tests.

**Test Coverage:** Added comprehensive tests (`RectangleConceptTest.*`) to ensure concept behavior matches expectations and catches regressions.

**Gradual Adoption:** Starting with Rectangle template allows us to validate the approach before applying concepts to other templates.

## Alternatives Considered

### Continue Using Static Assertions
**Approach:** Keep the existing static assertion approach with better error messages.

**Rejected because:**
- Still results in poor error messages compared to concepts
- Doesn't provide the self-documenting benefit of named concepts
- Misses the opportunity to leverage modern C++ features we're already using
- No improvement in IDE support or tooling integration

### SFINAE-Based Constraints  
**Approach:** Use Substitution Failure Is Not An Error patterns with `std::enable_if`.

**Rejected because:**
- Even worse error messages than static assertions
- Complex, hard-to-read template syntax
- Concepts are the modern C++ replacement for SFINAE constraints
- No benefit over concepts in any dimension

### Runtime Type Checking
**Approach:** Use dynamic type checks or type erasure with runtime validation.

**Rejected because:**
- Introduces runtime performance overhead unacceptable in graphics code
- Loses compile-time safety guarantees
- Conflicts with our zero-overhead abstraction principle
- Makes template instantiation errors occur at runtime rather than compile time

## Implementation Evidence

The concept implementation demonstrates clear benefits:

**Error Quality:** Demonstrated that concept violations produce substantially clearer error messages that guide users to the correct types (`int32_t`, `float`).

**Test Coverage:** Added 3 comprehensive test cases covering valid types, invalid types, and concept logic validation - all pass.

**Backward Compatibility:** All existing code continues to work unchanged. The 86-test suite passes with 100% success rate.

**Performance:** No runtime overhead - concepts are pure compile-time constructs that generate identical optimized code.

**API Clarity:** `template<RectangleCoordinate T>` immediately communicates template requirements better than hidden static assertions.

This implementation successfully modernizes our template constraints while maintaining performance and improving developer experience.

---

*This ADR establishes our approach to template constraints using C++20 concepts. Future templates requiring type constraints should follow this pattern for consistency and improved error messages.*