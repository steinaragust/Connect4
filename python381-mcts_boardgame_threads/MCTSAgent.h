using namespace std;

#include "HashMapTree.h"
#include "BoardGame.h"

#ifndef MCTSAGENT_H
#define MCTSAGENT_H

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
  MCTSAgent(GameInfo info, string name, int simulations, int nr_expands);
  virtual ~MCTSAgent();
  MCTSAgent(const MCTSAgent &copy);

  // Public methods
  void set_name(string name);
  string get_name();
  HashMapTree* get_tree();
  void set_root(BoardGame &game);
  void reset(BoardGame &agent);
  IterationValue* get_return_value(BoardGame &game, bool random_move);
  void print_iteration_value();
  IterationValue* play(BoardGame &game, bool random_move = false);
  int can_win_now(BoardGame &game);
  void set_NN_predict(bool value);
  int next_batch_simulations();

  // NN methods
  void call_predict(vector<Key> &states, vector<int> &turns, vector<TreeNodeLabel*> &nodes);

  virtual void predict(double**, vector<int**>, vector<int>) = 0;

  int _simulation_nr;
  int _nr_moves_so_far;
  GameInfo _info;
  Key* _state_buffer;
  int _nr_expands;
  condition_variable worker_cv;
  mutex worker_m;
  int _thread_counter;


  private:
  IterationValue *_latest_iteration_value;
  string _name;
  int _simulations;
  HashMapTree _tree;
};

#endif // TREENODE_H
