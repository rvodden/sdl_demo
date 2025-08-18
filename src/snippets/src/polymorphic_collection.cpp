#include <functional>
#include <iostream>
#include <memory>
#include <vector>

class Base {
 public:
  Base() = default;
  Base(const Base& other) = default;
  Base(Base&& other) noexcept = default;

  auto operator=(const Base& other) -> Base& = default;
  auto operator=(Base&& other) noexcept -> Base& = default;

  virtual ~Base() = default;
  virtual void print() const { std::cout << "Base!\n"; };
};

class Sub : public Base {
 public:
  void print() const override { std::cout << "Sub!\n"; };
};

auto main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) -> int {
  std::vector<std::unique_ptr<Base>> bases{};

  {
    auto base = std::make_unique<Base>();
    auto sub = std::make_unique<Sub>();
    bases.emplace_back(std::move(base));
    bases.emplace_back(std::move(sub));
  }

  for (const auto& base : bases) {
    base->print();
  }

  return 0;
}
