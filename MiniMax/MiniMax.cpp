using namespace std;

#include <array>
#include <vector>
#include <limits>
#include <random>

#include "../Connect4-Game/Game.h"

template <class X, class Y>
X CopyArray(const Y &src, const size_t start_size, const size_t end_size)
{
  X dst;
  std::copy(src.begin() + start_size, src.begin() + end_size, dst.begin());
  return dst;
}

int evaluate_window(array<int, 4> window, Connect4 game)
{
  int score = 0;
  int piece = game.get_current_piece();
  int opp_piece = game.get_current_opponent_piece();

  int player_pieces = 0;
  int opp_pieces = 0;
  int empty_pieces = 0;

  for (int i = 0; i < game.WINDOW; i++)
  {
    player_pieces += window[i] == piece ? 1 : 0;
    opp_pieces += window[i] == opp_piece ? 1 : 0;
    empty_pieces += window[i] == game.EMPTY_PIECE ? 1 : 0;
  }

  if (player_pieces == game.WINDOW)
  {
    score += 100;
  }
  else if (player_pieces == 3 && empty_pieces == 1)
  {
    score += 5;
  }
  else if (player_pieces == 2 && empty_pieces == 2)
  {
    score += 2;
  }

  if (opp_pieces == 3 && empty_pieces == 1)
  {
    score -= 4;
  }

  return score;
}

int score_position(Connect4 game)
{
  int score = 0;
  array<array<int, 7>, 6> board = game.get_board();
  int piece = game.get_current_piece();

  int center_count = 0;
  for (int i = 0; i < game.ROWS; i++)
  {
    center_count += board[i][(int)(game.COLUMNS / 2)] == piece ? 1 : 0;
  }
  score += center_count *= 3;

  // Score Horizontal
  for (int i = 0; i < game.ROWS; i++)
  {
    array<int, 7> row_array = board[i];
    for (int j = 0; j < game.COLUMNS - 3; j++)
    {
      array<int, 4> window = CopyArray<decltype(window)>(row_array, j, j + game.WINDOW);
      score += evaluate_window(window, game);
    }
  }

  // Score Vertical
  for (int i = 0; i < game.COLUMNS; i++)
  {
    array<int, 6> row_array;
    for (int j = 0; j < game.ROWS; j++)
    {
      row_array[j] = board[j][i];
    }
    for (int j = 0; j < game.ROWS - 3; j++)
    {
      array<int, 4> window = CopyArray<decltype(window)>(row_array, j, j + game.WINDOW);
      score += evaluate_window(window, game);
    }
  }

  // Score positive sloped diagonal
  for (int i = 0; i < game.ROWS - 3; i++)
  {
    for (int j = 0; j < game.COLUMNS - 3; j++)
    {
      array<int, 4> window;
      for (int k = 0; k < game.WINDOW; k++)
      {
        window[k] = board[i + k][j + k];
      }
      score += evaluate_window(window, game);
    }
  }

  for (int i = 0; i < game.ROWS - 3; i++)
  {
    for (int j = 0; j < game.COLUMNS - 3; j++)
    {
      array<int, 4> window;
      for (int k = 0; k < game.WINDOW; k++)
      {
        window[k] = board[i + 3 - k][j + k];
      }
      score += evaluate_window(window, game);
    }
  }

  return score;
}

array<int, 2> mini_max(Connect4 game, int depth, int alpha, int beta, bool maximizingPlayer)
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
        score = numeric_limits<int>::max();
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
      game.drop_piece_in_column(col);
      int new_score = mini_max(game, depth - 1, alpha, beta, false)[1];
      game.retract_piece_in_column(col);
      if (new_score > value)
      {
        value = new_score;
        column = col;
      }
      alpha = max(alpha, value);
      if (alpha >= beta)
      {
        break;
      }
    }
    return {column, value};
  }
  else
  {
    int value = numeric_limits<int>::max();
    int random = rand() % valid_locations.size();
    int column = valid_locations[random];
    for (int col : valid_locations)
    {
      game.drop_piece_in_column(col);
      int new_score = mini_max(game, depth - 1, alpha, beta, true)[1];
      if (new_score < value)
      {
        value = new_score;
        column = col;
      }
      beta = min(beta, value);
      if (alpha >= beta)
      {
        break;
      }
    }
    return {column, value};
  }
}
