using namespace std;

#include "HashMapTree.h"
#include "../Connect4-Game/Game.h"

#ifndef MCTSAGENT_H
#define MCTSAGENT_H

struct IterationValue {
  int column;
  array<double, COLUMNS> q_values;
  array<int, COLUMNS> n_values;
  array<double, COLUMNS> policy;
};

class MCTSAgent {
  public:
  // Constructor/Deconstructor
  MCTSAgent(string name = "MCTSAgent", int iterations = 200, function<void(float *, int **, int)> NN_predict = nullptr);
  ~MCTSAgent();
  MCTSAgent(const MCTSAgent &copy);

  // Public methods
  string get_name();
  HashMapTree* get_tree();
  TreeNodeLabel* set_root(Key &root_key);
  void reset();
  void print_iteration_value(IterationValue &value);
  IterationValue play(Connect4 game);

  // NN methods
  void set_predict(function<void(float *, int **, int)> f);
  array<float, COLUMNS + 1> call_predict(array<array<int, COLUMNS>, ROWS> board, int turn);

  private:
  function<void(float *, int **, int)> _predict;
  IterationValue get_return_value(TreeNodeLabel* root);

  string _name;
  int _iterations;
  HashMapTree _tree;
};

#endif // TREENODE_H

