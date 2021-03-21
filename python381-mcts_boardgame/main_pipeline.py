import cppyy
from mcts_self_trainer import MCTS_Self_Trainer
from train import train_dataset

cppyy.include('./Connect4.cpp')

game = cppyy.gbl.Connect4.Connect4()
trainer = MCTS_Self_Trainer(game, 300, 4)

def train(nr_generations, nr_matches):
  start_generation = trainer.generation
  for g in range(start_generation, start_generation + nr_generations):
    # One iteration
    print('Generation %d starting' % (g))
    trainer.play_games(nr_matches)
    data = trainer.get_training_set()
    train_dataset(data, trainer.model, trainer.generation)
    trainer.save_latest_dataset()
    trainer.save_latest_model()
    trainer.next_generation()

train(1, 200)
