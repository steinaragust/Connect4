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
model2 = load_model(3)
game = cppyy.gbl.Connect4()
agent1 = MCTSAgent(game.info, 'MCTSAgent1', 200, model1)
agent2 = MCTSAgent(game.info, 'MCTSAgent2', 200, model2)
winner1 = play_game(agent1, agent2)
print("Winner match 1: %s" % (winner1))
winner2 = play_game(agent2, agent1)
print("Winner match 2: %s" % (winner2))
