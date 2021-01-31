#pragma once
#include "../Game.h"
#include "HashMapTree.h"

using namespace std;


void simulate(Connect4 game, HashMapTree &tree);

int main() {
  Connect4 game = Connect4();
  Key root = game.get_board();
  HashMapTree tree = HashMapTree(root);
  simulate(game, tree);

  return 0;
}