import cppyy
import numpy as np
import torch
import torch.tensor as tensor
from cppyy import ll
from array import array
from resnet import ResNet
from utils import encode_for_predict, encode_for_training_1
from os import listdir
from os.path import isfile, join
import re
#onlyfiles = [f for f in listdir(mypath) if isfile(join(mypath, f))]

dataset_path = 'data/datasets'

cppyy.include('../Connect4-Game/Game.cpp')
cppyy.include('../MCTS/TreeNodeLabel.cpp')
cppyy.include('../MCTS/HashMapTree.cpp')
cppyy.include('../MCTS/MCTSAgent.cpp')
cppyy.include('../MCTS/MCTS.cpp')

from cppyy.gbl import MCTSAgent
from cppyy.gbl import Connect4

class MCTS_Trainer:
  def __init__(self, model, simulations = 200):
    self.model = model
    self.clear()
    self.agent_1 = MCTSAgent.MCTSAgent('AZ_Agent_0', simulations)
    self.agent_2 = MCTSAgent.MCTSAgent('AZ_Agent_1', simulations)
    # self.agent_1.set_predict(predict)
    # self.agent_2.set_predict(predict)

  def predict(self, states, values, n_states):
    tensor_arr = tensor(states, dtype=torch.float)
    policy_pred, value_pred = self.model(tensor_arr)
    for i in range(n_states):
      for j in range(0, 7):
        values[i][j] = policy_pred[i][j].item()
      values[i][7] = value_pred[i][0].item()

  def play_a_game(self, player_1, player_2):
    game = Connect4.Connect4()
    while(not game.is_terminal_state()):
        turn = game.get_to_move()
        obj = None
        if (turn == game.PLAYER_1):
            obj = player_1.play(game)
        else:
            obj = player_2.play(game)
        self.states.append(game.get_board())
        self.turns.append(turn)
        self.policies.append(list(array('f', obj.policy)))
        self.values.append(obj.q_value)
        game.drop_piece_in_column(obj.column)
    if game.winning_move():
        return player_1.get_name() if game.get_current_opponent_piece() == 0 else player_2.get_name()
    return 'Draw'
  
  def play_matches(self, n_matches = 100):
    for _ in range(n_matches):
      first = self.play_a_game(self.agent_1, self.agent_2)
      second = self.play_a_game(self.agent_2, self.agent_1)
      self.score[first] += 1
      self.score[second] += 1

  def clear(self):
    self.states = []
    self.turns = []
    self.policies = []
    self.values = []
    self.score = {'AZ_Agent_0': 0, 'AZ_Agent_1': 0, 'Draw': 0}

  def save_dataset(self):
    file_n = 0
    files = [f for f in listdir(dataset_path) if isfile(join(dataset_path, f))]
    if len(files) != 0:
      numbers = []
      for f in files:
        m = re.search('^dataset_(\d+)\.npy$', f)
        numbers.append(int(m.group(1)))
      file_n = max(numbers) + 1
    data = encode_for_training_1(self.states, self.turns, self.policies, self.values)
    print(data)
    #data = np.array([len(self.states), self.states, self.turns, self.values], dtype=object)
    # test = np.array([1, 2, 3.0])
    # fpath = dataset_path + '/dataset_' + str(file_n) + '.npy'
    # np.save(fpath, test)
    return 0

def predict(values, states, turn, n_states):
  encoded = encode_for_predict(states, turn, n_states)
  trainer.predict(encoded, values, n_states)



model = ResNet()
trainer = MCTS_Trainer(model)
trainer.play_matches(1)
trainer.save_dataset()



