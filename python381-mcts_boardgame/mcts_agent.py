import cppyy
import torch
import torch.tensor as tensor
import numpy as np

cppyy.include('./BoardGame.h')
cppyy.include('./TreeNodeLabel.cpp')
cppyy.include('./HashMapTree.cpp')
cppyy.include('./MCTSAgent.cpp')

class MCTSAgent(cppyy.gbl.MCTSAgent):
  def __init__(self, game_info, name = 'MCTSAgent', iterations = 200, model = None):
    self.model = model
    super().__init__(game_info, name, iterations, False if model == None else True)

  def load_model(self, model):
    self.model = model
    self.set_NN_predict(True)

  def predict(self, values, states, turns):
    states_list = list(states)
    turns_list = list(turns)
    encoded_states = self.encode(states_list, turns_list)
    tensor_arr = tensor(encoded_states, dtype=torch.float)
    with torch.no_grad():
      policy_pred, value_pred = self.model(tensor_arr)
    for i in range(len(states_list)):
      for j in range(self._info.priors_arr_size):
        values[i][j] = policy_pred[i][j].item()
      values[i][self._info.priors_arr_size] = value_pred[i][0].item()

  def encode(self, states, turn):
      encoded = np.zeros([len(states), self._info.nr_unique_pcs + 1, self._info.ROWS, self._info.COLUMNS]).astype(int)
      for i in range(len(states)):
          for r in range(self._info.ROWS):
              for c in range(self._info.COLUMNS):
                sq = states[i][r][c]
                if sq != 0:
                    channel = sq - 1
                    encoded[i][channel][r][c] = 1
          encoded[i,self._info.nr_unique_pcs,:,:] = turn[i]
      return encoded

