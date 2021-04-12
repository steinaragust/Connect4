#include "MCTSAgent.h"
#include "MCTS.cpp"

inline MCTSAgent::MCTSAgent(GameInfo info, string name, int simulations, double max_time_seconds, int nr_threads, bool use_NN_predict) {
  _name = name;
  _info = info;
  _simulations = simulations;
  _max_time_seconds = max_time_seconds;
  _tree = new HashMapTree(info);
  _latest_iteration_value = new IterationValue(info.priors_arr_size);
  _nr_threads = nr_threads;
  _use_NN_predict = use_NN_predict;
}

inline MCTSAgent::~MCTSAgent() {
  _tree->clear_map();
  // delete _tree;
  _latest_iteration_value->~IterationValue();
}

inline void MCTSAgent::set_name(string name) {
  _name = name;
}

inline string MCTSAgent::get_name() {
  return _name;
}

inline HashMapTree* MCTSAgent::get_tree() {
  return _tree;
}

inline void MCTSAgent::set_root(BoardGame &game) {
  Key root_key = game.get_board();
  TreeNodeLabel* root = _tree->set_root(root_key);
  if (_use_NN_predict) {
    root->add_visit();
    int turn = game.get_to_move();
    _states_buffer.push_back(root_key);
    _turns_buffer.push_back(turn);
    call_predict();
    root->backup_value(root->get_p()[_info.priors_arr_size]); 
  }
}

inline void MCTSAgent::reset(BoardGame &game) {
  _simulation_nr = 0;
  _states_predicted = 0;
  next_batch_simulations();
  _tree->clear_map();
  set_root(game);
  _nr_moves_so_far = game.get_move_no();
}

inline IterationValue* MCTSAgent::get_return_value(BoardGame &game, bool random_move) {
  int total = 0;
  int move = -1;
  int available_moves = 0;
  TreeNodeLabel* root = _tree->get_root();
  _latest_iteration_value->q_value = _tree->get_root()->get_q();
  _latest_iteration_value->simulations = _simulation_nr;
  _latest_iteration_value->states_predicted = _states_predicted;
  for (int i = 0; i < _info.priors_arr_size; i++) {
    _latest_iteration_value->q_values[i] = numeric_limits<double>::lowest();
    _latest_iteration_value->n_values[i] = numeric_limits<int>::min();
    _latest_iteration_value->policy[i] = 0.0;
  }
  vector<int> moves = game.get_valid_moves();
  for (int m : moves) {
    game.make_move(m);
    int p_index = game.get_prior_index(m);
    Key key = game.get_board();
    TreeNodeLabel* child = _tree->get_node_label(key);
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
  _latest_iteration_value->move = move;
  return _latest_iteration_value;
}

inline void MCTSAgent::print_iteration_value() {
  printf("Move: %d\n", _latest_iteration_value->move);
  printf("Simulations: %d\n", _latest_iteration_value->simulations);
  printf("Time elapsed: %lf\n", _latest_iteration_value->time_elapsed);
  printf("q value: %lf\n", _latest_iteration_value->q_value);
  printf("n values: ");
  for (int i = 0; i < _info.priors_arr_size; i++) {
    if (_latest_iteration_value->n_values[i] > numeric_limits<int>::min()) {
      printf("%d ", _latest_iteration_value->n_values[i]);
    } else {
      printf("NULL ");
    }
  }
  printf("\n");
  printf("q values: ");
  for (int i = 0; i < _info.priors_arr_size; i++) {
    if (_latest_iteration_value->n_values[i] > numeric_limits<int>::min()) {
      printf("%lf ", _latest_iteration_value->q_values[i]);
    } else {
      printf("NULL ");
    }
  }
  printf("\n");
  printf("policy values: ");
  for (int i = 0; i < _info.priors_arr_size; i++) {
    if (_latest_iteration_value->n_values[i] > numeric_limits<int>::min()) {
      printf("%lf ", _latest_iteration_value->policy[i]);
    } else {
      printf("NULL ");
    }
  }
  printf("\n\n");
}

inline IterationValue* MCTSAgent::play(BoardGame &game, bool random_move) {
  reset(game);
  Timer timer;
  timer.start();
  while (true) {
    if (
      (_simulations != 0 && _simulation_nr >= _simulations) ||
      (_max_time_seconds != 0 && timer.elapsedSeconds() >= _max_time_seconds)
    ) {
      break;
    }
    simulate_threads(game, *this);
  }
  _latest_iteration_value->time_elapsed = timer.elapsedSeconds();
  timer.stop();
  IterationValue* return_value = get_return_value(game, random_move);
  return return_value;
}

inline int MCTSAgent::can_win_now(BoardGame &game) {
  vector<int> moves = game.get_valid_moves();
  for (int m : moves) {
    game.make_move(m);
    bool winning_move = game.winning_move();
    game.retract_move(m);
    if (winning_move) {
      return m;
    }
  }
  return -1;
}

inline int MCTSAgent::next_batch_simulations() {
  _states_buffer.clear();
  _turns_buffer.clear();
  if (_simulations == 0 || _simulations > _simulation_nr + _nr_threads) {
    return _nr_threads;
  }
  return _simulations - _simulation_nr;
}

inline void MCTSAgent::call_predict() {
  _states_predicted += _states_buffer.size();
  double ** values = new double*[_states_buffer.size()];
  for (int i = 0; i < _states_buffer.size(); i++) {
    values[i] = new double[_info.priors_arr_size + 1];
  }
  predict_states(values, _states_buffer, _turns_buffer);
  for (int i = 0; i < _states_buffer.size(); i++) {
    TreeNodeLabel *node = _tree->get_node_label(_states_buffer[i]);
    node->set_p(values[i]);
  }
}

inline void MCTSAgent::fill_buffer(int thread_nr, BoardGame* game) {
  vector_m.lock();
  Key state = game->get_board();
  int turn = game->get_to_move();
  _states_buffer.push_back(state);
  _turns_buffer.push_back(turn);
  vector_m.unlock();
}

inline void MCTSAgent::set_NN_predict(bool value) {
  _use_NN_predict = value;
}

inline void MCTSAgent::set_max_time_seconds(double max_time_seconds) {
  _max_time_seconds = max_time_seconds;
}
