using namespace std;

#include <vector>
#include <array>
#include "math.h"

#ifndef TREENODELABEL_H
#define TREENODELABEL_H

const int ROWS = 6;
const int COLUMNS = 7;

class TreeNodeLabel {
  public:
  // Constructor/Deconstructor
  TreeNodeLabel();
  ~TreeNodeLabel();
  TreeNodeLabel(const TreeNodeLabel &copy);

  // Functions
  void add_visit(TreeNodeLabel* child, int index, int value);
  array<TreeNodeLabel*, COLUMNS> get_children();
  int get_best_child();

  private:
  double UCT(int i);
  array<TreeNodeLabel*, COLUMNS> _children;
  int _n;
  double _q;
  double _p;
  double _c;
};

#endif // TREENODE_H
