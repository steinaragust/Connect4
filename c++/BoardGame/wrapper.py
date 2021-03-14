import cppyy
from array import array

cppyy.include('./Connect4.cpp')
cppyy.include('./TreeNodeLabel.cpp')
cppyy.include('./HashMapTree.cpp')
cppyy.include('./MCTSAgent.cpp')


from cppyy.gbl import MCTSAgent, Connect4

class MCTS_Player(cppyy.gbl.MCTSAgent):
  def play_game(self):
    game = Connect4.Connect4()
    print(game.info.ROWS)
    while not game.is_terminal_state():
      obj = self.play(game)
      game.make_move(obj.move)
      game.print_board()

  def predict(self, values, states, turns):
    print(values)
    print(list(states))
    print(list(turns))


player = MCTS_Player("Jonas", 200, True)
player.play_game()
