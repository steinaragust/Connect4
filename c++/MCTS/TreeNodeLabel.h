using namespace std;

#include <array>
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
  int get_q();
  int get_p();
  void add_visit(TreeNodeLabel* child, int index, int value);
  array<TreeNodeLabel*, COLUMNS> get_children();
  int get_best_child();

  private:
  double UCT(int i);
  array<TreeNodeLabel*, COLUMNS> _children;
  int _n;
  double _q;
  double _p;
};

#endif // TREENODE_H
