using namespace std;

#include "BoardGame.cpp"

#ifndef CONNECT4_H
#define CONNECT4_H

class Connect4 : public BoardGame {
  public:
  Connect4();
  Connect4(const Connect4 &copy);
  ~Connect4();
  vector<string> get_valid_moves();
  void make_move(string move);
  void retract_move(string move);
  bool winning_move();
  void reset();
  int get_prior_index(string move);


  private:
  bool is_valid_move(int move);
  int get_next_open_row(int column);
  const int EMPTY_PIECE = 0;
  const int PLAYER_1_PIECE = 1;
  const int PLAYER_2_PIECE = 2;
  const int PIECE_PLAYER[2] = {PLAYER_1_PIECE, PLAYER_2_PIECE};
};

#endif // CONNECT4_H
