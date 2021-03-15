#include "HashMapTree.h"

unique_ptr<unique_ptr<int[]>[]> Hasher::zobrist;

int Hasher::ROWS = -1;
int Hasher::COLUMNS = -1;
int Hasher::NR_UNIQUE_PCS = -1;

HashMapTree::HashMapTree() {}

HashMapTree::HashMapTree(GameInfo &info) {
  if (Hasher::zobrist == nullptr) {
    Hasher::initialize(info);
  }
  ROWS = info.ROWS;
  COLUMNS = info.COLUMNS;
  _root = NULL;
}

HashMapTree::~HashMapTree() {
  clear_map();
}

HashMapTree::HashMapTree(const HashMapTree &copy) {}

TreeNodeLabel* HashMapTree::get_root() {
  return _root;
}

TreeNodeLabel* HashMapTree::set_root(Key &root_key) {
  _root = add_node(root_key);
  return _root;
}

TreeNodeLabel* HashMapTree::get_node_label(Key &key) {
  unordered_map<Key, TreeNodeLabel*, Hasher, EqualFn>::const_iterator it = _node_labels.find(key);
  if (it != _node_labels.end()) {
    TreeNodeLabel* item = it->second;
    return item;
  } else {
    return NULL;
  }
}

TreeNodeLabel* HashMapTree::add_node(Key &key) {
  TreeNodeLabel* item = get_node_label(key);
  if (item == NULL) {
      TreeNodeLabel* new_label = new TreeNodeLabel();
      Key new_key = copy_key(key);
      pair<Key, TreeNodeLabel*> new_pair(new_key, new_label);
      _node_labels.insert(new_pair);
      return new_label;
  }
  return item;
}

Key HashMapTree::copy_key(Key key) {
  Key new_key = new int*[ROWS];
  for (int i = 0; i < ROWS; i++) {
    new_key[i] = new int[COLUMNS];
    for (int j = 0; j < COLUMNS; j++) new_key[i][j] = key[i][j];
  }
  return new_key;
}

void HashMapTree::delete_key(Key key) {
  for (int i = 0; i < ROWS; i++) delete[] key[i];
  delete[] key;
}

void HashMapTree::clear_map() {
  for (auto& it: _node_labels) {
    delete_key(it.first);
    delete it.second;
  }
  _node_labels.clear();
  _root = NULL;
}

void HashMapTree::print_map_size() {
  int size = 0;
  for(auto it=_node_labels.begin();it!=_node_labels.end();it++) {
    size += 1; 
  }
  printf("Size of map: %d\n", size);
}

bool HashMapTree::is_equal(Key &key1, Key &key2) {
  unordered_map<Key, TreeNodeLabel*, Hasher, EqualFn>::key_equal fn = _node_labels.key_eq();
  return fn(key1, key2);
}

size_t HashMapTree::hash_code(Key &key) {
  unordered_map<Key, TreeNodeLabel*, Hasher, EqualFn>::hasher fn = _node_labels.hash_function();
  return fn(key);
}
