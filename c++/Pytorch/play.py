import cppyy
import cppyy.ll as ll
import ctypes
from pprint import pprint
from array import array
import math
from timeit import default_timer as timer
import numpy as np

cppyy.include('../Connect4-Game/Game.cpp')
cppyy.include('../MCTS/TreeNodeLabel.cpp')
cppyy.include('../MCTS/HashMapTree.cpp')
cppyy.include('../MCTS/MCTSAgent.cpp')
cppyy.include('../MCTS/MCTS.cpp')

from cppyy.gbl import MCTSAgent
from cppyy.gbl import Connect4

cppyy.cppdef("""
#include <limits>
const int max_int = numeric_limits<int>::max();
const int min_int = numeric_limits<int>::min();
""")

simulations = 200

def play_a_game(player_1, player_2, states, turns, columns, policies):
    game = Connect4.Connect4()

    while(not game.is_terminal_state()):
        turn = game.get_to_move()
        obj = None
        if (turn == game.PLAYER_1):
            obj = player_1.play(game)
        else:
            obj = player_2.play(game)
        states.append(np.array(game.get_board()))
        turns.append(turn)
        columns.append(obj.column)
        policies.append(array('f', obj.policy))
        # print(array('d', obj.n_values))
        # print(array('d', obj.q_values))
        # print(array('d', obj.policy))
        game.drop_piece_in_column(obj.column)
    if game.winning_move():
        return player_1.get_name() if game.get_current_opponent_piece() == 0 else player_2.get_name()
    return 'Draw'


def play_match(score, agent_1, agent_2, states, turns, columns, policies):
    first = play_a_game(agent_1, agent_2, states, turns, columns, policies)
    second = play_a_game(agent_2, agent_1, states, turns, columns, policies)
    score[first] += 1
    score[second] += 1

def play_matches(n_matches, agent_1, agent_2):
    score = {agent_1.get_name(): 0, agent_2.get_name(): 0, 'Draw': 0}
    states = []
    turns = []
    columns = []
    policies = []
    for _ in range(n_matches):
        play_match(score, agent_1, agent_2, states, turns, columns, policies)
    print(score)

    return states, turns, columns, policies

# agent_1 = MCTSAgent.MCTSAgent('Agent_0', simulations)
# agent_2 = MCTSAgent.MCTSAgent('Agent_1', simulations)

# play_matches(5, agent_1, agent_2)