#pragma once
#include <chrono>
#include "printf.h"
#include "string"
#include "map"
#include "Game.h"

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

array<int, 2> mini_max(Connect4, int, int, int, bool);

string players_arr[] = {"John", "Dave"};

string play_a_game(string player_1, string player_2)
{
  Connect4 game = Connect4();

  while (!game.is_terminal_state())
  {
    int column = mini_max(game, 5, numeric_limits<int>::min(), numeric_limits<int>::max(), true)[0];
    game.drop_piece_in_column(column);
  }

  game.print_board();

  if (game.winning_move())
  {
    return game.get_current_opponent_piece() == game.PLAYER_1 ? player_1 : player_2;
  }
  else
  {
    return "Draw";
  }
}

void play_a_match(map<string, int> *scores)
{
  string first = play_a_game(players_arr[0], players_arr[1]);
  string second = play_a_game(players_arr[1], players_arr[0]);

  (*scores)[first] += 1;
  (*scores)[second] += 1;
}

int main()
{
  map<string, int> score;

  score[players_arr[0]] = 0;
  score[players_arr[1]] = 0;
  score["Draw"] = 0;
  Connect4 game = Connect4();

  Timer timer = Timer();
  timer.Start();

  for (int i = 0; i < 1; i++)
  {
    play_a_match(&score);
  }

  printf("Time: %f\n", timer.GetDuration());
  printf("%s: %d, %s: %d, Draws: %d\n", players_arr[0].c_str(), score[players_arr[0]], players_arr[1].c_str(), score[players_arr[1]], score["Draws"]);

  return 0;
}
