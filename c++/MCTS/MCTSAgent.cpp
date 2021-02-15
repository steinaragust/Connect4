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

TreeNodeLabel* MCTSAgent::set_root(Key &root_key, int turn) {
  TreeNodeLabel* root = _tree.set_root(root_key);
  vector<Key> state;
  state.push_back(root_key);
  array<TreeNodeLabel*, COLUMNS> node_p = { root, NULL, NULL, NULL, NULL, NULL, NULL };
  call_predict(state, node_p, turn);
  return root;
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
  int turn = game.get_to_move();
  TreeNodeLabel* root_node = set_root(root_key, turn);
  expand(game, root_node, *this);
  
  for (int i = 0; i < _iterations; i++) {
    simulate(game, *this);
  }
  IterationValue return_value = get_return_value(root_node);
  return return_value;
}

void MCTSAgent::set_predict(function<void(double**, int ***, int, int)> f) {
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

void MCTSAgent::call_predict(vector<Key> &states, array<TreeNodeLabel*, COLUMNS> &nodes, int turn) {
  double ** values = new double*[states.size()];
   for (int s = 0; s < states.size(); s++) {
    values[s] = new double[COLUMNS + 1];
  }

  int*** _states = new int**[states.size()];
  for (int s = 0; s < states.size(); s++) {
    _states[s] = new int*[ROWS];
    for (int r = 0; r < ROWS; r++) {
      _states[s][r] = new int[COLUMNS];
    }
  }

  fill_states(states, _states);
  _predict(values, _states, turn, states.size());

  // vector<double> state_values;

  int j = 0;
  for (int i = 0; i < COLUMNS; i++) {
    if (nodes[i] != NULL) {
      nodes[i]->set_p(values[j]);
      // state_values.push_back(values[j][8]); // state value
      delete[] values[j];
      j += 1;
    }
  }
  free_states(_states, states.size());
  delete [] values;

  return;
}
