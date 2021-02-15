import cppyy
import numpy as np
import torch
import torch.tensor as tensor
from cppyy import ll
from array import array
from resnet import ResNet
from utils import encode_for_predict

cppyy.include('../Connect4-Game/Game.cpp')
cppyy.include('../MCTS/TreeNodeLabel.cpp')
cppyy.include('../MCTS/HashMapTree.cpp')
cppyy.include('../MCTS/MCTSAgent.cpp')
cppyy.include('../MCTS/MCTS.cpp')

from cppyy.gbl import MCTSAgent
from cppyy.gbl import Connect4

class MCTS_Trainer:
  def __init__(self, model, simulations = 200):
    self.turn = 0
    self.model = model
    self.states = np.array([])
    self.turns = np.array([])
    self.policies = np.array([])
    self.values = np.array([])
    self.agent_1 = MCTSAgent.MCTSAgent('AZ_Agent_0', simulations)
    self.agent_2 = MCTSAgent.MCTSAgent('AZ_Agent_1', simulations)
    self.score = {'AZ_Agent_0': 0, 'AZ_Agent_1': 0, 'Draw': 0}
    self.agent_1.set_predict(prufa)
    self.agent_2.set_predict(prufa)

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
        print(array('f', obj.policy))
        self.states = np.append(self.states, game.get_board())
        self.turns = np.append(self.turns, turn)
        self.policies = np.append(self.policies, list(array('f', obj.policy)))
        print(self.policies)
        # print(array('d', obj.n_values))
        # print(array('d', obj.q_values))
        # print(array('d', obj.policy))
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

def prufa(values, states, turn, n_states):
  encoded = encode_for_predict(states, turn, n_states)
  trainer.predict(encoded, values, n_states)

model = ResNet()
trainer = MCTS_Trainer(model)
trainer.play_matches(1)