#ifndef __CONSTEXPR_MAP_H__
#define __CONSTEXPR_MAP_H__

#include<algorithm>
#include<array>
#include<stdexcept>
#include<tuple>

namespace vodden {

//! @brief A simple map class capable of being used as a constexpr
template <class Key, class Value, std::size_t Size>
struct Map {
    constexpr Map(std::initializer_list<std::pair<Key, Value>> _initList) {
      std::copy_n(std::begin(_initList), Size, _data.begin());
    };
    constexpr Map(const std::array<std::pair<Key, Value>, Size>& data) : _data { data } {};
    constexpr Map(std::array<std::pair<Key, Value>, Size>&& data) : _data(std::move(data)) {};

    constexpr Value operator[](const Key &key) const;

    private:
      std::array<std::pair<Key, Value>, Size> _data;
};

template <class Key, class Value, std::size_t Size>
inline constexpr Value Map<Key, Value, Size>::operator[](const Key &key) const
{
  const auto& iterator = std::find_if(_data.cbegin(), _data.cend(), [&key](const auto& value){ return key == value.first; });
  if (iterator == _data.cend()) throw std::range_error("Not Found.");
  return iterator->second;
}

}

#endif
