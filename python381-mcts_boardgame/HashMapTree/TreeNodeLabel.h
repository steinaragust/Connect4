using namespace std;

#include <array>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <shared_mutex>
#include <algorithm>
#include "math.h"
#include "../BoardGame/BoardGame.h"

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
  void print_p(int size);
  void add_visit();
  void retract_visit();
  void backup_value(double value);
  int get_virtual_loss();

  private:
  int _n;
  double _q;
  double *_p;
  mutable shared_mutex _mutex;
  bool _use_threads;
  double _virtual_loss;
  double _score;
};

#endif // TREENODE_H
