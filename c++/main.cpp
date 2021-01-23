#include <iostream>
#include "Game.h"

using namespace std;

int main()
{
  Connect4 *game = new Connect4();

  int move = -1;

  while (!game->is_terminal_state())
  {
    // cout << "Choose column from 0-6" << endl;
    // cin >> move;
    try
    {
      vector<int> moves = game->get_valid_columns();

      int turn = game->get_to_move();

      cout << "Player " << turn << endl;

      // cout << "Choose a column to drop in: " << endl;
      // for (vector<int>::const_iterator i = moves.begin(); i != moves.end(); ++i)
      // {
      //   cout << *i << ' ';
      // }
      // cout << endl;

      // cin >> move;
      move = turn == 0 ? 0 : 1;

      game->drop_piece_in_column(move);
      game->print_board();
    }
    catch (const char *msg)
    {
      cout << msg << endl;
    }
    int winner = game->get_to_move_opponent();
    cout << "Player " << winner << " wins" << endl;
  }

  game->~Connect4();
  return 0;
}
