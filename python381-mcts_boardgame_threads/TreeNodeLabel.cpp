#include "TreeNodeLabel.h"

TreeNodeLabel::TreeNodeLabel() {
  _n = 0;
  _q = 0;
  _p = NULL;
  _virtual_loss = 0;
  _score = 0;
}

TreeNodeLabel::~TreeNodeLabel() {
  if (_p != NULL) {
    delete[] _p;
  }
}

TreeNodeLabel::TreeNodeLabel(const TreeNodeLabel &copy) {}

int TreeNodeLabel::get_n() {
  return _n;
}

double TreeNodeLabel::get_q() {
  return _q;
}

void TreeNodeLabel::set_q(double value) {
  _q = value;
}

double* TreeNodeLabel::get_p() {
  return _p;
}

void TreeNodeLabel::set_p(double* value) {
  _p = value;
}

void TreeNodeLabel::set_score(double value) {
  _score = value;
}

double TreeNodeLabel::get_score() {
  return _score;
}

void TreeNodeLabel::add_visit() {
  _n += 1;
  _virtual_loss += 1;
  _q = (_score - _virtual_loss) / _n;
  // _n += 1;
  // _q += (value - _q) / _n;
}

void TreeNodeLabel::backup_value(double value) {
  _virtual_loss -= 1;
  _score += value;
  _q = (_score - _virtual_loss) / _n;
}

void TreeNodeLabel::print_p(int size) {
  printf("Printing p values: \n");
  for (int i = 0; i < size; i++) printf("%lf ", _p[i]);
  printf("\n");
}
