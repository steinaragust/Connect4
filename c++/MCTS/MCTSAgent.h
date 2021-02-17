using namespace std;

#include "HashMapTree.h"
#include "../Connect4-Game/Game.h"

#ifndef MCTSAGENT_H
#define MCTSAGENT_H

struct IterationValue {
  int column;
  double q_value;
  array<double, COLUMNS> q_values;
  array<int, COLUMNS> n_values;
  array<double, COLUMNS> policy;
};

class MCTSAgent {
  public:
  // Constructor/Deconstructor
  MCTSAgent(string name = "MCTSAgent", int iterations = 200);
  ~MCTSAgent();
  MCTSAgent(const MCTSAgent &copy);

  // Public methods
  string get_name();
  HashMapTree* get_tree();
  TreeNodeLabel* set_root(Key &root_key, int turn);
  void reset();
  void print_iteration_value(IterationValue &value);
  IterationValue play(Connect4 game);

  // NN methods
  void set_predict(function<void(double**, int ***, int, int)> f);
  void call_predict(vector<Key> &states, vector<TreeNodeLabel*> &nodes, int turn);

  bool use_NN_predict;
  int _iteration_nr;

  private:
  void fill_states(vector<Key> &states, int ***_states);
  void free_states(int ***states, int length);
  function<void(double**, int ***, int, int)> _predict;
  IterationValue get_return_value(TreeNodeLabel* root);

  string _name;
  int _iterations;
  HashMapTree _tree;
};

#endif // TREENODE_H

