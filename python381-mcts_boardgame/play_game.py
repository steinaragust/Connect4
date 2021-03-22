import cppyy
import torch
import random
from resnet import ResNet
from mcts_agent import MCTSAgent

cppyy.include('./Connect4.cpp')

model_path = 'data/models'

matches = 20

class SimpleAgent:
  def __init__(self, info):
    self.agent = MCTSAgent(info, "SimpleAgent")
  def play(self, game):
    m = self.agent.can_win_now(game)
    if m == -1:
      moves = game.get_valid_moves()
      m = random.choice(moves)
    return type('',(object,),{"move": m})()
  def get_name(self):
    return self.agent.get_name()

def load_model(model_n):
  fpath = model_path + '/model_' + str(model_n) + '.pt'
  print('loading model: %s' % (fpath))
  model = ResNet()
  model.load_state_dict(torch.load(fpath))
  model.eval()
  return model
  
def play_game(agent1, agent2):
  game.reset()
  while not game.is_terminal_state():
    agent = agent1 if game.get_to_move() == 0 else agent2
    obj = agent.play(game)
    game.make_move(obj.move)
    game.print_board()
    print('\n\n')
  if not game.winning_move():
    return "Draw"
  return agent1.get_name() if game.get_to_move_opponent() == 0 else agent2.get_name()

model1 = load_model(0)
model2 = load_model(5)
game = cppyy.gbl.Connect4()
agent1 = MCTSAgent(game.info, 'MCTSAgent1', 200, model1)
agent2 = MCTSAgent(game.info, 'MCTSAgent2', 200, model2)
scores = { agent1.get_name(): 0, agent2.get_name(): 0 }

for i in range(matches):
  winner1 = play_game(agent1, agent2)
  print("Winner match 1: %s" % (winner1))
  winner2 = play_game(agent2, agent1)
  print("Winner match 2: %s" % (winner2))
  scores[winner1] += 1
  scores[winner2] += 1
  print(scores)
