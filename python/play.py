from connect4 import Connect4
from mini_max import minimax
import math
import copy
from timeit import default_timer as timer

players_arr = ['John', 'Dave']
score = {players_arr[0]: 0, players_arr[1]: 0, 'Draw': 0}


def play_a_game(player_1, player_2):
    game = Connect4()

    while(not game.is_terminal_node()):
        column, value = minimax(copy.deepcopy(
            game), 5, -math.inf, math.inf, True)
        game.drop_piece_in_column(column)

    if game.winning_move():
        return player_1 if game.get_opponent_piece() == 0 else player_2
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
