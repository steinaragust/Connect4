#include "TreeNodeLabel.h"

TreeNodeLabel::TreeNodeLabel() {
  _n = 0;
  _q = 0; // Average q gildi, [1, -1, 1, 1, ...] / n
  _p = NULL;
}

TreeNodeLabel::~TreeNodeLabel() {
  delete[] _p;
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


void TreeNodeLabel::add_visit(double value) {
  _n += 1;
  _q += (value - _q) / _n;
}

void TreeNodeLabel::print_p(int length) {
  printf("Printing p values: \n");
  for (int i = 0; i < length; i++) printf("%lf ", _p[i]);
  printf("\n");
}
