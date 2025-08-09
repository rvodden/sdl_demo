#include <functional>
#include <memory>

#include <iostream>
#include <vector>

class Base {
  public:
    virtual ~Base() {};
    virtual void print() const { std::cout << "Base!" << std::endl; };
};

class Sub : public Base {
  public:
    virtual void print() const { std::cout << "Sub!" << std::endl; };
};

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
  std::vector<std::reference_wrapper<Base>> bases {};

  {
    Base& base = std::ref(*(new Base()));
    Sub& sub = std::ref(*(new Sub()));
    bases.push_back(base);
    bases.push_back(sub);
  }

  for ( const Base& base : bases ) {
    base.print();
    delete &base;
  }

  return 0;
}
