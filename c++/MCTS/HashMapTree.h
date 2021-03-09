#include <vector>
#include <array>
#include <unordered_map>
#include <cstdlib>
#include <ctime>
#include "TreeNodeLabel.h"

using namespace std;

#ifndef HASHMAPTREE_H
#define HASHMAPTREE_H

typedef array<array<int, COLUMNS>, ROWS> Key;

class Hasher
{
private:
static array<array<int, 2>, ROWS * COLUMNS> intialize() {
  srand(time(0));
  array<array<int, 2>, ROWS * COLUMNS> arr;
  for (int i = 0; i < ROWS * COLUMNS; i++) {
    for (int j = 0; j < 2; j++) {
      arr[i][j] = rand();
    }
  }
  return arr;
};
static array<array<int, 2>, ROWS * COLUMNS> zobrist;
public:
  size_t operator() (Key const& key) const
  {
    int h = 0;
    for (int i = 0; i < ROWS; i++) {
      for (int j = 0; j < COLUMNS; j++) {
        if (key[i][j] != 0) {
          int k = key[i][j] - 1;
          h = h ^ zobrist[(i + 1)*(j + 1) - 1][k];
        }
      }
    }

    return h;
  };
};
class EqualFn
{
public:
  bool operator() (Key const& t1, Key const& t2) const
  {
    for (int i = 0; i < ROWS; i++) {
      for (int j = 0; j < COLUMNS; j++) if (t1[i][j] != t2[i][j]) return false;
    }
    return true;
  };
};

class HashMapTree {
  public:
  // Constructor/Deconstructor
  HashMapTree();
  HashMapTree(Key &root_key);
  ~HashMapTree();
  HashMapTree(const HashMapTree &copy);

  //Public functions
  TreeNodeLabel* set_root(Key &root_key);
  TreeNodeLabel* get_root();
  TreeNodeLabel* get_node_label(Key &key);
  TreeNodeLabel* add_node(Key &key);
  bool is_equal(Key &key1, Key &key2);
  size_t hash_code(Key &key);
  void clear_map();
  void print_map_size();

  private:
  TreeNodeLabel* _root;
  unordered_map<Key, TreeNodeLabel*, Hasher, EqualFn> _node_labels;
};

#endif // HASHMAPTREE_H
