#include "Game.h"
#include <iostream>

Connect4::Connect4()
{
  turn = new int;
  *turn = PLAYER_1;
  create_board();
  no_moves = new int;
  *no_moves = 0;
}

Connect4::~Connect4()
{
  delete turn;
  delete no_moves;
  for (int i = 0; i < ROWS; i++)
  {
    delete[] board[i];
  }
  delete[] board;
  cout << "Deconstructor called" << endl;
}

Connect4::Connect4(Connect4 &copy)
{
  turn = new int;
  *turn = copy.get_to_move();
  board = new int *[ROWS];
  int **copy_board = copy.get_board();
  for (int i = 0; i < ROWS; i++)
  {
    board[i] = new int[COLUMNS];
    for (int j = 0; j < COLUMNS; j++)
    {
      board[i][j] = copy_board[i][j];
    }
  }
  no_moves = new int;
  *no_moves = copy.get_move_no();
}

void Connect4::reset()
{
  for (int i = 0; i < ROWS; i++)
  {
    for (int j = 0; j < COLUMNS; j++)
    {
      board[i][j] = 0;
    }
  }
  *no_moves = 0;
  *turn = PLAYER_1;
}

void Connect4::create_board()
{
  board = new int *[ROWS];
  for (int i = 0; i < ROWS; i++)
  {
    board[i] = new int[COLUMNS];
    for (int j = 0; j < COLUMNS; j++)
    {
      board[i][j] = 0;
    }
  }
}

int Connect4::get_move_no()
{
  return *no_moves;
}

int **Connect4::get_board()
{
  return board;
}

int Connect4::get_to_move()
{
  return *turn;
}

int Connect4::get_to_move_opponent()
{
  return get_to_move() == PLAYER_1 ? PLAYER_2 : PLAYER_1;
}

int Connect4::get_current_piece()
{
  return PIECE_PLAYER[*turn];
}

int Connect4::get_current_opponent_piece()
{
  return PIECE_PLAYER[(*turn) == PLAYER_1 ? PLAYER_2 : PLAYER_1];
}

int Connect4::get_next_open_row(int column)
{
  for (int i = 0; i < ROWS; i++)
  {
    if (board[i][column] == 0)
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
  *no_moves += 1;
}

void Connect4::set_piece(int row, int column, int piece)
{
  board[row][column] = piece;
  *turn = get_to_move() == PLAYER_1 ? PLAYER_2 : PLAYER_1;
}

bool Connect4::is_valid_column(int column)
{
  return board[ROWS - 1][column] == 0;
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
  cout << string(COLUMNS + 2, '-') << endl;
  for (int i = ROWS - 1; i > -1; i--)
  {
    cout << "[";
    for (int j = 0; j < COLUMNS; j++)
    {
      cout << board[i][j];
    }
    cout << "]" << endl;
  }
  cout << string(COLUMNS + 2, '-') << endl;
}

bool Connect4::is_terminal_state()
{
  return winning_move() || get_valid_columns().size() == 0;
}
