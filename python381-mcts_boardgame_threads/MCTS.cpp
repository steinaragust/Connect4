using namespace std;

#include "MCTSAgent.h"

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

double traverse(BoardGame &game, TreeNodeLabel *parent_node, HashMapTree* tree, vector<TreeNodeLabel*> &path) {
  vector<int> available_moves = game.get_valid_moves();
  vector<TreeNodeLabel*> children_nodes;
  vector<double> children_values;
  int best_index = -1;
  double best_child_value = numeric_limits<double>::lowest();

  for (int i = 0; i < available_moves.size(); i++) {
    int m = available_moves[i];
    game.make_move(m);
    Key child_key = game.get_board();
    TreeNodeLabel* child_node = tree->get_node_label(child_key);
    if (child_node == NULL) {
      // KALLA Í NN
      child_node = tree->add_node(child_key);
      child_node->add_visit();
      path.push_back(child_node);
      return 1;
    } else {
      double child_value = PUCT(parent_node, child_node, game.get_prior_index(m));
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
  TreeNodeLabel* chosen = children_nodes[best_index];
  path.push_back(chosen);
  return game.is_terminal_state() ? game_final_score(game) : traverse(game, chosen, tree, path);
}

void simulate_thread(int nr, BoardGame &game, MCTSAgent &agent) {
  HashMapTree* tree = agent.get_tree();
  vector<TreeNodeLabel*> path;
  TreeNodeLabel* root = tree->get_root();
  path.push_back(root);
  double value = traverse(game, root, tree, path);
  for (TreeNodeLabel* node : path) {
    node->backup_value(value);
  }
  delete &game;
}

void predict_thread(MCTSAgent &agent) {
  unique_lock<mutex> lk(agent.worker_m);
  wait(lk, []{ return agent._thread_counter == agent._state_buffer_size });
  // Kalla og fylla í NN
  agent._thread_counter = 0;
  lk.unlock();
  agent.worker_cv.notify_all();
}

void simulate_threads(BoardGame &game, MCTSAgent &agent) {
  int nr_of_threads = agent.next_batch_simulations();
  vector<thread> threads;
  for (int i = 0; i < nr_of_threads; i++) {
    BoardGame *game_copy = game.get_copy();
    threads.push_back(thread(simulate_thread, i, &game_copy, agent));
  }
  for (thread t : threads) {
    t.join();
  }
  agent._simulation_nr += nr_of_threads;
}
