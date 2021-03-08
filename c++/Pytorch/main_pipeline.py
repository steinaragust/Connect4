from mcts_trainer import MCTS_Trainer
from train import train_dataset
from utils import encode_for_predict

def predict(values, states, turn, n_states):
  encoded = encode_for_predict(states, turn, n_states)
  trainer.predict(encoded, values, n_states)

trainer = MCTS_Trainer(predict, 600)

def train(nr_generations, nr_matches):
  start_generation = trainer.generation
  for g in range(start_generation, nr_generations):
    # One iteration
    print('Generation %d starting' % (g))
    trainer.play_matches(nr_matches)
    data = trainer.get_training_set()
    train_dataset(data, trainer.model, trainer.generation)
    trainer.save_latest_dataset()
    trainer.save_latest_model()
    trainer.next_generation()

train(11, 100)
