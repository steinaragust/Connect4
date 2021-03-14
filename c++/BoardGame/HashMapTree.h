#include <vector>
#include <array>
#include <unordered_map>
#include <cstdlib>
#include <ctime>
#include "TreeNodeLabel.h"
#include "BoardGame.h"

using namespace std;

#ifndef HASHMAPTREE_H
#define HASHMAPTREE_H

class Hasher
{
// thread local for threads
static int **zobrist;
public:
  static int** initialize() {
    srand(time(0));
    int size = BoardGame::info.ROWS * BoardGame::info.COLUMNS;
    int** arr = new int*[size];
    for (int i = 0; i < size; i++) {
      arr[i] = new int[BoardGame::info.nr_unique_pcs];
      for (int j = 0; j < BoardGame::info.nr_unique_pcs; j++) {
        arr[i][j] = rand();
      }
    }
    return arr;
  };
  static void clear() {
    for (int i = 0; i < BoardGame::info.ROWS * BoardGame::info.COLUMNS; i++) delete[] zobrist[i];
    delete[] zobrist;
  };
  size_t operator() (Key const& key) const
  {
    int h = 0;
    for (int i = 0; i < BoardGame::info.ROWS; i++) {
      for (int j = 0; j < BoardGame::info.COLUMNS; j++) {
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
    for (int i = 0; i < BoardGame::info.ROWS; i++) {
      for (int j = 0; j < BoardGame::info.COLUMNS; j++) if (t1[i][j] != t2[i][j]) return false;
    }
    return true;
  };
};

class HashMapTree {
  public:
  // Constructor/Deconstructor
  HashMapTree();
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
  Key copy_key(Key key);
  void delete_key(Key key);
  int ROWS;
  int COLUMNS;
  TreeNodeLabel* _root;
  unordered_map<Key, TreeNodeLabel*, Hasher, EqualFn> _node_labels;
};

#endif // HASHMAPTREE_H