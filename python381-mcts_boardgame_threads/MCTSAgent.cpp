#include "MCTSAgent.h"
#include "MCTS.cpp"

inline MCTSAgent::MCTSAgent(GameInfo info, string name, int simulations, int nr_threads) {
  _name = name;
  _info = info;
  _simulations = simulations;
  _tree = HashMapTree(info);
  _latest_iteration_value = new IterationValue(info.priors_arr_size);
  _nr_threads = nr_threads;
  _state_buffer = new Key[nr_threads];
  _turns_buffer = new int[nr_threads];
  _state_values_buffer = new double*[nr_threads];
  thread _predict_worker_thread(predict_worker);
}

inline MCTSAgent::~MCTSAgent() {
  _tree.clear_map();
  _latest_iteration_value->~IterationValue();
  _predict_worker_thread.detach();
  delete[] _state_buffer;
  delete _turns_buffer;
  delete[] _state_values_buffer;
}

inline void MCTSAgent::set_name(string name) {
  _name = name;
}

inline string MCTSAgent::get_name() {
  return _name;
}

inline HashMapTree* MCTSAgent::get_tree() {
  return &_tree;
}

inline void MCTSAgent::set_root(BoardGame &game) {
  Key root_key = game.get_board();
  TreeNodeLabel* root = _tree.set_root(root_key);
  root->add_visit();
  int turn = game.get_to_move();
  _state_buffer[0] = root_key;
  _turns_buffer[0] = turn;
  call_predict();
  root->backup_value(root->get_p()[_info.priors_arr_size]);
}

inline void MCTSAgent::reset(BoardGame &game) {
  _simulation_nr = 0;
  _thread_counter = 0;
  next_batch_simulations();
  _tree.clear_map();
  set_root(game);
  _nr_moves_so_far = game.get_move_no();
}

inline IterationValue* MCTSAgent::play(BoardGame &game, bool random_move) {
  reset(game);
  while (_simulation_nr < _simulations) {
    simulate_threads(game, *this);
  }
  IterationValue* return_value = get_return_value(game, random_move);
  return return_value;
}

inline IterationValue* MCTSAgent::get_return_value(BoardGame &game, bool random_move) {
  int total = 0;
  int move = -1;
  int available_moves = 0;
  TreeNodeLabel* root = _tree.get_root();
  _latest_iteration_value->q_value = _tree.get_root()->get_q();
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
  _latest_iteration_value->move = move;
  return _latest_iteration_value;
}

inline void MCTSAgent::print_iteration_value() {
  printf("Move: %d\n", _latest_iteration_value->move);
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

inline void MCTSAgent::call_predict() {
  for (int i = 0; i < _nr_threads; i++) {
    if (_state_buffer != NULL) {
      _state_values_buffer[i] = new double[_info.priors_arr_size + 1];
    }
  }
  predict();
  for (int i = 0; i < _nr_threads; i++) {
    if (_state_buffer[i] != NULL) {
      TreeNodeLabel* node = _tree.get_node_label(_state_buffer[i]);
      node->set_p(_state_values_buffer[i]);
    }
  }
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
  for (int i = 0; i < _nr_threads; i++) {
    _state_buffer[i] = NULL;
    _turns_buffer[i] = -1;
    _state_values_buffer = NULL;
  }
  if (_simulations - _simulation_nr < _nr_threads) {
    return _simulations - _simulation_nr;
  }
  return _nr_threads;
}

inline void MCTSAgent::get_thread_ready_and_wait(int nr, Key& key, int turn) {
  {
    lock_guard<mutex> lk(worker_m);
    _state_buffer[nr] = key;
    _turns_buffer[nr] = turn;
    _thread_counter += 1;
  }
  worker_cv.notify_all();
  {
    unique_lock<mutex> lk(worker_m);
    worker_cv.wait(lk, []{ return _thread_counter == 0;  })
  }
}

inline void MCTSAgent::predict_worker() {
  while (true) {
    unique_lock<mutex> lk(worker_m);
    wait(lk, []{ return _thread_counter == _nr_threads; });
    call_predict();
    _thread_counter = 0;
    lk.unlock();
    worker_cv.notify_all();
  }
}
