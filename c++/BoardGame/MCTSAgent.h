using namespace std;

#include "HashMapTree.h"
#include "BoardGame.h"

#ifndef MCTSAGENT_H
#define MCTSAGENT_H

typedef void (*Predict_func) (double**, vector<int**>, vector<int>);

struct IterationValue {
  int move;
  double q_value;
  double *q_values;
  int *n_values;
  double *policy;

  IterationValue(int size) {
    q_values = new double[size];
    n_values = new int[size];
    policy = new double[size];
  };
  ~IterationValue() {
    delete[] q_values;
    delete[] n_values;
    delete[] policy;
  }
};

class MCTSAgent {
  public:
  // Constructor/Deconstructor
  MCTSAgent(string name, int iterations, bool use_NN_predict);
  virtual ~MCTSAgent();
  MCTSAgent(const MCTSAgent &copy);

  // Public methods
  string get_name();
  HashMapTree* get_tree();
  void set_root(BoardGame &game);
  void reset(BoardGame &agent);
  IterationValue* get_return_value(BoardGame &game, bool random_move);
  void print_iteration_value();
  IterationValue* play(BoardGame &game, bool random_move = false);
  void can_win_now(BoardGame &game);

  // NN methods
  void call_predict(vector<Key> &states, vector<int> &turns, vector<TreeNodeLabel*> &nodes);

  virtual void predict(double**, vector<int**>, vector<int>) = 0;

  bool _use_NN_predict;
  int _iteration_nr;
  bool _can_win;
  int _nr_moves_so_far;

  private:
  IterationValue *_latest_iteration_value;
  string _name;
  int _iterations;
  HashMapTree _tree;
};

#endif // TREENODE_H
