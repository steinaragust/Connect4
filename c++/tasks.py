import cppyy
import cppyy.ll as ll
import ctypes
from pprint import pprint
from array import array

cppyy.include('Game.cpp')

from cppyy.gbl import Connect4

game = Connect4.Connect4()

def test_call_python(arr):
  print(arr)
  for i in range (6):
    for j in range(7):
      arr[i][j] = 6
  # arr = ll.array_new[int](game.ROWS)
  # for i in range(len(arr)):

  #   arr[i] = ll.cast['int*'](ll.array_new[int](game.COLUMNS))
  #   for j in range(game.COLUMNS):
  #     arr[i][j] = j

game.set_f(test_call_python)
game.call_f()

# while(not game.is_terminal_state()):
#   game.drop_piece_in_column(0 if game.get_to_move() == 0 else 1)
#   game.print_board()

