import cppyy
import time
import statistics
from simple_agent import SimpleAgent
from utils import load_model
from mcts_agent import MCTSAgent

cppyy.include('Connect4.cpp')

results_path = 'data/eval_results'

nr_random_moves = 4
simulations = 300

PLAYER_1 = 'Player_1'
PLAYER_2 = 'Player_2'
DRAW = 'Draw'
WIN = 'Win'
LOSE = 'Lose'

game = cppyy.gbl.Connect4.Connect4()

def play_matches(agent1, agent2, nr_matches = 100):
  scores = {
    agent1.get_name(): { WIN: { PLAYER_1: 0, PLAYER_2: 0 }, LOSE: { PLAYER_1: 0, PLAYER_2: 0 }, DRAW: { PLAYER_1: 0, PLAYER_2: 0 } },
    agent2.get_name(): { WIN: { PLAYER_1: 0, PLAYER_2: 0 }, LOSE: { PLAYER_1: 0, PLAYER_2: 0 }, DRAW: { PLAYER_1: 0, PLAYER_2: 0 } },
  }
  number_of_moves = {
    agent1.get_name(): { WIN: {PLAYER_1: [], PLAYER_2: [] }, LOSE: {PLAYER_1: [], PLAYER_2: [] } },
    agent2.get_name(): { WIN: {PLAYER_1: [], PLAYER_2: [] }, LOSE: {PLAYER_1: [], PLAYER_2: [] } },
  }
  times = {
    agent1.get_name(): { WIN: {PLAYER_1: [], PLAYER_2: [] }, LOSE: {PLAYER_1: [], PLAYER_2: [] } },
    agent2.get_name(): { WIN: {PLAYER_1: [], PLAYER_2: [] }, LOSE: {PLAYER_1: [], PLAYER_2: [] } },
  }

  def record_result(player_1, player_2, result, moves, time):
    nonlocal scores, number_of_moves, times
    if result == DRAW:
      scores[player_1][DRAW][PLAYER_1] += 1
      scores[player_2][DRAW][PLAYER_2] += 1
    elif result == PLAYER_1:
      scores[player_1][WIN][PLAYER_1] += 1
      scores[player_2][LOSE][PLAYER_2] += 1
      number_of_moves[player_1][WIN][PLAYER_1].append(moves)
      number_of_moves[player_2][LOSE][PLAYER_2].append(moves)
      times[player_1][WIN][PLAYER_1].append(time)
      times[player_2][LOSE][PLAYER_2].append(time)
    else:
      scores[player_2][WIN][PLAYER_2] += 1
      scores[player_1][LOSE][PLAYER_1] += 1
      number_of_moves[player_2][WIN][PLAYER_2].append(moves)
      number_of_moves[player_1][LOSE][PLAYER_1].append(moves)
      times[player_2][WIN][PLAYER_2].append(time)
      times[player_1][LOSE][PLAYER_1].append(time)

  def print_agent_summary(agent_name, f):
    nonlocal scores, number_of_moves, times

    f.write('Starting summary for agent: %s\n\n' % (agent_name))

    nr_of_games = nr_matches * 2

    # Scores
    total_wins = scores[agent_name][WIN][PLAYER_1] + scores[agent_name][WIN][PLAYER_2] + (sum(list(scores[agent_name][DRAW].values())) * 0.5)
    total_win_ratio = total_wins / nr_of_games

    agent_wins = scores[agent_name][WIN][PLAYER_1] + scores[agent_name][WIN][PLAYER_2]
    agent_win_ratio = agent_wins / nr_of_games
  
    agent_p1_win_ratio = 0.0 if agent_wins == 0 else scores[agent_name][WIN][PLAYER_1] / agent_wins
    agent_p2_win_ratio = 0.0 if agent_wins == 0 else scores[agent_name][WIN][PLAYER_2] / agent_wins

    agent_losses = scores[agent_name][LOSE][PLAYER_1] + scores[agent_name][LOSE][PLAYER_2]
    agent_loss_ratio = agent_losses / nr_of_games

    agent_p1_loss_ratio = 0.0 if agent_losses == 0 else scores[agent_name][LOSE][PLAYER_1] / agent_losses
    agent_p2_loss_ratio = 0.0 if agent_losses == 0 else scores[agent_name][LOSE][PLAYER_2] / agent_losses
  
    total_draws = sum(list(scores[agent_name][DRAW].values()))
    agent_p1_draw_ratio = 0.0 if total_draws == 0 else scores[agent_name][DRAW][PLAYER_1] / total_draws
    agent_p2_draw_ratio = 0.0 if total_draws == 0 else scores[agent_name][DRAW][PLAYER_2] / total_draws

    f.write('Total score: %f, total score ratio: %f\n\n' % (total_wins, total_win_ratio))

    f.write('Win ratios(excluding draws):\n')
    f.write('Win ratio: %f\n' % (agent_win_ratio))
    f.write('Win ratio: P1/P2: %f/%f\n\n' % (agent_p1_win_ratio, agent_p2_win_ratio))

    f.write('Loss ratios(excluding draws):\n')
    f.write('Loss ratio: %f\n' % (agent_loss_ratio))
    f.write('Loss ratio: P1/P2: %f/%f\n\n' % (agent_p1_loss_ratio, agent_p2_loss_ratio))

    f.write('Draw ratio: P1/P2: %f/%f\n\n' % (agent_p1_draw_ratio, agent_p2_draw_ratio))

    #Moves
    total_nr_moves_win = number_of_moves[agent_name][WIN][PLAYER_1] + number_of_moves[agent_name][WIN][PLAYER_2]
    avg_nr_moves_win = 0.0 if len(total_nr_moves_win) == 0 else statistics.fmean(total_nr_moves_win)
    std_nr_moves_win = 0.0 if len(total_nr_moves_win) < 2 else statistics.stdev(total_nr_moves_win)

    total_nr_moves_lose = number_of_moves[agent_name][LOSE][PLAYER_1] + number_of_moves[agent_name][LOSE][PLAYER_2]
    avg_nr_moves_lose = 0.0 if len(total_nr_moves_lose) == 0 else statistics.fmean(total_nr_moves_lose)
    std_nr_moves_lose = 0.0 if len(total_nr_moves_lose) < 2 else statistics.stdev(total_nr_moves_lose)

    avg_p1_win_move = 0.0 if len(number_of_moves[agent_name][WIN][PLAYER_1]) == 0 else statistics.fmean(number_of_moves[agent_name][WIN][PLAYER_1])
    std_p1_win_move = 0.0 if len(number_of_moves[agent_name][WIN][PLAYER_1]) < 2 else statistics.stdev(number_of_moves[agent_name][WIN][PLAYER_1])
  
    avg_p2_win_move = 0.0 if len(number_of_moves[agent_name][WIN][PLAYER_2]) == 0 else statistics.fmean(number_of_moves[agent_name][WIN][PLAYER_2])
    std_p2_win_move = 0.0 if len(number_of_moves[agent_name][WIN][PLAYER_2]) < 2 else statistics.stdev(number_of_moves[agent_name][WIN][PLAYER_2])

    avg_p1_lose_move = 0.0 if len(number_of_moves[agent_name][LOSE][PLAYER_1]) == 0 else statistics.fmean(number_of_moves[agent_name][LOSE][PLAYER_1])
    std_p1_lose_move = 0.0 if len(number_of_moves[agent_name][LOSE][PLAYER_1]) < 2 else statistics.stdev(number_of_moves[agent_name][LOSE][PLAYER_1])
  
    avg_p2_lose_move = 0.0 if len(number_of_moves[agent_name][LOSE][PLAYER_2]) == 0 else statistics.fmean(number_of_moves[agent_name][LOSE][PLAYER_2])
    std_p2_lose_move = 0.0 if len(number_of_moves[agent_name][LOSE][PLAYER_2]) < 2 else statistics.stdev(number_of_moves[agent_name][LOSE][PLAYER_2])

    f.write('Average number of moves for won/lost games: %f/%f\n' % (avg_nr_moves_win, avg_nr_moves_lose))
    f.write('Standard deviation for number of moves for won/lost games: %f/%f\n\n' % (std_nr_moves_win, std_nr_moves_lose))

    f.write('Average number of moves for won games playing as P1/P2: %f/%f\n' % (avg_p1_win_move, avg_p2_win_move))
    f.write('Standard deviation of moves for won games playing as P1/P2: %f/%f\n\n' % (std_p1_win_move, std_p2_win_move))

    f.write('Average number of moves for lost games playing as P1/P2: %f/%f\n' % (avg_p1_lose_move, avg_p2_lose_move))
    f.write('Standard deviation of moves for lost games playing as P1/P2: %f/%f\n\n' % (std_p1_lose_move, std_p2_lose_move))

    #Times
    total_times_win = times[agent_name][WIN][PLAYER_1] + times[agent_name][WIN][PLAYER_2]
    avg_times_win = 0.0 if len(total_times_win) == 0 else statistics.fmean(total_times_win)
    std_times_win = 0.0 if len(total_times_win) < 2 else statistics.stdev(total_times_win)

    total_times_lose = times[agent_name][LOSE][PLAYER_1] + times[agent_name][LOSE][PLAYER_2]
    avg_times_lose = 0.0 if len(total_times_lose) == 0 else statistics.fmean(total_times_lose)
    std_times_lose = 0.0 if len(total_times_lose) < 2 else statistics.stdev(total_times_lose)

    avg_p1_win_times = 0.0 if len(times[agent_name][WIN][PLAYER_1]) == 0 else statistics.fmean(times[agent_name][WIN][PLAYER_1])
    std_p1_win_times = 0.0 if len(times[agent_name][WIN][PLAYER_1]) < 2 else statistics.stdev(times[agent_name][WIN][PLAYER_1])
  
    avg_p2_win_times = 0.0 if len(times[agent_name][WIN][PLAYER_2]) == 0 else statistics.fmean(times[agent_name][WIN][PLAYER_2])
    std_p2_win_times = 0.0 if len(times[agent_name][WIN][PLAYER_2]) < 2 else statistics.stdev(times[agent_name][WIN][PLAYER_2])

    avg_p1_lose_times = 0.0 if len(times[agent_name][LOSE][PLAYER_1]) == 0 else statistics.fmean(times[agent_name][LOSE][PLAYER_1])
    std_p1_lose_times = 0.0 if len(times[agent_name][LOSE][PLAYER_1]) < 2 else statistics.stdev(times[agent_name][LOSE][PLAYER_1])
  
    avg_p2_lose_times = 0.0 if len(times[agent_name][LOSE][PLAYER_2]) == 0 else statistics.fmean(times[agent_name][LOSE][PLAYER_2])
    std_p2_lose_times = 0.0 if len(times[agent_name][LOSE][PLAYER_2]) < 2 else statistics.stdev(times[agent_name][LOSE][PLAYER_2])

    f.write('Average game length won/lost games: %f/%f\n' % (avg_times_win, avg_times_lose))
    f.write('Standard deviation for game length for won/lost games: %f/%f\n\n' % (std_times_win, std_times_lose))

    f.write('Average game length for won games playing as P1/P2: %f/%f\n' % (avg_p1_win_times, avg_p2_win_times))
    f.write('Standard deviation for game length for won games playing as P1/P2: %f/%f\n\n' % (std_p1_win_times, std_p2_win_times))

    f.write('Average game length for lost games playing as P1/P2: %f/%f\n' % (avg_p1_lose_times, avg_p2_lose_times))
    f.write('Standard deviation for game length for lost games playing as P1/P2: %f/%f\n\n' % (std_p1_lose_times, std_p2_lose_times))

    f.write('Ending summary\n\n')


  def play_game(agent1, agent2):
    game.reset()
    moves = 0
    start_time = time.perf_counter()
    while(not game.is_terminal_state()):
        random_move = moves < nr_random_moves
        obj = None
        if game.get_to_move() == 0:
          obj = agent1.play(game, random_move)
        else:
          obj = agent2.play(game, random_move)
        game.make_move(obj.move)
        game.print_board()
        moves += 1
    end_time = time.perf_counter()
    winner = DRAW
    if game.winning_move():
        winner = PLAYER_1 if game.get_to_move_opponent() == 0 else PLAYER_2
    time_taken = end_time - start_time
    return winner, moves, time_taken

  agent1_name = agent1.get_name()
  agent2_name = agent2.get_name()
  
  for i in range(nr_matches):
    if (i + 1) % 10 == 0:
      print('Starting match nr: %d\n' % (i))
    result1, moves1, time1 = play_game(agent1, agent2)
    record_result(agent1_name, agent2_name, result1, moves1, time1)
    result2, moves2, time2 = play_game(agent2, agent1)
    record_result(agent2_name, agent1_name, result2, moves2, time2)

  result_file = results_path + '/' + str(agent1_name) + '_vs_' + str(agent2_name) + '.txt'
  with open(result_file, 'a') as f:
    agent1_wins = scores[agent1_name][WIN][PLAYER_1] + scores[agent1_name][WIN][PLAYER_2]
    agent2_wins = scores[agent2_name][WIN][PLAYER_1] + scores[agent2_name][WIN][PLAYER_2]
    draws = sum(list(scores[agent1_name][DRAW].values()))
    f.write('%s wins a total of: %d games, %s wins a total of: %d games, draws: %d\n\n' % (agent1_name, agent1_wins, agent2_name, agent2_wins, draws))
    print_agent_summary(agent1_name, f)
    print_agent_summary(agent2_name, f)

model1_nr = 8
model1 = load_model(model1_nr)

model2_nr = 6
model2 = load_model(model2_nr)

agent1 = MCTSAgent(game.info, 'AZ_MCTS_Agent_Model-' + str(model1_nr), simulations, model1)
agent2 = MCTSAgent(game.info, 'AZ_MCTS_Agent_Model-' + str(model2_nr), simulations, model2)
#agent2 = SimpleAgent('SimpleAgent')

play_matches(agent1, agent2)