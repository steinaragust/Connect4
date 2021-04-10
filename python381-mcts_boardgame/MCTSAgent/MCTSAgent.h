using namespace std;

#include "../HashMapTree/HashMapTree.h"
#include "../BoardGame/BoardGame.h"
#include "Timer.cpp"

#ifndef MCTSAGENT_H
#define MCTSAGENT_H

struct IterationValue {
  int move;
  int simulations;
  double time_elapsed;
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
  MCTSAgent(GameInfo info, string name, int simulations, double max_time_seconds, int nr_threads, bool use_NN_predict);
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
  int next_batch_simulations();

  // NN methods
  void call_predict();
  virtual void predict_states(double**, vector<int**>, vector<int>) = 0;

  int _simulation_nr;
  int _nr_moves_so_far;
  GameInfo _info;

  void set_NN_predict(bool value);
  bool _use_NN_predict;

  // Thread stuff
  int _nr_threads;
  vector<Key> _states_buffer;
  vector<int> _turns_buffer;
  void fill_buffer(int thread_nr, BoardGame *game);

  private:
  mutex vector_m;
  thread _predict_worker_thread;
  IterationValue *_latest_iteration_value;
  string _name;
  int _simulations;
  double _max_time_seconds;
  HashMapTree *_tree;
};

#endif // MCTSAGENT_H
