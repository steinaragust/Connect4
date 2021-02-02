using namespace std;

#include "../Game.h"
#include "HashMapTree.h"

void select(Connect4 &game, TreeNodeLabel *parent, vector<int> &path) {
  if (game.is_terminal_state()) {
    return;
  }
  array<TreeNodeLabel*, COLUMNS> children = parent->get_children();
  for (int i = 0; i < COLUMNS; i++) if ((children[i] == NULL || children[i]->get_n() == 0) && game.is_valid_column(i)) {
    game.drop_piece_in_column(i);
    path.push_back(i);
    return;
  }
  int best_child = parent->get_best_child();
  game.drop_piece_in_column(best_child);
  path.push_back(best_child);
  select(game, children[best_child], path);
}

double playout (Connect4 &game, vector<int> &path) {
  int player = game.get_to_move();
  while (!game.is_terminal_state()) {
    vector<int> moves = game.get_valid_columns();
    int random_move = rand() % moves.size();
    game.drop_piece_in_column(moves[random_move]);
    path.push_back(moves[random_move]);
  }
  if (game.winning_move()) {
    return 1;
  }
  return 0.5;
}

void backup_value(HashMapTree& tree, Key &key, TreeNodeLabel* &parent, TreeNodeLabel* child, int column, double value) {
  if (parent == NULL) {
    parent = tree.add_node(key);
  }
  parent->add_visit(child, column, value);
}

// Backup simulation (parent og pointer á child)
void backup_simulation(Connect4 &game, HashMapTree& tree, vector<int> &path, double value) {
  int move = -1;
  TreeNodeLabel* child = NULL;
  for (vector<int>::reverse_iterator i = path.rbegin(); i != path.rend(); ++i ) { 
    Key key = game.get_board();
    game.retract_piece_in_column(*i);
    TreeNodeLabel* parent = tree.get_node_label(key);
    backup_value(tree, key, parent, child, move, value);
    value = -value;
    move = *i;
    child = parent;
  }
  Key key = game.get_board();
  TreeNodeLabel* parent = tree.get_node_label(key);
  backup_value(tree, key, parent, child, move, value);
}

void simulate(Connect4 &game, HashMapTree &tree) {
  vector<int> path;
  select(game, tree.get_root(), path);
  double value = playout(game, path);
  backup_simulation(game, tree, path, value);
}
