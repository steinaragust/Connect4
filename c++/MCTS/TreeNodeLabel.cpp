#include "TreeNodeLabel.h"

TreeNodeLabel::TreeNodeLabel() {
  _n = 0;
  _q = 0; // Average q gildi, [1, -1, 1, 1, ...] / n
  for (int i =  0; i < COLUMNS; i++) _p[i] = 0;
}

TreeNodeLabel::~TreeNodeLabel() {
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

array<double, COLUMNS> TreeNodeLabel::get_p() {
  return _p;
}

void TreeNodeLabel::set_p(double* value) {
  for (int i = 0; i < COLUMNS; i++) _p[i] = value[i];
}


void TreeNodeLabel::add_visit(double value) {
  _n += 1;
  _q += (value - _q) / _n;
}

void TreeNodeLabel::print_p() {
  printf("Printing p values: \n");
  for (int i = 0; i < COLUMNS; i++) printf("%lf ", _p[i]);
  printf("\n");
}
