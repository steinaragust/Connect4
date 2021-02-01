using namespace std;

#include "HashMapTree.h"
#include "../Game.h"

#ifndef MCTSAGENT_H
#define MCTSAGENT_H

struct IterationValue {
  int column;
  array<int, COLUMNS> q_values;
  array<int, COLUMNS> n_values;
  array<double, COLUMNS> policy;
};

class MCTSAgent {
  public:
  // Constructor/Deconstructor
  MCTSAgent();
  MCTSAgent(string name = "MCTS Agent", int iterations = 200);
  ~MCTSAgent();
  MCTSAgent(const MCTSAgent &copy);

  // Public methods
  string get_name();
  TreeNodeLabel* set_root(Key &root_key);
  void reset();
  IterationValue play(Connect4 game);

  private:
  IterationValue get_return_value(TreeNodeLabel* root);

  string _name;
  int _iterations;
  HashMapTree _tree;
};

#endif // TREENODE_H

