import random

class SimpleAgent:
  def __init__(self, name = 'SimpleAgent'):
    self.name = name
  def get_name(self):
    return self.name
  def can_win_now(self, game):
    moves = game.get_valid_moves()
    chosen = -1
    for m in moves:
      game.make_move(m)
      if game.winning_move():
        chosen = m
      game.retract_move(m)
    return chosen
      
  def play(self, game, random_move):
    m = self.can_win_now(game)
    if m == -1 or random_move == True:
      moves = game.get_valid_moves()
      m = random.choice(moves)
    return type('',(object,),{"move": m})()