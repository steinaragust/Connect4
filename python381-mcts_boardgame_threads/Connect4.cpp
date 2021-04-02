#include "Connect4.h"

inline Connect4::Connect4() {
  info = { 6, 7, 7, 2 };
  _board = new int*[info.ROWS];
  for (int i = 0; i < info.ROWS; i++) _board[i] = new int[info.COLUMNS];
  reset();
}

inline Connect4::~Connect4() {
  for (int i = 0; i < info.ROWS; i++) delete[] _board[i];
  delete[] _board;
}

inline Connect4::Connect4(const Connect4 &copy) {
  _turn = copy._turn;
  _board = copy._board;
  _no_moves = copy._no_moves;
}

inline vector<int> Connect4::get_valid_moves() {
  vector<int> valid_locations;

  for (int i = 0; i < info.COLUMNS; i++) {
    if (is_valid_move(i)) {
      valid_locations.push_back(i);
    }
  }

  return valid_locations;
}

inline void Connect4::make_move(int move) {
  if (move < 0 || move > info.COLUMNS - 1)
  {
    throw "Invalid column, choose a column between 0 and 1";
  }
  int row = get_next_open_row(move);
  if (row == -1)
  {
    throw "Column full";
  }
  int piece = PIECE_PLAYER[_turn];
  _board[row][move] = piece;
  _turn = get_to_move() == PLAYER_1 ? PLAYER_2 : PLAYER_1;
  _no_moves += 1;
}

inline void Connect4::retract_move(int move) {
  int row = get_next_open_row(move);
  row = (row == -1 ? info.ROWS : row) - 1;
  _board[row][move] = EMPTY_PIECE;
  _turn = get_to_move() == PLAYER_1 ? PLAYER_2 : PLAYER_1;
  _no_moves -= 1;
}

inline bool Connect4::winning_move()
{
  for (int piece : PIECE_PLAYER)
  {
    // Check horizontal locations for win
    for (int c = 0; c < info.COLUMNS - 3; c++)
    {
      for (int r = 0; r < info.ROWS; r++)
      {
        if (
            _board[r][c] == piece && _board[r][c + 1] == piece && _board[r][c + 2] == piece && _board[r][c + 3] == piece)
        {
          return true;
        }
      }
    }

    // Check vertical locations for win
    for (int c = 0; c < info.COLUMNS; c++)
    {
      for (int r = 0; r < info.ROWS - 3; r++)
      {
        if (
            _board[r][c] == piece && _board[r + 1][c] == piece && _board[r + 2][c] == piece && _board[r + 3][c] == piece)
        {
          return true;
        }
      }
    }

    // Check positively sloped diagonals
    for (int c = 0; c < info.COLUMNS - 3; c++)
    {
      for (int r = 0; r < info.ROWS - 3; r++)
      {
        if (
            _board[r][c] == piece && _board[r + 1][c + 1] == piece && _board[r + 2][c + 2] == piece && _board[r + 3][c + 3] == piece)
        {
          return true;
        }
      }
    }

    // Check negatively sloped diagonals
    for (int c = 0; c < info.COLUMNS - 3; c++)
    {
      for (int r = 3; r < info.ROWS; r++)
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
  for (int i = 0; i < info.ROWS; i++)
  {
    for (int j = 0; j < info.COLUMNS; j++)
    {
      _board[i][j] = EMPTY_PIECE;
    }
  }
  _no_moves = 0;
  _turn = PLAYER_1;
}

inline int Connect4::get_prior_index(int move) {
  return move;
}

inline int Connect4::get_next_open_row(int column)
{
  for (int i = 0; i < info.ROWS; i++)
  {
    if (_board[i][column] == EMPTY_PIECE)
    {
      return i;
    }
  }
  return -1;
}

inline bool Connect4::is_valid_move(int move) {
  return _board[info.ROWS - 1][move] == EMPTY_PIECE;
}

inline bool Connect4::is_terminal_state() {
  return get_valid_moves().size() == 0 || winning_move();
}

inline int Connect4::get_move_no() {
  return _no_moves;
}

inline Key Connect4::get_board() {
  return _board;
}

inline int Connect4::get_to_move() {
  return _turn;
}

inline int Connect4::get_to_move_opponent() {
  return get_to_move() == PLAYER_1 ? PLAYER_2 : PLAYER_1;
}

inline void Connect4::print_board() {
  printf("%s", string(info.COLUMNS + 2, '-').c_str());
  printf("\n");
  for (int i = info.ROWS - 1; i > -1; i--) {
    printf("[");
    for (int j = 0; j < info.COLUMNS; j++) {
      printf("%d", _board[i][j]);
    }
    printf("]\n");
  }
  printf("%s", string(info.COLUMNS + 2, '-').c_str());
  printf("\n");
}
