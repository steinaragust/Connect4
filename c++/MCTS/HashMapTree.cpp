#include "HashMapTree.h"

array<array<int, 2>, ROWS * COLUMNS> Hasher::zobrist = Hasher::intialize();

HashMapTree::HashMapTree(Key &key) {
  _root = add_node(key);
}

HashMapTree::~HashMapTree() {
  for (auto& it: _node_labels) {
    delete it.second;
  }
  // delete _node_labels;
}

HashMapTree::HashMapTree(const HashMapTree &copy) {}

TreeNodeLabel* HashMapTree::get_root() {
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
  const TreeNodeLabel* item = get_node_label(key);
  if (item == NULL) {
      TreeNodeLabel* new_label = new TreeNodeLabel();
      pair<Key, TreeNodeLabel*> new_pair(key, new_label);
      _node_labels.insert(new_pair);
      return get_node_label(key);
  }
  return NULL;
}
