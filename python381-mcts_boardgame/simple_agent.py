import random
from mcts_agent import MCTSAgent

class SimpleAgent:
  def __init__(self, info, name = 'SimpleAgent'):
    self.agent = MCTSAgent(info, name)
  def play(self, game, random_move):
    m = self.agent.can_win_now(game)
    if m == -1 or random_move == True:
      moves = game.get_valid_moves()
      m = random.choice(moves)
    return type('',(object,),{"move": m})()
  def get_name(self):
    return self.agent.get_name()