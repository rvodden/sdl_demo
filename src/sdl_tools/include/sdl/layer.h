#ifndef LAYER_H
#define LAYER_H

#include <functional>
#include <memory>

#include <sdl/sdl.h>

namespace sdl::tools {

// template<class State>
// class Layer {
//   Layer(
//     std::function<void> update,
//     std::function<void(const State&)> render
//   ) : _update(update), _render(render) {};

//   private:
//     std::function<State(State)> _update;
//     std::function<void(const State&)> _render;
// };

}

#endif // LAYER_H
