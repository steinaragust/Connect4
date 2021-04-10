#include <vector>
#include <array>
#include <unordered_map>
#include <cstdlib>
#include <ctime>
#include "TreeNodeLabel.h"

using namespace std;

#ifndef HASHMAPTREE_H
#define HASHMAPTREE_H

class Hasher
{
// thread local for threads
public:
  static unique_ptr<unique_ptr<int[]>[]> zobrist;
  static int ROWS;
  static int COLUMNS;
  static int NR_UNIQUE_PCS;
  static void initialize(GameInfo &info) {
    srand(time(0));
    ROWS = info.ROWS;
    COLUMNS = info.COLUMNS;
    NR_UNIQUE_PCS = info.nr_unique_pcs;
    int size = ROWS * COLUMNS;
    zobrist = make_unique<unique_ptr<int[]>[] >(size);
    for (int i = 0; i < size; i++) {
        unique_ptr<int[]> temp_ptr(new int[NR_UNIQUE_PCS]);
        for (int j = 0; j < NR_UNIQUE_PCS; j++)
            temp_ptr[j] = rand();
        zobrist[i] = move(temp_ptr);
    }
  };
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
    for (int i = 0; i < Hasher::ROWS; i++) {
      for (int j = 0; j < Hasher::COLUMNS; j++) if (t1[i][j] != t2[i][j]) return false;
    }
    return true;
  };
};

class HashMapTree {
  public:
  // Constructor/Deconstructor
  HashMapTree();
  HashMapTree(GameInfo &info);
  ~HashMapTree();
  HashMapTree(const HashMapTree &copy);

  //Public functions
  TreeNodeLabel* set_root(Key &root_key);
  TreeNodeLabel* get_root();
  TreeNodeLabel* get_node_label(Key &key);
  // Not thread safe
  TreeNodeLabel* add_node(Key &key);
  // Thread safe function that returns NULL if node exists
  TreeNodeLabel* add_node_if_not_exists(Key &key);
  bool is_equal(Key &key1, Key &key2);
  size_t hash_code(Key &key);
  void clear_map();
  void print_map_size();

  private:
  mutable shared_mutex _mutex;
  Key copy_key(Key key);
  void delete_key(Key key);
  int ROWS;
  int COLUMNS;
  TreeNodeLabel* _root;
  unordered_map<Key, TreeNodeLabel*, Hasher, EqualFn> _node_labels;
};

#endif // HASHMAPTREE_H
