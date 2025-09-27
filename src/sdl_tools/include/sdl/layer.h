#ifndef LAYER_H
#define LAYER_H

#include <functional>
#include <memory>

#include <sdl/sdl.h>

namespace sdl::tools {

template<class State>
class Layer {
  Layer(
    std::function<State, State, BaseLayer> iterate,
    std::function<void, const State&> render,
    std::shared_ptr<Renderer> renderer
  ) : _iterate(iterate), _render(render), _renderer(renderer) {}

  private:
    std::function<State, State, BaseLayer> _iterate;
    std::function<void, const State&> _render;

    std::shared_ptr<Renderer> _renderer;
};

}

#endif // LAYER_H
