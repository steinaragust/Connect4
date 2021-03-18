using namespace std;
#include <vector>
#include <array>
#include "printf.h"
#include "string"

#ifndef BOARDGAME_H
#define BOARDGAME_H

typedef int** Key;

struct GameInfo {
  int ROWS;
  int COLUMNS;
  int priors_arr_size;
  int nr_unique_pcs;
};

class BoardGame {
  public:
  GameInfo info;
  // Virtual functions that need to be implemented
  virtual vector<int> get_valid_moves() = 0;
  virtual void make_move(int move) = 0;
  virtual void retract_move(int move) = 0;
  virtual bool winning_move() = 0;
  virtual void reset() = 0;
  virtual int get_prior_index(int move) = 0;
  virtual bool is_terminal_state() = 0;
  virtual int get_move_no() = 0;
  virtual Key get_board() = 0;
  virtual int get_to_move() = 0;
  virtual int get_to_move_opponent() = 0;
  virtual void print_board() = 0;

  // Class constants
  const int PLAYER_1 = 0;
  const int PLAYER_2 = 1;

  protected:
  Key _board;
  int _turn;
  int _no_moves;
};

#endif // BOARDGAME_H
