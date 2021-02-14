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

string play_game(Connect4 &game, MCTSAgent &player1, MCTSAgent &player2) {
  game.reset();
  while(!game.is_terminal_state()) {
    int turn = game.get_to_move();
    IterationValue value;
    if (turn == game.PLAYER_1) value = player1.play(game);
    else value = player2.play(game);
    game.drop_piece_in_column(value.column);
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
  MCTSAgent john = MCTSAgent("John", 200, nullptr);
  MCTSAgent rick = MCTSAgent("Rick", 200, nullptr);
  Timer timer = Timer();
  timer.Start();
  for (int i = 0; i < 3; i++) {
    string match1 = play_game(game, john, rick);
    string match2 = play_game(game, rick, john);
  }
  // printf("Winner of match 1: %s\n", match1.c_str());
  // printf("Winner of match 2: %s\n", match2.c_str());
  printf("Time: %f\n", timer.GetDuration());
  return 0;
}