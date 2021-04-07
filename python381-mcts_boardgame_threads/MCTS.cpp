using namespace std;

#include "MCTSAgent.h"
#include <algorithm>

const double C = 0.2;

inline double PUCT(TreeNodeLabel* parent, TreeNodeLabel* child, int index) {
  if (child->get_n() == 0) return numeric_limits<double>::max();
  return child->get_q() + parent->get_p()[index] * C * (sqrt(log(parent->get_n()) / child->get_n()));
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

inline double game_final_score(BoardGame &game) {
  if (game.winning_move()) {
    return 1;
  }
  return 0.5;
}

void traverse(BoardGame *game, TreeNodeLabel *parent_node, MCTSAgent *agent, vector<TreeNodeLabel*> &path) {
  vector<int> available_moves = game->get_valid_moves();
  vector<TreeNodeLabel*> children_nodes;
  vector<double> children_values;
  int best_index = -1;
  double best_child_value = numeric_limits<double>::lowest();

  for (int i = 0; i < available_moves.size(); i++) {
    int m = available_moves[i];
    game->make_move(m);
    Key child_key = game->get_board();
    HashMapTree *tree = agent->get_tree();
    TreeNodeLabel* child_node = tree->get_node_label(child_key);
    if (child_node == NULL) {
      child_node = tree->add_node(child_key);
      child_node->add_visit();
      path.push_back(child_node);
      return;
    } else {
      double child_value = PUCT(parent_node, child_node, game->get_prior_index(m));
      if (child_value > best_child_value) {
        best_index = i;
        best_child_value = child_value;
      }
      children_nodes.push_back(child_node);
      children_values.push_back(child_value);
      game->retract_move(m);
    }
  }
  best_index = select_best_child(children_values, best_index);
  game->make_move(available_moves[best_index]);
  TreeNodeLabel* chosen = children_nodes[best_index];
  chosen->add_visit();
  path.push_back(chosen);
  if (!game->is_terminal_state()) traverse(game, chosen, agent, path);
}

void traverse_thread(int nr, BoardGame *game, MCTSAgent *agent) {
  //Traverse
  HashMapTree* tree = agent->get_tree();
  vector<TreeNodeLabel*> path;
  TreeNodeLabel* root = tree->get_root();
  root->add_visit();
  path.push_back(root);
  traverse(game, root, agent, path);
  agent->fill_buffer(nr, game);
}

void backup_thread(BoardGame *game, HashMapTree *tree) {
  printf("backup thread");
  double score;
  if (!game->is_terminal_state()) {
    Key key = game->get_board();
    TreeNodeLabel *node = tree->get_node_label(key);
    double *values = node->get_p();
    score = values[game->info.priors_arr_size];
  } else {
    score = game_final_score(*game);
  }
  while(!game->is_terminal_state()) {
    Key key = game->get_board();
    TreeNodeLabel *node = tree->get_node_label(key);
    node->backup_value(score);
    score = -score;
  }
}

void simulate_threads(BoardGame &game, MCTSAgent &agent) {
  int nr_of_threads = agent.next_batch_simulations();
  vector<thread> threads;
  vector<BoardGame*> games;
  for (int i = 0; i < nr_of_threads; i++) {
    BoardGame *game_copy = game.get_copy();
    threads.push_back(thread(traverse_thread, i, game_copy, &agent));
    games.push_back(game_copy);
  }
  for (thread &t : threads) t.join();
  threads.clear();
  agent.call_predict();
  HashMapTree *tree = agent.get_tree();
  for (int i = 0; i < nr_of_threads; i++) {
    threads.push_back(thread(backup_thread, games[i], tree));
  }
  for (thread &t : threads) t.join();
  for (BoardGame *g : games) delete g;
  agent._simulation_nr += nr_of_threads;
}
