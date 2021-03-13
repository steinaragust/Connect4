#include "Connect4.cpp"

void profa(BoardGame &game) {
  game.print_board();
}

int main() {
  BoardGame *game = new Connect4();
  return 0;
}
