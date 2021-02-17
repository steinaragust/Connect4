using namespace std;

#include <array>
#include <vector>
#include "math.h"

#ifndef TREENODELABEL_H
#define TREENODELABEL_H

const int ROWS = 6;
const int COLUMNS = 7;
const double C = 0.2;

class TreeNodeLabel {
  public:
  // Constructor/Deconstructor
  TreeNodeLabel();
  ~TreeNodeLabel();
  TreeNodeLabel(const TreeNodeLabel &copy);

  // Functions
  int get_n();
  double get_q();
  void set_q(double value);
  array<double, COLUMNS> get_p();
  void set_p(double *value);
  void add_visit(TreeNodeLabel* child, int index, double value);
  void add_child(TreeNodeLabel* child, int index);
  array<TreeNodeLabel*, COLUMNS> get_children();
  int get_best_child(bool use_PUCT);

  private:
  double UCT(int i);
  double PUCT(int i);
  int chosen_index(double (&values)[COLUMNS], int best_value_index);
  array<TreeNodeLabel*, COLUMNS> _children;
  int _n;
  double _q;
  array<double, COLUMNS> _p;
};

#endif // TREENODE_H
