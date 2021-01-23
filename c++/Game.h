using namespace std;
#include <iostream>

#ifndef CONNECT4_H
#define CONNECT4_H
#include <vector>

class Connect4
{
public:
  // Constructor/Deconstructoor
  Connect4();
  ~Connect4();
  Connect4(Connect4 &copy);

  // Get methods
  int get_move_no();
  int **get_board();
  int get_to_move();
  int get_to_move_opponent();
  int get_current_piece();
  int get_current_opponent_piece();
  bool is_valid_column(int column);
  vector<int> get_valid_columns();

  // Game methods
  void reset();
  void drop_piece_in_column(int column);
  void retract_piece_in_column(int column);
  bool winning_move();
  bool is_terminal_state();
  void print_board();

  // Class variables
  int **board;
  int *turn;
  int *no_moves;

  // Class constants
  const int EMPTY = 1;
  const int PLAYER_1 = 0;
  const int PLAYER_2 = 1;
  const int PLAYER_1_PIECE = 1;
  const int PLAYER_2_PIECE = 2;
  const int PIECE_PLAYER[2] = {PLAYER_1_PIECE, PLAYER_2_PIECE};
  const int ROWS = 6;
  const int COLUMNS = 7;
  const int WINDOW = 4;

  // Call python functions
  function<void(int **)> cpp_f;
  void set_f(function<void(int **)> f)
  {
    cpp_f = f;
  }
  void call_f()
  {
    int **arr = new int *[ROWS];
    for (int i = 0; i < ROWS; i++)
    {
      arr[i] = new int[COLUMNS];
      for (int j = 0; j < COLUMNS; j++)
      {
        arr[i][j] = 0;
      }
    }

    cpp_f(arr);
    cout << string(COLUMNS + 2, '-') << endl;
    for (int i = ROWS - 1; i > -1; i--)
    {
      cout << "[";
      for (int j = 0; j < COLUMNS; j++)
      {
        cout << arr[i][j];
      }
      cout << "]" << endl;
    }
    cout << string(COLUMNS + 2, '-') << endl;
  }

private:
  void create_board();
  int get_next_open_row(int column);
  void set_piece(int row, int column, int piece);
};

#endif // CONNECT4_H
