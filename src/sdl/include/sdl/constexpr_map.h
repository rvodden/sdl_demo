#ifndef SDL_CONSTEXPR_MAP_H
#define SDL_CONSTEXPR_MAP_H

#include <algorithm>
#include <array>
#include <iterator>
#include <stdexcept>
#include <tuple>

namespace sdl {

/**
 * @brief A compile-time key-value mapping container with O(n) lookup.
 * 
 * This class template provides a constexpr-compatible map-like container that can be
 * used at compile time for static lookups. The container is backed by a std::array
 * and uses linear search for key lookups.
 * 
 * Key features:
 * - Compile-time construction and lookup
 * - Automatic size deduction via CTAD (Class Template Argument Deduction)
 * - Exception-based error handling for missing keys
 * - Support for any key/value types that are equality comparable
 * 
 * @tparam Key The type of keys stored in the map
 * @tparam Value The type of values stored in the map  
 * @tparam Size The number of key-value pairs in the map
 * 
 * @note This implementation uses linear search, so lookup time is O(n). For large
 *       maps or performance-critical code, consider using a different container.
 * 
 * Example usage:
 * @code
 * // Method 1: Using automatic size deduction (recommended)
 * static constexpr auto colorMap = sdl::Map(std::array{
 *     std::pair{"red", 0xFF0000},
 *     std::pair{"green", 0x00FF00}, 
 *     std::pair{"blue", 0x0000FF}
 * });
 * 
 * // Method 2: Using makeConstexprMap helper
 * static constexpr auto data = std::array{
 *     std::pair{1, "one"},
 *     std::pair{2, "two"}
 * };
 * static constexpr auto numberMap = sdl::makeConstexprMap(data);
 * 
 * // Method 3: Manual size specification (legacy)
 * static constexpr sdl::Map<int, const char*, 2> legacyMap{{
 *     {1, "one"},
 *     {2, "two"}
 * }};
 * 
 * // Usage
 * auto color = colorMap["red"];  // Returns 0xFF0000
 * @endcode
 */

template <class Key, class Value, std::size_t Size>
struct Map {
  /**
   * @brief Constructs a Map from an initializer list.
   * 
   * @param initList An initializer list of key-value pairs
   * @pre The initializer list must contain exactly Size elements
   * @warning If initList contains fewer than Size elements, behavior is undefined
   */
  constexpr Map(std::initializer_list<std::pair<Key, Value>> initList) {
    auto it = initList.begin();
    for (std::size_t i = 0; i < Size; ++i) {
      _data.at(i) = *std::next(it, static_cast<int32_t>(i));
    }
  };
  
  /**
   * @brief Constructs a Map from a const reference to std::array.
   * 
   * @param data The array of key-value pairs to copy from
   */
  constexpr Map(const std::array<std::pair<Key, Value>, Size>& data)
      : _data(data) {}
      
  /**
   * @brief Constructs a Map from an rvalue reference to std::array.
   * 
   * @param data The array of key-value pairs to move from
   */
  constexpr Map(std::array<std::pair<Key, Value>, Size>&& data)
      : _data(std::move(data)) {}

  /**
   * @brief Retrieves the value associated with the given key.
   * 
   * Performs a linear search through the underlying array to find the key.
   * 
   * @param key The key to search for
   * @return The value associated with the key
   * @throws std::range_error if the key is not found
   * @note This operation has O(n) time complexity
   */
  constexpr auto operator[](const Key& key) const -> Value;

 private:
  std::array<std::pair<Key, Value>, Size> _data;
};

/**
 * @brief Factory function for creating a Map with automatic size deduction.
 * 
 * This function template allows creating a Map without explicitly specifying
 * the Size template parameter. The size is automatically deduced from the
 * number of pairs provided.
 * 
 * @tparam Key The type of keys in the map
 * @tparam Value The type of values in the map  
 * @tparam Pairs Parameter pack of pair types
 * @param pairs The key-value pairs to store in the map
 * @return A Map containing the provided key-value pairs
 * 
 * @deprecated This function is deprecated. Use CTAD with std::array instead:
 * @code
 * // Preferred approach:
 * static constexpr auto myMap = sdl::Map(std::array{
 *     std::pair{key1, value1},
 *     std::pair{key2, value2}
 * });
 * @endcode
 */
template <class Key, class Value, class... Pairs>
constexpr auto makeMap(Pairs&&... pairs) -> Map<Key, Value, sizeof...(pairs)> {
  return Map<Key, Value, sizeof...(pairs)>(
      std::array<std::pair<Key, Value>, sizeof...(pairs)>{
          {std::forward<Pairs>(pairs)...}});
}

template <class Key, class Value, std::size_t Size>
constexpr auto Map<Key, Value, Size>::operator[](const Key& key) const
    -> Value {
  const auto& iterator = std::find_if(
      _data.cbegin(), _data.cend(),
      [&key](const auto& value) -> bool { return key == value.first; });
  if (iterator == _data.cend()) {
    throw std::range_error("Not Found.");
  }
  return iterator->second;
}

/**
 * @brief Class Template Argument Deduction (CTAD) guide for const std::array.
 * 
 * This deduction guide allows automatic template parameter deduction when
 * constructing a Map from a const std::array reference.
 * 
 * @tparam Key The key type deduced from the array
 * @tparam Value The value type deduced from the array
 * @tparam N The size deduced from the array
 */
template <class Key, class Value, std::size_t N>
Map(const std::array<std::pair<Key, Value>, N>&) -> Map<Key, Value, N>;

/**
 * @brief Class Template Argument Deduction (CTAD) guide for rvalue std::array.
 * 
 * This deduction guide allows automatic template parameter deduction when
 * constructing a Map from an rvalue std::array reference.
 * 
 * @tparam Key The key type deduced from the array
 * @tparam Value The value type deduced from the array
 * @tparam N The size deduced from the array
 */
template <class Key, class Value, std::size_t N>
Map(std::array<std::pair<Key, Value>, N>&&) -> Map<Key, Value, N>;

/**
 * @brief Factory function for creating a Map from a std::array.
 * 
 * This function provides a convenient way to create a Map from an existing
 * std::array of key-value pairs. It's particularly useful when you already
 * have the data in array form.
 * 
 * @tparam Key The type of keys in the map
 * @tparam Value The type of values in the map
 * @tparam N The number of elements in the array
 * @param data The array of key-value pairs
 * @return A Map containing the data from the array
 * 
 * Example:
 * @code
 * static constexpr auto data = std::array{
 *     std::pair{1, "one"},
 *     std::pair{2, "two"}
 * };
 * static constexpr auto numberMap = sdl::makeConstexprMap(data);
 * @endcode
 */
template <class Key, class Value, std::size_t N>
constexpr auto makeConstexprMap(
    const std::array<std::pair<Key, Value>, N>& data) -> Map<Key, Value, N> {
  return Map<Key, Value, N>(data);
}

}  // namespace sdl

#endif  // SDL_CONSTEXPR_MAP_H