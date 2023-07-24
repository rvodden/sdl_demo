#include "sprite_impl.h"

namespace sdl::tools {

Sprite::Sprite( const Texture& texture, const Rectangle& rectangle ): _spriteImpl { std::make_unique<SpriteImpl>( texture, rectangle ) } { }

Sprite::Sprite( Sprite&& other ): _spriteImpl { std::move( other._spriteImpl ) } { }

Sprite::~Sprite() {};

}
