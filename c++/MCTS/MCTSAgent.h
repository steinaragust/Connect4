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
  MCTSAgent(string name = "MCTSAgent", int iterations = 200, function<void(double**, int ***, int, int)> NN_predict =  nullptr);
  ~MCTSAgent();
  MCTSAgent(const MCTSAgent &copy);

  // Public methods
  string get_name();
  HashMapTree* get_tree();
  void set_root(Connect4 &game);
  void reset();
  IterationValue get_return_value(Connect4 &game, bool random_move);
  void print_iteration_value(IterationValue &value);
  IterationValue play(Connect4 game, bool random_move = false);
  void can_win_now(Connect4 &game);

  // NN methods
  void set_predict(function<void(double**, int ***, int, int)> f);
  void call_predict(vector<Key> &states, vector<TreeNodeLabel*> &nodes, int turn);

  bool use_NN_predict;
  int _iteration_nr;
  bool _can_win;
  int _nr_moves_so_far;
  size_t root_hash_key;

  private:
  void fill_states(vector<Key> &states, int ***_states);
  void free_states(int ***states, int length);
  function<void(double**, int ***, int, int)> _predict;

  string _name;
  int _iterations;
  HashMapTree _tree;
};

#endif // MCTSAGENT_H