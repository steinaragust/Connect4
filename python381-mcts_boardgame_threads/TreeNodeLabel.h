using namespace std;

#include <array>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <shared_mutex>
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
  double* get_p();
  void set_p(double *value);
  void add_visit();
  void backup_value(double value);
  void print_p(int size);

  private:
  mutable shared_mutex _mutex;
  double _virtual_loss;
  double _score;
  int _n;
  double _q;
  double *_p;
};

#endif // TREENODE_H
