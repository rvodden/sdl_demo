#ifndef TICTACTOE_GAME_CONSTANTS_H
#define TICTACTOE_GAME_CONSTANTS_H

#include <cstdint>

static const uint8_t kCellColumns = 3;
static const uint8_t kCellRows = 3;

enum class GameState : std::uint8_t { Playing,
                                 PlayerOWins,
                                 PlayerXWins,
                                 Draw };

#endif // TICTACTOE_GAME_CONSTANTS_H