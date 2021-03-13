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
private:
inline static int NR_UNIQUE_PCS;
inline static int ROWS;
inline static int COLUMNS;
// thread local for threads
inline static int **zobrist;
public:
  ~Hasher() {
    clear();
  };
  static void initialize(int _rows, int _columns, int _nr_unique_pcs) {
    NR_UNIQUE_PCS = _nr_unique_pcs;
    ROWS = _rows;
    COLUMNS = _rows;
    srand(time(0));
    zobrist = new int*[ROWS * COLUMNS];
    for (int i = 0; i < ROWS * COLUMNS; i++) {
      zobrist[i] = new int[NR_UNIQUE_PCS];
      for (int j = 0; j < NR_UNIQUE_PCS; j++) {
        zobrist[i][j] = rand();
      }
    }
  };
  static void clear() {
    for (int i = 0; i < ROWS * COLUMNS; i++) delete[] zobrist[i];
    delete[] zobrist;
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
  inline static void initialize(int _rows, int _columns) {
    ROWS = _rows;
    COLUMNS = _columns;
  };
  bool operator() (Key const& t1, Key const& t2) const
  {
    for (int i = 0; i < ROWS; i++) {
      for (int j = 0; j < COLUMNS; j++) if (t1[i][j] != t2[i][j]) return false;
    }
    return true;
  };
  private:
  inline static int ROWS;
  inline static int COLUMNS;
};

class HashMapTree {
  public:
  // Constructor/Deconstructor
  HashMapTree();
  HashMapTree(GameInfo info);
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
