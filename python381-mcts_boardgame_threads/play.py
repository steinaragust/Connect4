import cppyy
from cppyy import ll
from utils import load_model
from mcts_agent import MCTSAgent

cppyy.include('Connect4.cpp')

nr_random_moves = 0
simulations = 300

PLAYER_1 = 'Player_1'
PLAYER_2 = 'Player_2'
DRAW = 'Draw'
WIN = 'Win'
LOSE = 'Lose'

game = cppyy.gbl.Connect4.Connect4()

def play_game(agent1):
  game.reset()
  moves = 0
  while(not game.is_terminal_state()):
      random_move = moves < nr_random_moves
      obj = agent1.play(game, random_move)
      game.make_move(obj.move)
      game.print_board()
      moves += 1
  winner = DRAW
  if game.winning_move():
      winner = PLAYER_1 if game.get_to_move_opponent() == 0 else PLAYER_2
  return winner, moves

model1_nr = 8
model1 = load_model(model1_nr)

agent1 = MCTSAgent(game.info, 'AZ_MCTS_Agent_Model-' + str(model1_nr), simulations, model1)

play_game(agent1)
