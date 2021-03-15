import cppyy
import torch
from resnet import ResNet
from mcts_agent import MCTSAgent

cppyy.include('./Connect4.cpp')

model_path = 'data/models'

def load_model(model_n):
  fpath = model_path + '/model_' + str(model_n) + '.pt'
  print('loading model: %s' % (fpath))
  model = ResNet()
  model.load_state_dict(torch.load(fpath))
  model.eval()
  return model

model1 = load_model(7)
model2 = load_model(7)
game = cppyy.gbl.Connect4()
agent1 = MCTSAgent(game.info, 'MCTSAgent1', 200)
agent2 = MCTSAgent(game.info, 'MCTSAgent2', 200)

while not game.is_terminal_state():
  agent = agent1 if game.get_to_move() else agent2
  obj = agent.play(game)
  game.make_move(obj.move)
  game.print_board()
