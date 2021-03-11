#include "Connect4.h"

inline Connect4::Connect4() {
  ROWS = 6;
  COLUMNS = 7;
  priors_arr_size = 7;
  _board = new int*[ROWS];
  for (int i = 0; i < ROWS; i++) _board[i] = new int[COLUMNS];
  reset();
}

inline Connect4::~Connect4() {
  for (int i = 0; i < ROWS; i++) delete[] _board[i];
  delete[] _board;
}

inline Connect4::Connect4(const Connect4 &copy) {
  _turn = copy._turn;
  _board = copy._board;
  _no_moves = copy._no_moves;
}

inline vector<string> Connect4::get_valid_moves() {
  vector<string> valid_locations;

  for (int i = 0; i < COLUMNS; i++) {
    if (is_valid_move(i)) {
      valid_locations.push_back(to_string(i));
    }
  }

  return valid_locations;
}

inline void Connect4::make_move(string move) {
  int column = stoi(move);

  if (column < 0 || column > COLUMNS - 1)
  {
    throw "Invalid column, choose a column between 0 and 1";
  }
  int row = get_next_open_row(column);
  if (row == -1)
  {
    throw "Column full";
  }
  int piece = PIECE_PLAYER[_turn];
  _board[row][column] = piece;
  _turn = get_to_move() == PLAYER_1 ? PLAYER_2 : PLAYER_1;
  _no_moves += 1;
}

inline void Connect4::retract_move(string move) {
  int column = stoi(move);
  int row = get_next_open_row(column);
  row = (row == -1 ? ROWS : row) - 1;
  _board[row][column] = EMPTY_PIECE;
  _turn = get_to_move() == PLAYER_1 ? PLAYER_2 : PLAYER_1;
  _no_moves -= 1;
}

inline bool Connect4::winning_move()
{
  for (int piece : PIECE_PLAYER)
  {
    // Check horizontal locations for win
    for (int c = 0; c < COLUMNS - 3; c++)
    {
      for (int r = 0; r < ROWS; r++)
      {
        if (
            _board[r][c] == piece && _board[r][c + 1] == piece && _board[r][c + 2] == piece && _board[r][c + 3] == piece)
        {
          return true;
        }
      }
    }

    // Check vertical locations for win
    for (int c = 0; c < COLUMNS; c++)
    {
      for (int r = 0; r < ROWS - 3; r++)
      {
        if (
            _board[r][c] == piece && _board[r + 1][c] == piece && _board[r + 2][c] == piece && _board[r + 3][c] == piece)
        {
          return true;
        }
      }
    }

    // Check positively sloped diagonals
    for (int c = 0; c < COLUMNS - 3; c++)
    {
      for (int r = 0; r < ROWS - 3; r++)
      {
        if (
            _board[r][c] == piece && _board[r + 1][c + 1] == piece && _board[r + 2][c + 2] == piece && _board[r + 3][c + 3] == piece)
        {
          return true;
        }
      }
    }

    // Check negatively sloped diagonals
    for (int c = 0; c < COLUMNS - 3; c++)
    {
      for (int r = 3; r < ROWS; r++)
      {
        if (
            _board[r][c] == piece && _board[r - 1][c + 1] == piece && _board[r - 2][c + 2] == piece && _board[r - 3][c + 3] == piece)
        {
          return true;
        }
      }
    }
  }

  return false;
}

inline void Connect4::reset() {
  for (int i = 0; i < ROWS; i++)
  {
    for (int j = 0; j < COLUMNS; j++)
    {
      _board[i][j] = EMPTY_PIECE;
    }
  }
  _no_moves = 0;
  _turn = PLAYER_1;
}

inline int Connect4::get_prior_index(string move) {
  return stoi(move);
}

inline int Connect4::get_next_open_row(int column)
{
  for (int i = 0; i < ROWS; i++)
  {
    if (_board[i][column] == EMPTY_PIECE)
    {
      return i;
    }
  }
  return -1;
}

inline bool Connect4::is_valid_move(int move) {
  return _board[ROWS - 1][move] == EMPTY_PIECE;
}
