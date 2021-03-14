import cppyy
from cppyy import ll
import torch
import torch.tensor as tensor
from resnet import ResNet
import numpy as np
from array import array

cppyy.include('./Connect4.cpp')
cppyy.include('./TreeNodeLabel.cpp')
cppyy.include('./HashMapTree.cpp')
cppyy.include('./MCTSAgent.cpp')

model_path = 'data/models'

from cppyy.gbl import MCTSAgent, Connect4, BoardGame

class MCTSAgent(cppyy.gbl.MCTSAgent):
  def __init__(self, name = "MCTSAgent", iterations = 200, use_nn = False, model_nr = 0):
    if use_nn:
      self.load_model(model_nr)
    super().__init__(name, iterations, use_nn)

  def load_model(self, model_n):
    fpath = model_path + '/model_' + str(model_n) + '.pt'
    print('loading model: %s' % (fpath))
    self.model = ResNet()
    self.model.load_state_dict(torch.load(fpath))
    self.model.eval()

  def play_game(self):
    game = Connect4.Connect4()
    while not game.is_terminal_state():
      obj = self.play(game)
      game.make_move(obj.move)
      game.print_board()

  def predict(self, values, states, turns):
    states_list = list(states)
    turns_list = list(turns)
    encoded_states = self.encode(states_list, turns_list)
    tensor_arr = tensor(encoded_states, dtype=torch.float)
    with torch.no_grad():
      policy_pred, value_pred = self.model(tensor_arr)
    for i in range(len(states_list)):
      for j in range(BoardGame.info.priors_arr_size):
        values[i][j] = policy_pred[i][j].item()
      values[i][BoardGame.info.priors_arr_size] = value_pred[i][0].item()

  def encode(self, states, turn):
      encoded = np.zeros([len(states),BoardGame.info.nr_unique_pcs + 1, BoardGame.info.ROWS, BoardGame.info.COLUMNS]).astype(int)
      for i in range(len(states)):
          for r in range(BoardGame.info.ROWS):
              for c in range(BoardGame.info.COLUMNS):
                sq = states[i][r][c]
                if sq != 0:
                    channel = sq - 1
                    encoded[i][channel][r][c] = 1
          encoded[i,BoardGame.info.nr_unique_pcs,:,:] = turn[i]
      return encoded


player = MCTSAgent("Jonas", 200, True, 7)
player.play_game()
