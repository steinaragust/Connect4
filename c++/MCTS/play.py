import cppyy
import cppyy.ll as ll
import ctypes
from pprint import pprint
from array import array
import math
from timeit import default_timer as timer
from array import array

cppyy.include('../Game.cpp')
cppyy.include('TreeNodeLabel.cpp')
cppyy.include('HashMapTree.cpp')
cppyy.include('MCTSAgent.cpp')
cppyy.include('MCTS.cpp')

from cppyy.gbl import MCTSAgent
from cppyy.gbl import Connect4

cppyy.cppdef("""
#include <limits>
const int max_int = numeric_limits<int>::max();
const int min_int = numeric_limits<int>::min();
""")

# Play match

players_arr = ['John', 'Dave']
score = {players_arr[0]: 0, players_arr[1]: 0, 'Draw': 0}


agent_1 = MCTSAgent.MCTSAgent(players_arr[0], 200)
agent_2 = MCTSAgent.MCTSAgent(players_arr[1], 200)
game = Connect4.Connect4()

def play_a_game(player_1, player_2):
    game = Connect4.Connect4()

    while(not game.is_terminal_state()):
        turn = game.get_to_move()
        obj = None
        if (turn == game.PLAYER_1):
            obj = player_1.play(game)
        else:
            obj = player_2.play(game)
        # print(array('d', obj.n_values))
        # print(array('d', obj.q_values))
        # print(array('d', obj.policy))
        game.drop_piece_in_column(obj.column)
    if game.winning_move():
        return player_1.get_name() if game.get_current_opponent_piece() == 0 else player_2.get_name()
    return 'Draw'


def play_match():
    first = play_a_game(agent_1, agent_2)
    second = play_a_game(agent_2, agent_1)
    score[first] += 1
    score[second] += 1


start_time = timer()
for i in range(10):
    play_match()
end_time = timer()

print(score)
print('Time: %f' % (end_time - start_time))
