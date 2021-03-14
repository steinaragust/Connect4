using namespace std;

#include "MCTSAgent.h"

const double C = 0.2;

inline TreeNodeLabel* expand(BoardGame &game, Key &key, MCTSAgent &agent) {
  TreeNodeLabel* node = agent.get_tree()->add_node(key);
  int turn = game.get_to_move();
  vector<Key> states { key };
  vector<TreeNodeLabel*> nodes { node };
  vector<int> turns { turn };

  agent.call_predict(states, turns, nodes);

  return node;
}

inline double game_final_score(BoardGame &game) {
  if (game.winning_move()) {
    return 1;
  }
  return 0.5;
}

inline double playout (BoardGame &game, vector<int> &path) {
  while (!game.is_terminal_state()) {
    vector<int> moves = game.get_valid_moves();
    int random_move = rand() % moves.size();
    game.make_move(moves[random_move]);
    path.push_back(moves[random_move]);
  }
  return game_final_score(game);
}

inline double UCT(TreeNodeLabel* parent, TreeNodeLabel* child, int index) {
  if (child->get_n() == 0) return numeric_limits<double>::max();
  return child->get_q() + C * (sqrt(log(parent->get_n()) / child->get_n()));
}

inline double PUCT(TreeNodeLabel* parent, TreeNodeLabel* child, int index) {
  if (child->get_n() == 0) return numeric_limits<double>::max();
  return child->get_q() + child->get_p()[index] * C * (sqrt(log(parent->get_n()) / child->get_n()));
}

inline double calculate_child_value(TreeNodeLabel* parent, TreeNodeLabel* child, int index, bool use_PUCT) {
  return use_PUCT ? PUCT(parent, child, index) : UCT(parent, child, index);
}

// Choose random child if there are more than one best child
inline int select_best_child(vector<double> children_values, int best_index) {
  vector<int> available_columns;
  for (int i = 0; i < children_values.size(); i++) {
    if (children_values[i] >= children_values[best_index]) {
      available_columns.push_back(i);
    }
  }
  int random_move = rand() % available_columns.size();
  return available_columns[random_move];
}

inline double traverse(BoardGame &game, TreeNodeLabel *parent_node, MCTSAgent &agent, vector<int> &path) {
  vector<int> available_moves = game.get_valid_moves();
  vector<TreeNodeLabel*> children_nodes;
  vector<double> children_values;
  int best_index = -1;
  double best_child_value = numeric_limits<double>::lowest();

  for (int i = 0; i < available_moves.size(); i++) {
    int m = available_moves[i];
    game.make_move(m);
    Key child_key = game.get_board();
    TreeNodeLabel* child_node = agent.get_tree()->get_node_label(child_key);
    if (child_node == NULL || child_node->get_n() == 0) {
      path.push_back(m);
      if (agent.use_NN_predict) {
        child_node = expand(game, child_key, agent);
        return game.is_terminal_state() ? game_final_score(game) : child_node->get_q();
      }
      return playout(game, path);
    } else {
      double child_value = calculate_child_value(parent_node, child_node, game.get_prior_index(m), agent.use_NN_predict);
      if (child_value > best_child_value) {
        best_index = i;
        best_child_value = child_value;
      }
      children_nodes.push_back(child_node);
      children_values.push_back(child_value);
      game.retract_move(m);
    }
  }
  best_index = select_best_child(children_values, best_index);

  game.make_move(available_moves[best_index]);
  path.push_back(available_moves[best_index]);
  return game.is_terminal_state() ? game_final_score(game) : traverse(game, children_nodes[best_index], agent, path);
}

inline void backup_value(HashMapTree* tree, Key &key, double value) {
  TreeNodeLabel* node = tree->add_node(key);
  node->add_visit(value);
}

inline void backup_simulation(BoardGame &game, HashMapTree* tree, vector<int> &path, double value) {
  for (vector<int>::reverse_iterator i = path.rbegin(); i != path.rend(); ++i ) { 
    Key key = game.get_board();
    backup_value(tree, key, value);
    game.retract_move(*i);
    value = -value;
  }
  Key key = game.get_board();
  backup_value(tree, key, value);
}

inline void simulate(BoardGame &game, MCTSAgent &agent) {
  vector<int> path;
  HashMapTree* tree = agent.get_tree();
  double value = traverse(game, tree->get_root(), agent, path);
  backup_simulation(game, tree, path, value);
}
