import cppyy
import statistics
from utils import load_model
from mcts_agent import MCTSAgent

cppyy.include('./BoardGame/C4/Connect4.cpp')

results_path = 'data/eval_results'

nr_random_moves = 4
simulations = 0
seconds = 3
model_nr = 8
nr_threads1 = 3
nr_threads2 = 1

write_stats = True

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
  number_of_simulations = {
    agent1.get_name(): { 'Move': [], 'Game': [] },
    agent2.get_name(): { 'Move': [], 'Game': [] },
  }
  states_predicted = {
    agent1.get_name(): [],
    agent2.get_name(): []
  }

  def record_result(player_1, player_2, result, moves, simulations_p1, simulations_p2, simulations_p1_avg, simulations_p2_avg, expands_p1, expands_p2):
    nonlocal scores, number_of_moves, number_of_simulations
    number_of_simulations[player_1]['Game'].append(simulations_p1)
    number_of_simulations[player_2]['Game'].append(simulations_p2)
    number_of_simulations[player_1]['Move'].append(simulations_p1_avg)
    number_of_simulations[player_2]['Move'].append(simulations_p2_avg)
    states_predicted[player_1].append(expands_p1)
    states_predicted[player_2].append(expands_p2)
    if result == DRAW:
      scores[player_1][DRAW][PLAYER_1] += 1
      scores[player_2][DRAW][PLAYER_2] += 1
    elif result == PLAYER_1:
      scores[player_1][WIN][PLAYER_1] += 1
      scores[player_2][LOSE][PLAYER_2] += 1
      number_of_moves[player_1][WIN][PLAYER_1].append(moves)
      number_of_moves[player_2][LOSE][PLAYER_2].append(moves)
    else:
      scores[player_2][WIN][PLAYER_2] += 1
      scores[player_1][LOSE][PLAYER_1] += 1
      number_of_moves[player_2][WIN][PLAYER_2].append(moves)
      number_of_moves[player_1][LOSE][PLAYER_1].append(moves)

  def print_agent_summary(agent_name, f):
    nonlocal scores, number_of_moves, number_of_simulations

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

    #Simulations
    avg_simulations_game = statistics.fmean(number_of_simulations[agent_name]['Game'])
    std_simulations_game = statistics.stdev(number_of_simulations[agent_name]['Game'])

    avg_simulations_move = statistics.fmean(number_of_simulations[agent_name]['Move'])
    std_simulations_move = statistics.stdev(number_of_simulations[agent_name]['Move'])

    f.write('Average nr simulations per game: %f\n' % (avg_simulations_game))
    f.write('Standard deviation for nr simulations per game: %f\n\n' % (std_simulations_game))
    
    f.write('Average nr simulations per move: %f\n' % (avg_simulations_move))
    f.write('Standard deviation for nr simulations per move: %f\n\n' % (std_simulations_move))

    #Expands
    avg_expands_game = statistics.fmean(states_predicted[agent_name])
    std_expands_game = statistics.stdev(states_predicted[agent_name])

    f.write('Average nr expands per move: %f\n' % (avg_expands_game))
    f.write('Standard deviation for nr expands per move: %f\n\n' % (std_expands_game))

    f.write('Ending summary\n\n')


  def play_game(agent1, agent2):
    game.reset()
    moves_p1 = 0
    moves_p2 = 0
    simulations_a1 = 0
    simulations_a2 = 0
    expands_a1 = 0
    expands_a2 = 0
    while(not game.is_terminal_state()):
        random_move = (moves_p1 + moves_p2) < nr_random_moves
        if game.get_to_move() == 0:
          obj = agent1.play(game, random_move)
          simulations_a1 += obj.simulations
          expands_a1 += obj.states_predicted
          moves_p1 += 1
        else:
          obj = agent2.play(game, random_move)
          simulations_a2 += obj.simulations
          expands_a2 += obj.states_predicted
          moves_p2 += 1
        game.make_move(obj.move)
    winner = DRAW
    if game.winning_move():
        winner = PLAYER_1 if game.get_to_move_opponent() == 0 else PLAYER_2
    return winner, moves_p1 + moves_p2, simulations_a1, simulations_a2, simulations_a1 / moves_p1, simulations_a2 / moves_p2, expands_a1 / moves_p1, expands_a2 / moves_p2

  agent1_name = agent1.get_name()
  agent2_name = agent2.get_name()
  
  for i in range(nr_matches):
    if (i + 1) % 10 == 0:
      print('Starting match nr: %d\n' % (i))
      agent1_wins = scores[agent1_name][WIN][PLAYER_1] + scores[agent1_name][WIN][PLAYER_2]
      agent2_wins = scores[agent2_name][WIN][PLAYER_1] + scores[agent2_name][WIN][PLAYER_2]
      draws = scores[agent1_name][DRAW][PLAYER_1] + scores[agent1_name][DRAW][PLAYER_2]
      print('Score so far, %s: %d, %s: %d, %s: %d\n' % (agent1_name, agent1_wins, agent2_name, agent2_wins, DRAW, draws))
    result, moves, simulations_a1, simulations_a2, simulations_a1m, simulations_a2m, expandsa1, expandsa2 = play_game(agent1, agent2)
    record_result(agent1_name, agent2_name, result, moves, simulations_a1, simulations_a2, simulations_a1m, simulations_a2m, expandsa1, expandsa2)
    result, moves, simulations_a1, simulations_a2, simulations_a1m, simulations_a2m, expandsa1, expandsa2 = play_game(agent2, agent1)
    record_result(agent2_name, agent1_name, result, moves, simulations_a1, simulations_a2, simulations_a1m, simulations_a2m, expandsa1, expandsa2)

  result_file = results_path + '/' + str(agent1_name) + '_vs_' + str(agent2_name) + '.txt'
  agent1_wins = scores[agent1_name][WIN][PLAYER_1] + scores[agent1_name][WIN][PLAYER_2]
  agent2_wins = scores[agent2_name][WIN][PLAYER_1] + scores[agent2_name][WIN][PLAYER_2]
  draws = sum(list(scores[agent1_name][DRAW].values()))
  summary = '%s wins a total of: %d games, %s wins a total of: %d games, draws: %d\n\n' % (agent1_name, agent1_wins, agent2_name, agent2_wins, draws)
  if write_stats:
    with open(result_file, 'a') as f:
      f.write(summary)
      print_agent_summary(agent1_name, f)
      print_agent_summary(agent2_name, f)

model = load_model(model_nr)
print("agent 1 has: %d threads, agent 2 has: %d threads\n" % (nr_threads1, nr_threads2))

agent1 = MCTSAgent(game.info, 'AZ_MCTS_Agent_threads-' + str(nr_threads1) + '_sec-' + str(seconds), simulations, seconds, nr_threads1, model)
agent2 = MCTSAgent(game.info, 'AZ_MCTS_Agent_threads-' + str(nr_threads2) + '_sec-' + str(seconds), simulations, seconds, nr_threads2, model)

play_matches(agent1, agent2)

seconds = 1

agent1.set_max_time_seconds(seconds)
agent1.set_name('AZ_MCTS_Agent_threads-' + str(nr_threads1) + '_sec-' + str(seconds))
agent2.set_max_time_seconds(seconds)
agent2.set_name('AZ_MCTS_Agent_threads-' + str(nr_threads2) + '_sec-' + str(seconds))
play_matches(agent1, agent2)

