using namespace std;

#include "../Connect4-Game/Game.h"
#include "MCTSAgent.h"

const double C = 0.2;

TreeNodeLabel* expand(Connect4 &game, Key &key, MCTSAgent &agent) {
  HashMapTree* tree = agent.get_tree();
  TreeNodeLabel* node = tree->add_node(key);
  vector<Key> states { key };
  vector<TreeNodeLabel*> nodes { node };

  int turn = game.get_to_move();
  agent.call_predict(states, nodes, turn);

  return node;
}

double game_final_score(Connect4 &game) {
  if (game.winning_move()) {
    return 1;
  }
  return 0.5;
}

double playout (Connect4 &game, vector<int> &path) {
  int player = game.get_to_move();
  while (!game.is_terminal_state()) {
    vector<int> moves = game.get_valid_columns();
    int random_move = rand() % moves.size();
    game.drop_piece_in_column(moves[random_move]);
    path.push_back(moves[random_move]);
  }
  return game_final_score(game);
}

double UCT(TreeNodeLabel* parent, TreeNodeLabel* child, int index) {
  if (child->get_n() == 0) return numeric_limits<double>::max();
  return child->get_q() + C * (sqrt(log(parent->get_n()) / child->get_n()));
}

double PUCT(TreeNodeLabel* parent, TreeNodeLabel* child, int index) {
  if (child->get_n() == 0) return numeric_limits<double>::max();
  return child->get_q() + parent->get_p()[index] * C * (sqrt(log(parent->get_n()) / child->get_n()));
}

// Choose random child when there are more than one best child
int choose_index(double (&values)[COLUMNS], int best_value_index) {
  vector<int> available_columns;
  for (int i = 0; i < COLUMNS; i++) {
    if (values[i] >= values[best_value_index]) {
      available_columns.push_back(i);
    }
  }
  int random_move = rand() % available_columns.size();
  return available_columns[random_move];
}

int select_best_child(TreeNodeLabel* parent, TreeNodeLabel* (&children)[COLUMNS], bool use_PUCT) {
  double best_child_value = numeric_limits<double>::lowest();
  double child_values[COLUMNS];
  int index = -1;

  for (int i = 0; i < COLUMNS; i++) {
    if (children[i] == NULL) {
      child_values[i] = numeric_limits<double>::lowest();
      continue;
    }
    double child_value = use_PUCT ? PUCT(parent, children[i], i) : UCT(parent, children[i], i);
    child_values[i] = child_value;
    if (child_value > best_child_value) {
      best_child_value = child_value;
      index = i;
    }
  }
  
  return choose_index(child_values, index);
}

double traverse(Connect4 &game, TreeNodeLabel *parent, vector<int> &path, MCTSAgent &agent) {
  HashMapTree* tree = agent.get_tree();
  TreeNodeLabel* nodes[COLUMNS];
  for (int i = 0; i < COLUMNS; i++) nodes[i] = NULL;

  vector<int> available_moves = game.get_valid_columns();

  for (int m : available_moves) {
    game.drop_piece_in_column(m);
    Key child_key = game.get_board();
    TreeNodeLabel* child_node = tree->get_node_label(child_key);
    if (child_node == NULL || child_node->get_n() == 0) {
      path.push_back(m);
      if (agent.use_NN_predict) {
        child_node = expand(game, child_key, agent);
        return game.is_terminal_state() ? game_final_score(game) : child_node->get_q();
      }
      return playout(game, path);
    }
    game.retract_piece_in_column(m);
    nodes[m] = child_node;
  }
  int best_child = select_best_child(parent, nodes, agent.use_NN_predict);
  game.drop_piece_in_column(best_child);
  path.push_back(best_child);
  return game.is_terminal_state() ? game_final_score(game) : traverse(game, nodes[best_child], path, agent);
}

void backup_value(HashMapTree* tree, Key &key, double value) {
  TreeNodeLabel* node = tree->add_node(key);
  node->add_visit(value);
}

void backup_simulation(Connect4 &game, HashMapTree* tree, vector<int> &path, double value) {
  for (vector<int>::reverse_iterator i = path.rbegin(); i != path.rend(); ++i ) { 
    Key key = game.get_board();
    game.retract_piece_in_column(*i);
    backup_value(tree, key, value);
    value = -value;
  }
  Key key = game.get_board();
  backup_value(tree, key, value);
}

void simulate(Connect4 &game, MCTSAgent &agent) {
  vector<int> path;
  HashMapTree* tree = agent.get_tree();
  double value = traverse(game, tree->get_root(), path, agent);
  backup_simulation(game, tree, path, value);
}
