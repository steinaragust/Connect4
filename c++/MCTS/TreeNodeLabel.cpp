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
  return _children[i]->_q + C * (sqrt((log(_n) / _children[i]->_n)));
}

void TreeNodeLabel::add_visit(TreeNodeLabel* child, int index, int value) {
  if (index != -1) {
    _children[index] = child;
  }
  _n += 1;
  _q += (value - _q) / _n;
}

array<TreeNodeLabel*, COLUMNS> TreeNodeLabel::get_children() {
  return _children;
}

int TreeNodeLabel::get_best_child() {
  double best_child_value = numeric_limits<double>::lowest();
  int index = -1;
  for (int i = 0; i < COLUMNS; i++) {
    if (_children[i] == NULL) continue;
    double child_value = UCT(i);
    if (child_value > best_child_value) {
      index = i;
    }
  }
  return index;
}
