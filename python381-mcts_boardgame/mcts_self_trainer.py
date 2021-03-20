import numpy as np
import torch
from array import array
from resnet import ResNet
from utils import encode_for_training, decode_training_data
from mcts_agent import MCTSAgent
from os import listdir
from os.path import isfile, join
import re

dataset_path = 'data/datasets'
model_path = 'data/models'

class MCTS_Self_Trainer:
  def __init__(self, game, simulations = 200, nr_random_moves = 0):
    self.game = game
    self.nr_random_moves = nr_random_moves
    self.model = ResNet()
    self.next_generation(True)
    self.agent = MCTSAgent(self.game.info, 'MCTSAgent', simulations, self.model)

  def next_generation(self, initializing = False):
    if initializing:
      self.generation = 0
      files = [f for f in listdir(model_path) if isfile(join(model_path, f))]
      if len(files) != 0:
        numbers = []
        for f in files:
          m = re.search('^model_(\d+)\.pt$', f)
          numbers.append(int(m.group(1)))
        self.generation = max(numbers) + 1
      self.load_model()
    else:
      self.generation += 1
    self.states = []
    self.turns = []
    self.policies = []
    self.values = []
    self.score = {'Player_1': 0, 'Player_2': 0, 'Draw': 0}

  def get_training_set(self):
    return encode_for_training(self.states, self.turns, self.policies, self.values)

  # Loads model nr n. If n = -1, then will load latest model
  def load_model(self, model_n = -1):
    if self.generation == 0:
      return False
    if model_n == -1:
      model_n = self.generation - 1
    fpath = model_path + '/model_' + str(model_n) + '.pt'
    self.model.load_state_dict(torch.load(fpath))
    self.model.eval()
    print('%s loaded\n' % (fpath))
    return True

  # Saves the latest, newly trained model
  def save_latest_model(self):
    fpath = model_path + '/model_' + str(self.generation) + '.pt'
    torch.save(self.model.state_dict(), fpath)

  # Loads dataset nr n. If n = -1, then will load latest dataset
  def load_dataset(self, dataset_n = -1):
    if self.generation == 0:
      return False
    if dataset_n == -1:
      dataset_n = self.generation - 1
    fpath = dataset_path + '/dataset_' + str(dataset_n) + '.npy'
    dataset = np.load(fpath, allow_pickle=True)
    states, turns, policies, values = decode_training_data(dataset)
    self.states = states
    self.turns = turns
    self.policies = policies
    self.values = values
    return True

  # Saves the newest dataset
  def save_latest_dataset(self):
    data = self.get_training_set()
    fpath = dataset_path + '/dataset_' + str(self.generation) + '.npy'
    np.save(fpath, data)

  def play_a_game(self):
    self.game.reset()
    moves = 0
    while(not self.game.is_terminal_state()):
        random_move = moves < self.nr_random_moves
        turn = self.game.get_to_move()
        obj = self.agent.play(self.game, random_move)
        self.states.append(self.game.get_board())
        self.turns.append(turn)
        policy = list([obj.policy[i] for i in range(self.game.info.priors_arr_size)])
        print(policy)
        print('\n')
        self.policies.append(policy)
        self.values.append(obj.q_value)
        self.game.make_move(obj.move)
        moves += 1
    if self.game.winning_move():
        return 'Player_1' if self.game.get_to_move_opponent() == 0 else 'Player_2'
    return 'Draw'
  
  def play_games(self, n_games = 200):
    for m in range(n_games):
      should_print = ((m + 1) % 10) == 0
      if should_print:
        print('Starting game nr: %d\n' % (m + 1))
      result = self.play_a_game()
      self.score[result] += 1
