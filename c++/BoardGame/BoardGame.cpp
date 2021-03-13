#include "BoardGame.h"

inline int BoardGame::get_move_no() {
  return _no_moves;
}

inline Key BoardGame::get_board() {
  return _board;
}

inline int BoardGame::get_to_move() {
  return _turn;
}

inline int BoardGame::get_to_move_opponent() {
  return get_to_move() == PLAYER_1 ? PLAYER_2 : PLAYER_1;
}

// inline bool BoardGame::is_terminal_state() {
//   return winning_move() || get_valid_moves().size() == 0;
// }

inline void BoardGame::print_board() {
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
