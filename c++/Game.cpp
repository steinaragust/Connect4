#include "Game.h"

Connect4::Connect4()
{
  reset();
}

Connect4::~Connect4()
{
}

Connect4::Connect4(const Connect4 &copy)
{
  turn = copy.turn;
  board = copy.board;
  no_moves = copy.no_moves;
}

void Connect4::reset()
{
  for (int i = 0; i < ROWS; i++)
  {
    for (int j = 0; j < COLUMNS; j++)
    {
      board[i][j] = EMPTY_PIECE;
    }
  }
  no_moves = 0;
  turn = PLAYER_1;
}

int Connect4::get_move_no()
{
  return no_moves;
}

array<array<int, 7>, 6> Connect4::get_board()
{
  return board;
}

int Connect4::get_to_move()
{
  return turn;
}

int Connect4::get_to_move_opponent()
{
  return get_to_move() == PLAYER_1 ? PLAYER_2 : PLAYER_1;
}

int Connect4::get_current_piece()
{
  return PIECE_PLAYER[turn];
}

int Connect4::get_current_opponent_piece()
{
  return PIECE_PLAYER[turn == PLAYER_1 ? PLAYER_2 : PLAYER_1];
}

int Connect4::get_next_open_row(int column)
{
  for (int i = 0; i < ROWS; i++)
  {
    if (board[i][column] == EMPTY_PIECE)
    {
      return i;
    }
  }
  return -1;
}

vector<int> Connect4::get_valid_columns()
{
  vector<int> valid_locations;

  for (int i = 0; i < COLUMNS; i++)
  {
    if (is_valid_column(i))
    {
      valid_locations.push_back(i);
    }
  }

  return valid_locations;
}

void Connect4::drop_piece_in_column(int column)
{
  if (column < 0 || column > COLUMNS - 1)
  {
    throw "Invalid column, choose a column between 0 and 1";
  }
  int row = get_next_open_row(column);
  if (row == -1)
  {
    throw "Column full";
  }
  int piece = get_current_piece();
  set_piece(row, column, piece);
  no_moves += 1;
}

void Connect4::retract_piece_in_column(int column)
{
  int row = get_next_open_row(column);
  set_piece((row == -1 ? ROWS : row) - 1, column, EMPTY_PIECE);
  no_moves -= 1;
}

void Connect4::set_piece(int row, int column, int piece)
{
  board[row][column] = piece;
  turn = get_to_move() == PLAYER_1 ? PLAYER_2 : PLAYER_1;
}

bool Connect4::is_valid_column(int column)
{
  return board[ROWS - 1][column] == EMPTY_PIECE;
}

bool Connect4::winning_move()
{
  for (int piece : PIECE_PLAYER)
  {
    // Check horizontal locations for win
    for (int c = 0; c < COLUMNS - 3; c++)
    {
      for (int r = 0; r < ROWS; r++)
      {
        if (
            board[r][c] == piece && board[r][c + 1] == piece && board[r][c + 2] == piece && board[r][c + 3] == piece)
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
            board[r][c] == piece && board[r + 1][c] == piece && board[r + 2][c] == piece && board[r + 3][c] == piece)
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
            board[r][c] == piece && board[r + 1][c + 1] == piece && board[r + 2][c + 2] == piece && board[r + 3][c + 3] == piece)
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
            board[r][c] == piece && board[r - 1][c + 1] == piece && board[r - 2][c + 2] == piece && board[r - 3][c + 3] == piece)
        {
          return true;
        }
      }
    }
  }

  return false;
}

void Connect4::print_board()
{
  printf("%s", string(COLUMNS + 2, '-').c_str());
  printf("\n");
  for (int i = ROWS - 1; i > -1; i--)
  {
    printf("[");
    for (int j = 0; j < COLUMNS; j++)
    {
      printf("%d", board[i][j]);
    }
    printf("]\n");
  }
  printf("%s", string(COLUMNS + 2, '-').c_str());
  printf("\n");
}

bool Connect4::is_terminal_state()
{
  return winning_move() || get_valid_columns().size() == 0;
}
