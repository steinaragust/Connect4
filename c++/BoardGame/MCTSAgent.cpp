#include "MCTSAgent.h"

void simulate(BoardGame &game, MCTSAgent &agent);
TreeNodeLabel* expand(BoardGame &game, TreeNodeLabel* node,  MCTSAgent &agent);

MCTSAgent::MCTSAgent(GameInfo game_info, string name, int iterations, function<void(double**, vector<int**>, vector<int>)> NN_predict) {
  _name = name;
  _iterations = iterations;
  _game_info = game_info;
  _tree = HashMapTree(game_info);
  _predict = NN_predict;
  use_NN_predict = NN_predict != nullptr;
  _latest_iteration_value = new IterationValue(_game_info.priors_arr_size);
}

MCTSAgent::~MCTSAgent() {
  _tree.clear_map();
  _latest_iteration_value->~IterationValue();
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
    vector <int> turns = { turn };
    call_predict(states, turns, nodes);
    root->print_p(_game_info.priors_arr_size);
  }
}

void MCTSAgent::reset(BoardGame &game) {
  _tree.clear_map();
  set_root(game);
  _iteration_nr = 0;
  _nr_moves_so_far = game.get_move_no();
  _can_win = false;
}

IterationValue* MCTSAgent::play(BoardGame &game, bool random_move) {
  reset(game);
  for ( ;_iteration_nr < _iterations; _iteration_nr += 1) {
    simulate(game, *this);
  }
  IterationValue* return_value = get_return_value(game, random_move);
  
  return return_value;
}

IterationValue* MCTSAgent::get_return_value(BoardGame &game, bool random_move) {
  int total = 0;
  int move = -1;
  int available_moves = 0;
  _latest_iteration_value->q_value = _tree.get_root()->get_q();

  for (int i = 0; _game_info.priors_arr_size; i++) {
    _latest_iteration_value->q_values[i] = numeric_limits<double>::lowest();
    _latest_iteration_value->n_values[i] = numeric_limits<int>::min();
    _latest_iteration_value->policy[i] = 0.0;
  }
  vector<int> moves = game.get_valid_moves();

  for (int m : moves) {
    game.make_move(m);
    int p_index = game.get_prior_index(m);
    Key key = game.get_board();
    TreeNodeLabel* child = _tree.get_node_label(key);
    _latest_iteration_value->n_values[p_index] = child->get_n();
    _latest_iteration_value->q_values[p_index] = child ->get_q();
    game.retract_move(m);
    total += _latest_iteration_value->n_values[p_index];
    available_moves += 1;
  }

  int max_visits = numeric_limits<int>::min();

  for (int m : moves) {
    int p_index = game.get_prior_index(m);
    _latest_iteration_value->policy[p_index] = (double)_latest_iteration_value->n_values[p_index] / (double)total;
    if (_latest_iteration_value->n_values[p_index] > max_visits) {
      max_visits = _latest_iteration_value->n_values[p_index];
      move = m;
    }
  }

  if (random_move) {
    int chosen = rand() % available_moves;
    move = moves[chosen];
  }
  return _latest_iteration_value;
}

void MCTSAgent::print_iteration_value() {
  printf("Move: %d\n", _latest_iteration_value->move);
  printf("q value: %lf\n", _latest_iteration_value->q_value);
  printf("n values: ");
  for (int i = 0; i < _game_info.priors_arr_size; i++) {
    if (_latest_iteration_value->n_values[i] > numeric_limits<int>::min()) {
      printf("%d ", _latest_iteration_value->n_values[i]);
    } else {
      printf("NULL ");
    }
  }
  printf("\n");
  printf("q values: ");
  for (int i = 0; i < _game_info.priors_arr_size; i++) {
    if (_latest_iteration_value->n_values[i] > numeric_limits<int>::min()) {
      printf("%lf ", _latest_iteration_value->q_values[i]);
    } else {
      printf("NULL ");
    }
  }
  printf("\n");
  printf("policy values: ");
  for (int i = 0; i < _game_info.priors_arr_size; i++) {
    if (_latest_iteration_value->n_values[i] > numeric_limits<int>::min()) {
      printf("%lf ", _latest_iteration_value->policy[i]);
    } else {
      printf("NULL ");
    }
  }
  printf("\n\n");
}

void MCTSAgent::set_predict(function<void(double**, vector<int**>, vector<int>)> f) {
  _predict = f;
  use_NN_predict = true;
}

void MCTSAgent::call_predict(vector<Key> &states, vector<int>& turns, vector<TreeNodeLabel*> &nodes) {
  double ** values = new double*[states.size()];
   for (int s = 0; s < states.size(); s++) {
    values[s] = new double[_game_info.priors_arr_size + 1];
  }

  _predict(values, states, turns);

  for (int i = 0; i < states.size(); i++) {
    nodes[i]->set_p(values[i]);
    // q-value is last index
    nodes[i]->set_q(values[i][_game_info.priors_arr_size]);
  }
  return;
}

void MCTSAgent::can_win_now(BoardGame &game) {
  vector<int> moves = game.get_valid_moves();
  for (int m : moves) {
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
