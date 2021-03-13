#include "Connect4.cpp"
#include "MCTSAgent.cpp"

int main() {
  BoardGame *game = new Connect4();
  MCTSAgent agent1 = MCTSAgent(game->info);
  while(!game->is_terminal_state()) {
    agent1.play(*game);
    game->print_board();
  }



  return 0;
}
