#include "Connect4.cpp"
#include "MCTSAgent.cpp"

int main() {
  BoardGame *game = new Connect4();
  MCTSAgent agent1 = MCTSAgent();
  while(!game->is_terminal_state()) {
    IterationValue* value = agent1.play(*game);
    game->make_move(value->move);
    game->print_board();
  }

  return 0;
}
