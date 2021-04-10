using namespace std;

#include "MCTSAgent.h"
#include <algorithm>

const double C = 0.2;

inline double PUCT(TreeNodeLabel* parent, TreeNodeLabel* child, int index) {
  if (child->get_n() == 0) return numeric_limits<double>::max();
  double value = child->get_q() + parent->get_p()[index] * C * (sqrt(log(parent->get_n()) / child->get_n()));
  return value;
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


// Returns true if simulation should be discarded
bool traverse(int nr, BoardGame *game, TreeNodeLabel *parent_node, MCTSAgent *agent, vector<TreeNodeLabel*> &path) {
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
    TreeNodeLabel* child_node = tree->add_node_if_not_exists(child_key);
    // If node is added
    if (child_node != NULL) {
      child_node->add_visit();
      path.push_back(child_node);
      agent->fill_buffer(nr, game);
      return false;
    } else {
      child_node = tree->get_node_label(child_key);
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
  // Discard simulation
  for (TreeNodeLabel* child : children_nodes) {
    if (child->get_p() == NULL) {
      return true;
    }
  }
  best_index = select_best_child(children_values, best_index);
  game->make_move(available_moves[best_index]);
  TreeNodeLabel* chosen = children_nodes[best_index];
  chosen->add_visit();
  path.push_back(chosen);
  if (!game->is_terminal_state()) {
    return traverse(nr, game, chosen, agent, path);
  }
  // Found terminal state
  return false;
}

void traverse_thread(int nr, BoardGame *game, MCTSAgent *agent, vector<TreeNodeLabel*> &path) {
  //Traverse
  HashMapTree* tree = agent->get_tree();
  TreeNodeLabel* root = tree->get_root();
  root->add_visit();
  path.push_back(root);
  bool discard_thread = traverse(nr, game, root, agent, path);
  if (discard_thread) {
    for (TreeNodeLabel* node : path) {
      node->retract_visit();
    }
    path.clear();
  }
}

void backup_thread(BoardGame *game, HashMapTree *tree, vector<TreeNodeLabel*> &path) {
  double score;
  if (game->is_terminal_state()) {
    score = game_final_score(*game); 
  } else {
    double *values = path.back()->get_p();
    // Value for state is always last index
    score = values[game->info.priors_arr_size];
  }
  reverse(path.begin(), path.end());
  for (TreeNodeLabel* node : path) {
    node->backup_value(score);
    score = -score;
  }
}

void simulate_threads(BoardGame &game, MCTSAgent &agent) {
  int nr_of_threads = agent.next_batch_simulations();
  vector<thread> threads;
  vector<BoardGame*> games;
  vector<vector<TreeNodeLabel*>> paths(nr_of_threads, vector<TreeNodeLabel*>());
  for (int i = 0; i < nr_of_threads; i++) {
    BoardGame *game_copy = game.get_copy();
    threads.push_back(thread(traverse_thread, i, game_copy, &agent, ref(paths[i])));
    games.push_back(game_copy);
  }
  for (thread &t : threads) {
    t.join();
  }
  threads.clear();
  agent.call_predict();
  HashMapTree *tree = agent.get_tree();
  int nr_simulations_done = 0;
  for (int i = 0; i < nr_of_threads; i++) {
    if (paths[i].size() > 0) {
      threads.push_back(thread(backup_thread, games[i], tree, ref(paths[i])));
      nr_simulations_done += 1;
    }
  }
  for (thread &t : threads) {
    t.join();
  }
  for (BoardGame* game : games) delete game;
  agent._simulation_nr += nr_simulations_done;
}
