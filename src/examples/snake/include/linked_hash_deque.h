#include <list>
#include <unordered_map>

template<typename T, typename Hash = std::hash<T>, typename Eq = std::equal_to<T>>
class LinkedHashDeque {
  public:
    LinkedHashDeque() = default;

    LinkedHashDeque(std::initializer_list<T> init) {
        for (const auto& val : init) {
            pushBack(val);
        }
    }

    void pushFront(const T& value) {
      _list.push_front(value);
      _map[value] = _list.begin();
    }
    
    void pushBack(const T& value) {
      _list.push_back(value);
      _map[value] = _list.begin();
    }

    void popBack() {
      auto it = _list.end();
      --it;
      _map.erase(*it);
      _list.pop_back();
    }

    auto contains(const T& value) const -> bool {
      return _map.find(value) != _map.end();
    }

    [[nodiscard]] auto front() const -> const T& { return _list.front(); }
    [[nodiscard]] auto back()  const -> const T&{ return _list.back();  }
    [[nodiscard]] auto empty() const -> bool { return _list.empty(); }
    [[nodiscard]] auto size() const -> std::size_t { return _list.size(); }
    
  private: 
    std::list<T> _list;
    std::unordered_map<T, typename std::list<T>::iterator, Hash, Eq> _map;
};
