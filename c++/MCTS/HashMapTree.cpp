#include "HashMapTree.h"

array<array<int, 2>, ROWS * COLUMNS> Hasher::zobrist = Hasher::intialize();

HashMapTree::HashMapTree() {
  _root = NULL;
}

HashMapTree::HashMapTree(Key &root_key) {
  _root = add_node(root_key);
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
      pair<Key, TreeNodeLabel*> new_pair(key, new_label);
      _node_labels.insert(new_pair);
      return get_node_label(key);
  }
  return item;
}

void HashMapTree::clear_map() {
    for (auto& it: _node_labels) {
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
