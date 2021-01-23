using namespace std;

#include <array>
#include <vector>
#include <limits>
#include <random>
#include "Game.h"

array<int, 2> mini_max(Connect4 &game, int depth, int alpha, int beta, bool maximizingPlayer)
{
  vector<int> valid_locations = game.get_valid_columns();
  bool is_terminal = game.is_terminal_state();
  if (depth == 0 || is_terminal)
  {
    int score = 0;
    if (is_terminal)
    {
      if (game.winning_move())
      {
        score = 100000000000000;
      }
      else
      {
        score = 0;
      }
    }
    else
    {
      score = score_position(game);
    }
    return {-1, score};
  }
  if (maximizingPlayer)
  {
    int value = numeric_limits<int>::min();
    int random = rand() % valid_locations.size();
    int column = valid_locations[random];
    for (int col : valid_locations)
    {
      Connect4 *game = Connect4(&game);
    }
  }
}
