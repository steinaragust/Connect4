#include "TreeNodeLabel.h"

TreeNodeLabel::TreeNodeLabel(bool use_threads) {
  _n = 0;
  _q = 0; // Average q gildi, [1, -1, 1, 1, ...] / n
  _p = NULL;
  _virtual_loss = 0;
  _score = 0;
  _use_threads = use_threads;
}

TreeNodeLabel::~TreeNodeLabel() {
  if (_p != NULL) {
    delete[] _p;
  }
}

TreeNodeLabel::TreeNodeLabel(const TreeNodeLabel &copy) {
  _n = copy._n;
  _q = copy._q;
  _p = copy._p;
  _virtual_loss = copy._virtual_loss;
  _score = copy._score;
  _use_threads = copy._use_threads;
}

int TreeNodeLabel::get_n() {
  shared_lock lock(_mutex);
  return _n;
}

double TreeNodeLabel::get_q() {
  shared_lock lock(_mutex);
  return _q;
}


void TreeNodeLabel::set_q(double value) {
  _q = value;
}

double* TreeNodeLabel::get_p() {
  shared_lock lock(_mutex);
  return _p;
}

void TreeNodeLabel::set_p(double* value) {
  unique_lock lock(_mutex);
  _p = value;
}


void TreeNodeLabel::add_visit(double value) {
  if (_use_threads) {
    unique_lock lock(_mutex);
    _n += 1;
    _virtual_loss += 1;
    _q = (_score - _virtual_loss) / _n;
  } else {
    _n += 1;
    _q += (value - _q) / _n; 
  }
}

void TreeNodeLabel::retract_visit() {
  unique_lock lock(_mutex);
  _n -= 1;
  _virtual_loss -= 1;
  _q = (_score - _virtual_loss) / _n;
}

void TreeNodeLabel::backup_value(double value) {
  unique_lock lock(_mutex);
  _virtual_loss -= 1;
  _score += value;
  _q = (_score - _virtual_loss) / _n;
}

int TreeNodeLabel::get_virtual_loss() {
  shared_lock lock(_mutex);
  return _virtual_loss;
}

void TreeNodeLabel::print_p(int size) {
  printf("Printing p values: \n");
  for (int i = 0; i < size; i++) printf("%lf ", _p[i]);
  printf("\n");
}
