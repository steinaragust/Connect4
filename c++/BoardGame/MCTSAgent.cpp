#include "MCTSAgent.h"

void simulate(BoardGame &game, MCTSAgent &agent);
TreeNodeLabel* expand(BoardGame &game, TreeNodeLabel* node,  MCTSAgent &agent);

MCTSAgent::MCTSAgent(string name, int iterations, function<void(double**, int ***, int, int)> NN_predict) {
  _name = name;
  _iterations = iterations;
  _tree = HashMapTree();
  use_NN_predict = false;
  _iteration_nr = 0;
  _predict = NN_predict;
  use_NN_predict = NN_predict != nullptr;
  _can_win = false;
}

MCTSAgent::~MCTSAgent() {
}

string MCTSAgent::get_name() {
  return _name;
}

HashMapTree* MCTSAgent::get_tree() {
  return &_tree;
}

void MCTSAgent::set_root(BoardGame &game) {
  Key root_key = game.get_board();
  TreeNodeLabel* root = _tree.set_root(root_key);
  if (use_NN_predict) {
    int turn = game.get_to_move();
    vector<Key> states { root_key };
    vector<TreeNodeLabel*> nodes { root };
    call_predict(states, nodes, turn);
    root->print_p();
  }
}

void MCTSAgent::reset() {
  _iteration_nr = 0;
  _tree.clear_map();
}

IterationValue MCTSAgent::get_return_value(BoardGame &game, bool random_move) {
  int total = 0;
  string move = "";
  int available_moves = 0;
  double *q_values = new double[game.priors_arr_size];
  int *n_values = new int[game.priors_arr_size];
  double *policy = new double[game.priors_arr_size];
  double q_value = _tree.get_root()->get_q();

  for (int i = 0; game.priors_arr_size; i++) {
    q_values[i] = numeric_limits<double>::lowest();
    n_values[i] = numeric_limits<int>::min();
    policy[i] = 0.0;
  }
  vector<string> moves = game.get_valid_moves();

  for (string m : moves) {
    game.make_move(m);
    int p_index = game.get_prior_index(m);
    Key key = game.get_board();
    TreeNodeLabel* child = _tree.get_node_label(key);
    total += n_values[p_index];
    available_moves += 1;
    n_values[p_index] = child->get_n();
    q_values[p_index] = child ->get_q();
    game.retract_move(m);
  }

  int max_visits = numeric_limits<int>::min();

  for (string m : moves) {
    int p_index = game.get_prior_index(m);
    policy[p_index] = (double)n_values[p_index] / (double)total;
    if (n_values[p_index] > max_visits) {
      max_visits = n_values[p_index];
      move = m;
    }
  }

  if (random_move) {
    int chosen = rand() % available_moves;
    move = moves[chosen];
  }
  return {move, q_value, q_values, n_values, policy};
}

void MCTSAgent::print_iteration_value(IterationValue &value) {
  printf("Column: %d\n", value.column);
  printf("q value: %lf\n", value.q_value);
  printf("n values: ");
  for (int i = 0; i < COLUMNS; i++) {
    if (value.n_values[i] > numeric_limits<int>::min()) {
      printf("%d ", value.n_values[i]);
    } else {
      printf("NULL ");
    }
  }
  printf("\n");
  printf("q values: ");
  for (int i = 0; i < COLUMNS; i++) {
    if (value.n_values[i] > numeric_limits<int>::min()) {
      printf("%lf ", value.q_values[i]);
    } else {
      printf("NULL ");
    }
  }
  printf("\n");
  printf("policy values: ");
  for (int i = 0; i < COLUMNS; i++) {
    if (value.n_values[i] > numeric_limits<int>::min()) {
      printf("%lf ", value.policy[i]);
    } else {
      printf("NULL ");
    }
  }
  printf("\n\n");
}

void MCTSAgent::can_win_now(BoardGame &game) {
  vector<string> moves = game.get_valid_moves();
  for (string m : moves) {
    game.make_move(m);
    bool winning_move = game.winning_move();
    game.retract_move(m);
    if (winning_move) {
      _can_win = true;
      return;
    }
  }
  _can_win = false;
}

IterationValue MCTSAgent::play(Connect4 game, bool random_move) {
  reset();
  set_root(game);
  _nr_moves_so_far = game.get_move_no();
  for ( ;_iteration_nr < _iterations; _iteration_nr += 1) {
    simulate(game, *this);
  }
  IterationValue return_value = get_return_value(game, random_move);
  
  return return_value;
}

void MCTSAgent::set_predict(function<void(double**, int ***, int, int)> f) {
  _predict = f;
  use_NN_predict = true;
}

void MCTSAgent::fill_states(vector<Key> &states, int ***_states) {
  for (int s = 0; s < states.size(); s++) {
    for (int r = 0; r < ROWS; r++) {
      for (int c = 0; c < COLUMNS; c++) {
        _states[s][r][c] = states[s][r][c];
      }
    }
  }
}

void MCTSAgent::free_states(int ***states, int length) {
    for (int s = 0; s < length; s++) {
    for (int r = 0; r < ROWS; r++) {
      delete[] states[s][r];
    }
    delete[] states[s];
  }
  delete[] states;
}

void MCTSAgent::call_predict(vector<Key> &states, vector<TreeNodeLabel*> &nodes, int turn) {
  double ** values = new double*[states.size()];
   for (int s = 0; s < states.size(); s++) {
    values[s] = new double[COLUMNS + 1];
  }

  int*** _states = new int**[states.size()];
  for (int s = 0; s < states.size(); s++) {
    _states[s] = new int*[ROWS];
    for (int r = 0; r < ROWS; r++) {
      _states[s][r] = new int[COLUMNS];
    }
  }

  fill_states(states, _states);
  _predict(values, _states, turn, states.size());

  for (int i = 0; i < states.size(); i++) {
    nodes[i]->set_p(values[i]);
    nodes[i]->set_q(values[i][7]);
    delete[] values[i];
  }
  free_states(_states, states.size());
  delete [] values;

  return;
}
