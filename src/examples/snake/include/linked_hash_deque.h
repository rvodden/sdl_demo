#include <list>
#include <unordered_set>

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
      _set.insert(value);
    }
    
    void pushBack(const T& value) {
      _list.push_back(value);
      _set.insert(value);
    }

    void popBack() {
      auto it = _list.end();
      --it;
      _set.erase(*it);
      _list.pop_back();
    }

    auto contains(const T& value) const -> bool {
      return _set.contains(value);
    }

    // Range-based for loop support - iterate over the list to maintain order
    auto begin() -> typename std::list<T>::iterator {
      return _list.begin();
    }

    auto end() -> typename std::list<T>::iterator {
      return _list.end();
    }

    auto begin() const -> typename std::list<T>::const_iterator {
      return _list.begin();
    }

    auto end() const -> typename std::list<T>::const_iterator {
      return _list.end();
    }

    [[nodiscard]] auto head() const -> const T& { return _list.front(); }
    [[nodiscard]] auto tail()  const -> const T&{ return _list.back();  }
    [[nodiscard]] auto empty() const -> bool { return _list.empty(); }
    [[nodiscard]] auto size() const -> std::size_t { return _list.size(); }
   
    void clear() {
      _list.clear();
      _set.clear();
    }
  private: 
    std::list<T> _list;
    std::unordered_set<T, Hash, Eq> _set;
};
