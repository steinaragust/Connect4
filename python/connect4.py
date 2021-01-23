import numpy as np


class Connect4:
    PLAYER_1 = 0
    PLAYER_2 = 1

    OPPONENT = [PLAYER_2, PLAYER_1]

    EMPTY = 0
    PLAYER_1_PIECE = 1
    PLAYER_2_PIECE = 2
    ROW_COUNT = 6
    COLUMN_COUNT = 7
    WINDOW_LENGTH = 4

    PIECES = [PLAYER_1_PIECE, PLAYER_2_PIECE]

    def __init__(self):
        self.setup()
        return

    def get_move_no(self):
        return self._no_moves

    def get_board(self):
        return self.board

    def setup(self):
        self.board = self.create_board()
        self.turn = self.PLAYER_1
        self._no_moves = 0

    def get_piece(self):
        if self.turn == self.PLAYER_1:
            return self.PLAYER_1_PIECE
        return self.PLAYER_2_PIECE

    def get_opponent_piece(self):
        if self.turn == self.PLAYER_1:
            return self.PLAYER_2_PIECE
        return self.PLAYER_1_PIECE

    def create_board(self):
        board = np.zeros((self.ROW_COUNT, self.COLUMN_COUNT))
        return board

    def drop_piece_in_column(self, col):
        row = self.get_next_open_row(col)
        piece = self.get_piece()
        self.set_piece(row, col, piece)
        self._no_moves += 1

    def retract_piece_in_column(self, col):
        row = self.get_next_open_row(col)
        self.set_piece(
            (self.ROW_COUNT if row is None else row) - 1, col, self.EMPTY)
        self._no_moves -= 1

    def set_piece(self, row, col, piece):
        self.board[row][col] = piece
        self.turn = self.OPPONENT[self.turn]

    def is_valid_location(self, col):
        return self.board[self.ROW_COUNT - 1][col] == 0

    def get_next_open_row(self, col):
        for r in range(self.ROW_COUNT):
            if self.board[r][col] == 0:
                return r

    def print_board(self):
        print(np.flip(self.board, 0))

    def winning_move(self):
        # Check horizontal locations for win
        for piece in self.PIECES:
            for c in range(self.COLUMN_COUNT - 3):
                for r in range(self.ROW_COUNT):
                    if (
                        self.board[r][c] == piece
                        and self.board[r][c + 1] == piece
                        and self.board[r][c + 2] == piece
                        and self.board[r][c + 3] == piece
                    ):
                        return True

            # Check vertical locations for win
            for c in range(self.COLUMN_COUNT):
                for r in range(self.ROW_COUNT - 3):
                    if (
                        self.board[r][c] == piece
                        and self.board[r + 1][c] == piece
                        and self.board[r + 2][c] == piece
                        and self.board[r + 3][c] == piece
                    ):
                        return True

            # Check positively sloped diaganols
            for c in range(self.COLUMN_COUNT - 3):
                for r in range(self.ROW_COUNT - 3):
                    if (
                        self.board[r][c] == piece
                        and self.board[r + 1][c + 1] == piece
                        and self.board[r + 2][c + 2] == piece
                        and self.board[r + 3][c + 3] == piece
                    ):
                        return True

            # Check negatively sloped diaganols
            for c in range(self.COLUMN_COUNT - 3):
                for r in range(3, self.ROW_COUNT):
                    if (
                        self.board[r][c] == piece
                        and self.board[r - 1][c + 1] == piece
                        and self.board[r - 2][c + 2] == piece
                        and self.board[r - 3][c + 3] == piece
                    ):
                        return True
        return False

    def is_terminal_node(self):
        return (
            self.winning_move()
            or len(self.get_valid_locations()) == 0
        )

    def get_valid_locations(self):
        valid_locations = []
        for col in range(self.COLUMN_COUNT):
            if self.is_valid_location(col):
                valid_locations.append(col)
        return valid_locations

    def get_to_move(self):
        return self.turn
