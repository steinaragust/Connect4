using namespace std;

#include "../Connect4-Game/Game.h"
#include "MCTSAgent.h"

TreeNodeLabel* expand(Connect4 &game, Key &key, MCTSAgent &agent) {
  HashMapTree* tree = agent.get_tree();
  TreeNodeLabel* node = tree->add_node(key);
  vector<Key> states { key };
  vector<TreeNodeLabel*> nodes { node };

  int turn = game.get_to_move();
  agent.call_predict(states, nodes, turn);

  return node;
}

double select(Connect4 &game, TreeNodeLabel *parent, vector<int> &path, MCTSAgent &agent) {
  if (game.is_terminal_state()) {
    if (game.winning_move()) {
      return 1;
    }
    return 0.5;
  }
  array<TreeNodeLabel*, COLUMNS> children = parent->get_children();
  for (int i = 0; i < COLUMNS; i++) if (game.is_valid_column(i) && (children[i] == NULL || children[i]->get_n() == 0)) {
    game.drop_piece_in_column(i);
    path.push_back(i);
    if (agent.use_NN_predict) {
      Key child_key = game.get_board();
      TreeNodeLabel* child = expand(game, child_key, agent);
      parent->add_child(child, i);
      return child->get_q();
    }
    return 0;
  }
  int best_child = parent->get_best_child(agent.use_NN_predict);
  game.drop_piece_in_column(best_child);
  path.push_back(best_child);
  return select(game, children[best_child], path, agent);
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

void backup_value(HashMapTree* tree, Key &key, TreeNodeLabel* &parent, TreeNodeLabel* child, int column, double value) {
  if (parent == NULL) {
    parent = tree->add_node(key);
  }
  parent->add_visit(child, column, value);
}

void backup_simulation(Connect4 &game, HashMapTree* tree, vector<int> &path, double value) {
  int move = -1;
  TreeNodeLabel* child = NULL;
  for (vector<int>::reverse_iterator i = path.rbegin(); i != path.rend(); ++i ) { 
    Key key = game.get_board();
    game.retract_piece_in_column(*i);
    TreeNodeLabel* parent = tree->get_node_label(key);
    backup_value(tree, key, parent, child, move, value);
    value = -value;
    move = *i;
    child = parent;
  }
  Key key = game.get_board();
  TreeNodeLabel* parent = tree->get_node_label(key);
  backup_value(tree, key, parent, child, move, value);
}

void simulate(Connect4 &game, MCTSAgent &agent) {
  vector<int> path;
  HashMapTree* tree = agent.get_tree();
  double value = select(game, tree->get_root(), path, agent);
  // if (agent._iteration_nr == 199) {
  //   game.print_board();
  // }
  if (!agent.use_NN_predict) {
    value = playout(game, path); 
  }
  backup_simulation(game, tree, path, value);
}
