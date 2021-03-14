import cppyy
import numpy as np
import torch
import torch.tensor as tensor
from resnet import ResNet
from utils import encode_for_predict
from array import array

model_path = 'data/models'

cppyy.include('../Connect4-Game/Game.cpp')
cppyy.include('../MCTS/TreeNodeLabel.cpp')
cppyy.include('../MCTS/HashMapTree.cpp')
cppyy.include('../MCTS/MCTSAgent.cpp')
cppyy.include('../MCTS/MCTS.cpp')

from cppyy.gbl import MCTSAgent
from cppyy.gbl import Connect4

class MCTS_Evaluator:
  def __init__(self, predictPtr, agent_1_model_nr, agent_2_model_nr, simulations = 200):
    self.agent_1_name = 'AZ_Agent_0_model_' + str(agent_1_model_nr)
    self.agent_2_name = 'AZ_Agent_1_model_' + str(agent_2_model_nr)
    self.agent_1 = MCTSAgent.MCTSAgent(self.agent_1_name, simulations, predictPtr)
    self.agent_2 = MCTSAgent.MCTSAgent(self.agent_2_name, simulations, predictPtr)
    self.model_1 = ResNet()
    self.load_model_for_agent(0, agent_1_model_nr)
    self.model_2 = ResNet()
    self.load_model_for_agent(1, agent_2_model_nr)
    self.model_1.eval()
    self.model_2.eval()
    self.reset()

  def load_model_for_agent(self, agent_model, model_n):
    fpath = model_path + '/model_' + str(model_n) + '.pt'
    print('loading model for agent %d: ' % (agent_model) + ' ' + fpath)
    if agent_model == 0:
      self.model_1.load_state_dict(torch.load(fpath))
    else:
      self.model_2.load_state_dict(torch.load(fpath))

  def predict(self, states, values, n_states):
    tensor_arr = tensor(states, dtype=torch.float)
    model = self.model_1 if self.agent_turn == 0 else self.model_2
    with torch.no_grad():
      policy_pred, value_pred = model(tensor_arr)
    for i in range(n_states):
      for j in range(0, 7):
        values[i][j] = policy_pred[i][j].item()
      values[i][7] = value_pred[i][0].item()

  def can_win_now(self, game):
    moves = game.get_valid_columns()
    for m in moves:
      game.drop_piece_in_column(m)
      winning_move = game.winning_move()
      game.retract_piece_in_column(m)
      if winning_move:
        return True
    return False

  def play_a_game(self, player_1, player_2):
    game = Connect4.Connect4()
    print('player: 1 is: %s' % (player_1.get_name()))
    print('player: 2 is: %s\n' % (player_2.get_name()))
    move = 0
    while(not game.is_terminal_state()):
      self.turn = game.get_to_move()
      obj = None
      random_move = move < 4
      print('move nr: %d' % (move))
      if (self.turn == game.PLAYER_1):
        self.agent_turn = 0 if player_1.get_name() == self.agent_1_name else 1
        print('player 1 turn')
        obj = player_1.play(game, random_move)
      else:
        self.agent_turn = 0 if player_2.get_name() == self.agent_1_name else 1
        print('player 2 turn')
        obj = player_2.play(game, random_move)
      game.drop_piece_in_column(obj.column)
      game.print_board()
      print('\n\n')
      move += 1
    if game.winning_move():
      winner = player_1.get_name() if game.get_to_move_opponent() == 0 else player_2.get_name()
      return winner
    return 'Draw'

  def reset(self):
    self.score = {self.agent_1_name: 0, self.agent_2_name: 0, 'Draw': 0}

  def play_matches_cpu(self, n_matches = 100):
    for _ in range(n_matches):
      first = self.play_a_game(self.agent_1, self.agent_2)
      print('winner is: %s\n\n' % (first))
      second = self.play_a_game(self.agent_2, self.agent_1)
      print('winner is: %s\n\n' % (second))
      self.score[first] += 1
      self.score[second] += 1
      print(self.score)
    return self.score
  
def predict(values, states, turn, n_states):
  encoded = encode_for_predict(states, turn, n_states)
  evaluator.predict(encoded, values, n_states)

evaluator = MCTS_Evaluator(predict, 0, 7)

score = evaluator.play_matches_cpu(10)
