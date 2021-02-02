#include "TreeNodeLabel.h"

TreeNodeLabel::TreeNodeLabel() {
  _n = 0;
  _q = 0; // Average q gildi, [1, -1, 1, 1, ...] / n
  _p = 0;
  for (int i = 0; i < COLUMNS; i++) {
    _children[i] = NULL;
  }
}

TreeNodeLabel::~TreeNodeLabel() {}

TreeNodeLabel::TreeNodeLabel(const TreeNodeLabel &copy) {}

int TreeNodeLabel::get_n() {
  return _n;
}

int TreeNodeLabel::get_q() {
  return _q;
}

int TreeNodeLabel::get_p() {
  return _p;
}

double TreeNodeLabel::UCT(int i) {
  if (_children[i]->_n == 0) return numeric_limits<double>::max();
  return _children[i]->_q + C * (sqrt(log(_n) / _children[i]->_n));
}

void TreeNodeLabel::add_visit(TreeNodeLabel* child, int index, int value) {
  if (index != -1) {
    _children[index] = child;
  }
  _n += 1;
  _q += (value - _q) / _n;
}

void TreeNodeLabel::add_child(TreeNodeLabel* child, int index) {
  _children[index] = child;
}

array<TreeNodeLabel*, COLUMNS> TreeNodeLabel::get_children() {
  return _children;
}

int TreeNodeLabel::chosen_index(double (&values)[COLUMNS], int best_value_index) {
  vector<int> available_columns;
  for (int i = 0; i < COLUMNS; i++) {
    if (values[i] >= values[best_value_index]) {
      available_columns.push_back(i);
    }
  }
  int random_move = rand() % available_columns.size();
  return available_columns[random_move];
}


// Velja random þegar sömu child_value eru
int TreeNodeLabel::get_best_child() {
  double best_child_value = numeric_limits<double>::lowest();
  double child_values[COLUMNS];
  int index = -1;
  for (int i = 0; i < COLUMNS; i++) {
    if (_children[i] == NULL) {
      child_values[i] = numeric_limits<double>::lowest();
      continue;
    };
    double child_value = UCT(i);
    child_values[i] = child_value;
    if (child_value > best_child_value) {
      best_child_value = child_value;
      index = i;
    }
  }
  return index;
  
  // return chosen_index(child_values, index);
}
