import cppyy
import numpy as np
import torch
import torch.tensor as tensor
from array import array

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
    self.states = np.array()
    self.turns = np.array()
    self.policies = np.array()
    self.values = np.array()
    self.agent_1 = MCTSAgent.MCTSAgent('AZ_Agent_0', simulations)
    self.agent_2 = MCTSAgent.MCTSAgent('AZ_Agent_1', simulations)
    self.agent_1.set_predict(self.predict)
    self.agent_2.set_predict(self.predict)
    self.score = {'AZ_Agent_0': 0, 'AZ_Agent_1': 0, 'Draw': 0}

  def predict(self, values, board, turn):
    encoded = np.zeros([1,3,6,7]).astype(int)
    for r in range(0, 6):
      for c in range(0, 7):
        if (board[r][c] != 0):
          channel = board[r][c] - 1
          encoded[0][channel][r][c] = 1
    encoded[0, 2, :, :] = turn
    tensor_arr = tensor(encoded, dtype=torch.float)
    policy_pred, value_pred = self.model(tensor_arr)
    for i in range(0, 7):
      values[i] = policy_pred[0][i].item()
    values[7] = value_pred[0][0].item()

  def play_a_game(self, player_1, player_2):
    game = Connect4.Connect4()

    while(not game.is_terminal_state()):
        turn = game.get_to_move()
        obj = None
        if (turn == game.PLAYER_1):
            obj = player_1.play(game)
        else:
            obj = player_2.play(game)
        self.states.append(np.array(game.get_board()))
        self.turns.append(turn)
        self.columns.append(obj.column)
        self.policies.append(array('f', obj.policy))
        # print(array('d', obj.n_values))
        # print(array('d', obj.q_values))
        # print(array('d', obj.policy))
        game.drop_piece_in_column(obj.column)
    if game.winning_move():
        return player_1 if game.get_current_opponent_piece() == 0 else player_2
    return 'Draw'
  
  def play_matches(self, n_matches = 100):
    for _ in range(n_matches):
      first = self.play_a_game(self.agent_1.get_name(), self.agent_2.get_name())
      second = self.play_a_game(self.agent_2.get_name(), self.agent_1.get_name())
      self.score[first] += 1
      self.score[second] += 1