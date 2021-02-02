#include "MCTSAgent.h"

void simulate(Connect4 &game, HashMapTree &tree);

MCTSAgent::MCTSAgent(string name, int iterations) {
  _name = name;
  _iterations = iterations;
  _tree = HashMapTree();
}

MCTSAgent::~MCTSAgent() {
}

string MCTSAgent::get_name() {
  return _name;
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
      policy[i] = numeric_limits<double>::lowest();
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
  return {column, q_values, n_values, policy };
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
  for (int i = 0; i < _iterations; i++) {
    simulate(game, _tree);
  }
  IterationValue return_value = get_return_value(root_node);
  // print_iteration_value(return_value);
  return return_value;
}
