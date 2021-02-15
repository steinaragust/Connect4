#include "MCTSAgent.h"

void simulate(Connect4 &game, MCTSAgent &agent);
void expand(Connect4 &game, TreeNodeLabel* node,  MCTSAgent &agent);

MCTSAgent::MCTSAgent(string name, int iterations) {
  _name = name;
  _iterations = iterations;
  _tree = HashMapTree();
  use_NN_predict = false;
  // _predict = NN_predict;
  // use_NN_predict = NN_predict != nullptr;
}

MCTSAgent::~MCTSAgent() {
}

string MCTSAgent::get_name() {
  return _name;
}

HashMapTree* MCTSAgent::get_tree() {
  return &_tree;
}

TreeNodeLabel* MCTSAgent::set_root(Key &root_key) {
  return _tree.set_root(root_key);
}

void MCTSAgent::reset() {
  _tree.clear_map();
}

IterationValue MCTSAgent::get_return_value(TreeNodeLabel* root) {
  int total = 0;
  int column = -1;
  array<double, COLUMNS> q_values;
  array<int, COLUMNS> n_values;
  array<double, COLUMNS> policy;
  array<TreeNodeLabel*, COLUMNS> children = root->get_children();
  for (int i = 0; i < COLUMNS; i++) {
    if(children[i] != NULL) {
      n_values[i] = children[i]->get_n();
      q_values[i] = children[i]->get_q();
      total += n_values[i];
    } else {
      n_values[i] = numeric_limits<int>::min();
      q_values[i] = numeric_limits<double>::lowest();
      policy[i] = 0.0;
    }
  }
  int max_visits = numeric_limits<int>::min();
  for (int i = 0; i < COLUMNS; i++) {
    if (children[i] != NULL) {
      policy[i] = (double)n_values[i] / (double)total;
      if (n_values[i] > max_visits) {
        max_visits = n_values[i];
        column = i;
      }
    }
  }
  return {column, q_values, n_values, policy};
}

void MCTSAgent::print_iteration_value(IterationValue &value) {
  printf("Column: %d\n", value.column);
  printf("n values: ");
  for (int i = 0; i < COLUMNS; i++) {
    if (value.n_values[i] > numeric_limits<int>::min()) {
      printf("%d ", value.n_values[i]);
    } else {
      printf("NULL ");
    }
  }
  printf("\n");
  printf("q values: ");
  for (int i = 0; i < COLUMNS; i++) {
    if (value.n_values[i] > numeric_limits<int>::min()) {
      printf("%.2f ", value.q_values[i]);
    } else {
      printf("NULL ");
    }
  }
  printf("\n");
  printf("policy values: ");
  for (int i = 0; i < COLUMNS; i++) {
    if (value.n_values[i] > numeric_limits<int>::min()) {
      printf("%.2f ", value.policy[i]);
    } else {
      printf("NULL ");
    }
  }
  printf("\n");
}

IterationValue MCTSAgent::play(Connect4 game) {
  reset();
  Key root_key = game.get_board();
  TreeNodeLabel* root_node = set_root(root_key);
  expand(game, root_node, *this);
  
  for (int i = 0; i < _iterations; i++) {
    simulate(game, *this);
  }
  IterationValue return_value = get_return_value(root_node);
  // print_iteration_value(return_value);
  printf("Returning \n");
  return return_value;
}

void MCTSAgent::set_predict(function<void(double[8], int ***, int, int)> f) {
  use_NN_predict = true;
  _predict = f;
}

void MCTSAgent::fill_states(vector<Key> &states, int ***_states) {
  for (int s = 0; s < states.size(); s++) {
    for (int r = 0; r < ROWS; r++) {
      for (int c = 0; c < COLUMNS; c++) {
        _states[s][r][c] = states[s][r][c];
      }
    }
  }
}

void MCTSAgent::free_states(int ***states, int length) {
    for (int s = 0; s < length; s++) {
    for (int r = 0; r < ROWS; r++) {
      delete[] states[s][r];
    }
    delete[] states[s];
  }
  delete[] states;
}

array<double, COLUMNS + 1> MCTSAgent::call_predict(vector<Key> &states, int turn) {
  double *_values = new double [COLUMNS + 1];
  int*** _states = new int**[states.size()];
  for (int s = 0; s < states.size(); s++) {
    _states[s] = new int*[ROWS];
    for (int r = 0; r < ROWS; r++) {
      _states[s][r] = new int[COLUMNS];
    }
  }
  fill_states(states, _states);
  _predict(_values, _states, turn, states.size());
  array<double, COLUMNS + 1> return_values;
  for (int i = 0; i < COLUMNS + 1; i++) {
    return_values[i] = _values[i];
  }
  free_states(_states, states.size());

  // for (int i = 0; i < COLUMNS + 1; i++) {
  //   printf("%lf ", return_values[i]);
  // }
  // printf("\n");

  // Last index is state value, which is not used for training
  return return_values;
}
