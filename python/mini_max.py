import math
import random


def evaluate_window(window, game):
    score = 0
    piece = game.get_piece()
    opp_piece = game.get_opponent_piece()

    if window.count(piece) == 4:
        score += 100
    elif window.count(piece) == 3 and window.count(game.EMPTY) == 1:
        score += 5
    elif window.count(piece) == 2 and window.count(game.EMPTY) == 2:
        score += 2

    if window.count(opp_piece) == 3 and window.count(game.EMPTY) == 1:
        score -= 4

    return score


def score_position(game):
    score = 0

    board = game.get_board()
    piece = game.get_piece()

    # Score center column
    center_array = [int(i) for i in list(board[:, game.COLUMN_COUNT//2])]
    center_count = center_array.count(piece)
    score += center_count * 3

    # Score Horizontal
    for r in range(game.ROW_COUNT):
        row_array = [int(i) for i in list(board[r, :])]
        for c in range(game.COLUMN_COUNT-3):
            window = row_array[c:c+game.WINDOW_LENGTH]
            score += evaluate_window(window, game)

    # Score Vertical
    for c in range(game.COLUMN_COUNT):
        col_array = [int(i) for i in list(board[:, c])]
        for r in range(game.ROW_COUNT-3):
            window = col_array[r:r+game.WINDOW_LENGTH]
            score += evaluate_window(window, game)

    # Score positive sloped diagonal
    for r in range(game.ROW_COUNT-3):
        for c in range(game.COLUMN_COUNT-3):
            window = [board[r+i][c+i] for i in range(game.WINDOW_LENGTH)]
            score += evaluate_window(window, game)

    for r in range(game.ROW_COUNT-3):
        for c in range(game.COLUMN_COUNT-3):
            window = [board[r+3-i][c+i] for i in range(game.WINDOW_LENGTH)]
            score += evaluate_window(window, game)

    return score


def minimax(game, depth, alpha, beta, maximizingPlayer):
    valid_locations = game.get_valid_locations()
    is_terminal = game.is_terminal_node()
    if depth == 0 or is_terminal:
        score = 0
        if is_terminal:
            if game.winning_move():
                score = 100000000000000
            else:  # Game is over, no more valid moves
                score = 0
        else:  # Depth is zero
            score = score_position(game)
        return (None, (1 if maximizingPlayer else -1) * score)
    if maximizingPlayer:
        value = -math.inf
        column = random.choice(valid_locations)
        for col in valid_locations:
            game.drop_piece_in_column(col)
            new_score = minimax(game, depth-1, alpha, beta, False)[1]
            game.retract_piece_in_column(col)
            if new_score > value:
                value = new_score
                column = col
            alpha = max(alpha, value)
            if alpha >= beta:
                break
        return column, value

    else:  # Minimizing player
        value = math.inf
        column = random.choice(valid_locations)
        for col in valid_locations:
            game.drop_piece_in_column(col)
            new_score = minimax(game, depth-1, alpha, beta, True)[1]
            game.retract_piece_in_column(col)
            if new_score < value:
                value = new_score
                column = col
            beta = min(beta, value)
            if alpha >= beta:
                break
        return column, value
