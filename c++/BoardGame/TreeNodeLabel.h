using namespace std;

#include <array>
#include <vector>
#include "math.h"
#include "BoardGame.h"

#ifndef TREENODELABEL_H
#define TREENODELABEL_H

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
  double* get_p();
  void set_p(double *value);
  void add_visit(double value);
  void print_p();

  private:
  int _n;
  double _q;
  double *_p;
};

#endif // TREENODE_H
