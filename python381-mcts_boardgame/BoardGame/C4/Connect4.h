using namespace std;

#include "../BoardGame.h"

#ifndef CONNECT4_H
#define CONNECT4_H

class Connect4 : public BoardGame {
  public:
  Connect4();
  Connect4(const Connect4 &copy);
  ~Connect4();
  void set_copy(BoardGame* copy);
  BoardGame* get_copy();
  vector<int> get_valid_moves();
  void make_move(int move);
  void retract_move(int move);
  bool winning_move();
  void reset();
  int get_prior_index(int move);
  bool is_terminal_state();
  int get_move_no();
  Key get_board();
  int get_to_move();
  int get_to_move_opponent();
  void print_board();

  private:
  bool is_valid_move(int move);
  int get_next_open_row(int column);
  const int EMPTY_PIECE = 0;
  const int PLAYER_1_PIECE = 1;
  const int PLAYER_2_PIECE = 2;
  const int PIECE_PLAYER[2] = {PLAYER_1_PIECE, PLAYER_2_PIECE};
};

#endif // CONNECT4_H
