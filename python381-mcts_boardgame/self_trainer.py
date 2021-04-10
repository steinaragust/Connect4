import cppyy
from utils import load_model, latest_generation, encode_for_training, save_dataset, save_model
from train import train_dataset
from mcts_agent import MCTSAgent

cppyy.include('./BoardGame/C4/Connect4.cpp')

nr_random_moves = 6
simulations = 300

def train_generations(nr_generations, nr_matches = 200):
  game = cppyy.gbl.Connect4.Connect4()
  generation = latest_generation()
  agent = MCTSAgent(game.info, 'MCTSAgent', simulations)

  states = []
  turns = []
  policies = []
  values = []
  score = { 'Player_1': 0, 'Player_2': 0, 'Draw': 0 }

  def next_generation():
    nonlocal states, turns, policies, values, score, agent, generation
    states = []
    turns = []
    policies = []
    values = []
    score = { 'Player_1': 0, 'Player_2': 0, 'Draw': 0 }
    model = load_model(generation)
    agent.load_model(model)
    generation += 1

  def play_a_game():
    game.reset()
    moves = 0
    nonlocal states, turns, policies, values, agent
    while(not game.is_terminal_state()):
        random_move = moves < nr_random_moves
        turn = game.get_to_move()
        obj = agent.play(game, random_move)
        turns.append(turn)
        state = game.get_board()
        state = [[state[r][c] for c in range(game.info.COLUMNS)] for r in range(game.info.ROWS)]
        states.append(state)
        policy = list([obj.policy[i] for i in range(game.info.priors_arr_size)])
        policies.append(policy)
        values.append(obj.q_value)
        game.make_move(obj.move)
        moves += 1
    if game.winning_move():
        return 'Player_1' if game.get_to_move_opponent() == 0 else 'Player_2'
    return 'Draw'

  def play_games(n_games):
    nonlocal score
    for m in range(n_games):
      should_print = ((m + 1) % 10) == 0
      if should_print:
        print('Starting game nr: %d\n' % (m + 1))
      result = play_a_game()
      score[result] += 1

  for _ in range(nr_generations):
    next_generation()
    print('Generation %d starting' % (generation))
    play_games(nr_matches)
    data = encode_for_training(states, turns, policies, values)
    model = agent.get_model()
    train_dataset(data, model, generation)
    save_dataset(states, turns, policies, values, generation)
    save_model(model, generation)

print('Starting training')
train_generations(2, 200)
print('Training over')