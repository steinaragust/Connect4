#pragma once
#include "../Game.h"
#include "HashMapTree.h"

using namespace std;


void simulate(Connect4 &game, HashMapTree &tree);

int main() {
  Connect4 game = Connect4();
  Key root = game.get_board();
  HashMapTree tree = HashMapTree(root);
  for (int i = 0; i < 200; i++) {
    simulate(game, tree);
    game.reset();
  }
  return 0;
}