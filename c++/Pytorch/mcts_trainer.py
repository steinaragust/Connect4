import cppyy
import numpy as np
import torch
import torch.tensor as tensor
from array import array
from resnet import ResNet
from utils import encode_for_training, decode_training_data
from os import listdir
from os.path import isfile, join
import re

dataset_path = 'data/datasets'
model_path = 'data/models'

cppyy.include('../Connect4-Game/Game.cpp')
cppyy.include('../MCTS/TreeNodeLabel.cpp')
cppyy.include('../MCTS/HashMapTree.cpp')
cppyy.include('../MCTS/MCTSAgent.cpp')
cppyy.include('../MCTS/MCTS.cpp')

from cppyy.gbl import MCTSAgent
from cppyy.gbl import Connect4

class MCTS_Trainer:
  def __init__(self, predictPtr, simulations = 200):
    self.model = ResNet()
    self.next_generation(True)
    # self.agent_1 = MCTSAgent.MCTSAgent('AZ_Agent_0', simulations)
    # self.agent_2 = MCTSAgent.MCTSAgent('AZ_Agent_1', simulations)
    self.agent_1 = MCTSAgent.MCTSAgent('AZ_Agent_0', simulations, predictPtr)
    self.agent_2 = MCTSAgent.MCTSAgent('AZ_Agent_1', simulations, predictPtr)

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
    self.score = {'AZ_Agent_0': 0, 'AZ_Agent_1': 0, 'Draw': 0}

  # Loads model nr n. If n = -1, then will load latest model
  def load_model(self, model_n = -1):
    if self.generation == 0:
      return False
    if model_n == -1:
      model_n = self.generation - 1
    fpath = model_path + '/model_' + str(model_n) + '.pt'
    self.model.load_state_dict(torch.load(fpath))
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

  def predict(self, states, values, n_states):
    tensor_arr = tensor(states, dtype=torch.float)
    with torch.no_grad():
      policy_pred, value_pred = self.model(tensor_arr)
    for i in range(n_states):
      for j in range(0, 7):
        values[i][j] = policy_pred[i][j].item()
      values[i][7] = value_pred[i][0].item()

  def play_a_game(self, player_1, player_2):
    game = Connect4.Connect4()
    moves = 0
    while(not game.is_terminal_state()):
        turn = game.get_to_move()
        obj = None
        if (turn == game.PLAYER_1):
            obj = player_1.play(game, moves < 6)
        else:
            obj = player_2.play(game, moves < 6)
        moves += 1
        self.states.append(game.get_board())
        self.turns.append(turn)
        self.policies.append(list(array('f', obj.policy)))
        self.values.append(obj.q_value)
        game.drop_piece_in_column(obj.column)
    if game.winning_move():
        return player_1.get_name() if game.get_to_move_opponent() == 0 else player_2.get_name()
    return 'Draw'
  
  def play_matches(self, n_matches = 100):
    for m in range(n_matches):
      should_print = ((m + 1) % 10) == 0
      if should_print:
        print('Starting match nr: %d\n' % (m + 1))
      first = self.play_a_game(self.agent_1, self.agent_2)
      second = self.play_a_game(self.agent_2, self.agent_1)
      self.score[first] += 1
      self.score[second] += 1
  
  def get_training_set(self):
    return encode_for_training(self.states, self.turns, self.policies, self.values)

