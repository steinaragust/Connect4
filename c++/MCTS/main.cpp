#pragma once
#include "../Game.h"
#include "HashMapTree.h"
#include <chrono>
#include<printf.h>

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

void simulate(Connect4 &game, HashMapTree &tree);

int main() {
  Connect4 game = Connect4();
  Key root = game.get_board();
  HashMapTree tree = HashMapTree(root);
  Timer timer = Timer();
  timer.Start();
  for (int i = 0; i < 200; i++) {
    simulate(game, tree);
    game.reset();
  }
  printf("Time: %f\n", timer.GetDuration());
  return 0;
}