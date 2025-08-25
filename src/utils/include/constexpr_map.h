#ifndef CONSTEXPR_MAP_H
#define CONSTEXPR_MAP_H

#include <algorithm>
#include <array>
#include <iterator>
#include <stdexcept>
#include <tuple>

namespace vodden {

//! @brief A simple map class capable of being used as a constexpr
template <class Key, class Value, std::size_t Size>
struct Map {
  constexpr Map(std::initializer_list<std::pair<Key, Value>> initList) {
    auto it = initList.begin();
    for (std::size_t i = 0; i < Size; ++i) {
      _data.at(i) = *std::next(it, static_cast<int32_t>(i));
    }
  };
  constexpr Map(const std::array<std::pair<Key, Value>, Size>& data)
      : _data(data) {}
  constexpr Map(std::array<std::pair<Key, Value>, Size>&& data)
      : _data(std::move(data)) {}

  constexpr auto operator[](const Key& key) const -> Value;

 private:
  std::array<std::pair<Key, Value>, Size> _data;
};

//! @brief Helper function to create a Map from initializer list with
//! automatic size deduction
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

// Deduction guides for automatic size deduction
template <class Key, class Value, std::size_t N>
Map(const std::array<std::pair<Key, Value>, N>&) -> Map<Key, Value, N>;

template <class Key, class Value, std::size_t N>
Map(std::array<std::pair<Key, Value>, N>&&) -> Map<Key, Value, N>;

//! @brief Convenient alias for creating maps with automatic size deduction
//! from arrays
template <class Key, class Value, std::size_t N>
constexpr auto makeConstexprMap(
    const std::array<std::pair<Key, Value>, N>& data) -> Map<Key, Value, N> {
  return Map<Key, Value, N>(data);
}

}  // namespace vodden

#endif
