#pragma once
#include <chrono>
#include<printf.h>
#include "MCTSAgent.h"

using namespace std;

class Timer
{
private:
  std::chrono::time_point<std::chrono::steady_clock> m_StartTime;

public:
  void Start()
  {
    m_StartTime = std::chrono::high_resolution_clock::now();
  }
  float GetDuration()
  {
    std::chrono::duration<float> duration = std::chrono::high_resolution_clock::now() - m_StartTime;
    return duration.count();
  }
};

void print_iteration_value(IterationValue &value) {
  printf("Column: %d\n", value.column);
  printf("n values: ");
  for (int i = 0; i < COLUMNS; i++) printf("%d ", value.n_values[i]);
  printf("\n");
  printf("q values: ");
  for (int i = 0; i < COLUMNS; i++) printf("%d ", value.q_values[i]);
  printf("\n");
  printf("policy values: ");
  for (int i = 0; i < COLUMNS; i++) printf("%f ", value.policy[i]);
  printf("\n");
}

string play_game(Connect4 &game, MCTSAgent &player1, MCTSAgent &player2) {
  game.reset();
  while(!game.is_terminal_state()) {
    int turn = game.get_to_move();
    IterationValue value;
    if (turn == game.PLAYER_1) value = player1.play(game);
    else value = player2.play(game);
    game.drop_piece_in_column(value.column);
    if (game.is_terminal_state()) {
      print_iteration_value(value);
    }
  }
  game.print_board();
  
  if (game.winning_move()) {
    if (game.get_to_move() == game.PLAYER_1) return player2.get_name();
    else return player1.get_name();
  }
  return "Draw";
}

int main() {
  Connect4 game = Connect4();
  MCTSAgent stalin = MCTSAgent("Stalin", 200);
  MCTSAgent hitler = MCTSAgent("hitler", 200);
  Timer timer = Timer();
  timer.Start();
  string match1 = play_game(game, stalin, hitler);
  string match2 = play_game(game, hitler, stalin);
  printf("Winner of match 1: %s\n", match1.c_str());
  printf("Winner of match 2: %s\n", match2.c_str());
  printf("Time: %f\n", timer.GetDuration());
  return 0;
}