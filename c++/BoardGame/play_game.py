import cppyy
import torch
from resnet import ResNet
from mcts_agent import MCTSAgent

cppyy.include('./Connect4.cpp')
from cppyy.gbl import Connect4

model_path = 'data/models'

game = Connect4.Connect4()

def load_model(model_n):
  fpath = model_path + '/model_' + str(model_n) + '.pt'
  print('loading model: %s' % (fpath))
  model = ResNet()
  model.load_state_dict(torch.load(fpath))
  model.eval()
  return model

model = load_model(7)
game = Connect4.Connect4()
agent = MCTSAgent(game.info, 'MCTSAgent', 200, model)

while not game.is_terminal_state():
  obj = agent.play(game)
  game.make_move(obj.move)
  game.print_board()
