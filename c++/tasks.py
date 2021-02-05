import cppyy
import cppyy.ll as ll
import ctypes
from pprint import pprint
from array import array
import math
from timeit import default_timer as timer
from array import array

cppyy.include('./Connect4-Game/Game.cpp')
cppyy.include('./Connect4-Game/MiniMax.cpp')

from cppyy.gbl import mini_max
from cppyy.gbl import Connect4

cppyy.cppdef("""
#include <limits>
const int max_int = numeric_limits<int>::max();
const int min_int = numeric_limits<int>::min();
""")


# Call python

# game = Connect4.Connect4()

# def test_call_python(arr):
#     print(arr)
#     for i in range(6):
#         for j in range(7):
#             arr[i][j] = 6
# arr = ll.array_new[int](game.ROWS)
# for i in range(len(arr)):

#   arr[i] = ll.cast['int*'](ll.array_new[int](game.COLUMNS))
#   for j in range(game.COLUMNS):
#     arr[i][j] = j


# game.set_f(test_call_python)
# game.call_f()

# Play one  game simply

# game = Connect4.Connect4()

# while(not game.is_terminal_state()):
#     game.drop_piece_in_column(0 if game.get_to_move() == 0 else 1)
#     game.print_board()

# Play match

players_arr = ['John', 'Dave']
score = {players_arr[0]: 0, players_arr[1]: 0, 'Draw': 0}


def play_a_game(player_1, player_2):
    game = Connect4.Connect4()

    while(not game.is_terminal_state()):
        arr = array('d', mini_max(game, 5, cppyy.gbl.min_int, cppyy.gbl.max_int, True))
        game.drop_piece_in_column(int(arr[0]))

    if game.winning_move():
        return player_1 if game.get_current_opponent_piece() == 0 else player_2
    game.print_board()
    return 'Draw'


def play_match():
    first = play_a_game(players_arr[0], players_arr[1])
    second = play_a_game(players_arr[1], players_arr[0])
    score[first] += 1
    score[second] += 1


start_time = timer()
for i in range(1):
    play_match()
end_time = timer()

print(score)
print('Time: %f' % (end_time - start_time))
