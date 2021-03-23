import numpy as np
from resnet import ResNet
import torch
from os import listdir
from os.path import isfile, join
import re

dataset_path = 'data/datasets'
model_path = 'data/models'

def latest_generation():
    files = [f for f in listdir(model_path) if isfile(join(model_path, f))]
    if len(files) == 0:
        return -1
    numbers = []
    for f in files:
        m = re.search('^model_(\d+)\.pt$', f)
        numbers.append(int(m.group(1)))
    return max(numbers)

def load_model(model, generation):
    if generation != -1:
        fpath = model_path + '/model_' + str(generation) + '.pt'
        model.load_state_dict(torch.load(fpath))
        print('Loaded model: %s' % (fpath))
    else:
        print('No model loaded')
    model.eval()

def save_model(model, generation):
    fpath = model_path + '/model_' + str(generation) + '.pt'
    torch.save(model.state_dict(), fpath)

def save_dataset(states, turns, policies, values, generation):
    data = encode_for_training(states, turns, policies, values)
    fpath = dataset_path + '/dataset_' + str(generation) + '.npy'
    np.save(fpath, data)

  # Loads dataset nr n. If n = -1, then will load latest dataset
# def load_dataset(self, dataset_n = -1):
#     if self.generation == 0:
#         return False
#     if dataset_n == -1:
#         dataset_n = self.generation - 1
#     fpath = dataset_path + '/dataset_' + str(dataset_n) + '.npy'
#     dataset = np.load(fpath, allow_pickle=True)
#     states, turns, policies, values = decode_training_data(dataset)
#     self.states = states
#     self.turns = turns
#     self.policies = policies
#     self.values = values
#     return True

# Encode utils

def encode_for_training(states, turns, policies, values):
    states = encode_states(states, turns)
    policies = np.array(policies, dtype=float)
    list_to_save = np.array([(states[i], policies[i], values[i]) for i in range (len(states))], dtype=object)
    return list_to_save

def decode_training_data(array):
    states = []
    turns = []
    policies = []
    values = []
    for sample in array:
        state, policy, value = sample
        state, turn = decode_state(state)
        states.append(state)
        turns.append(turn)
        policies.append(policy)
        values.append(value)
    return states, turns, policies, values


def decode_state(state):
    turn = state[2][0][0]
    decoded_state = np.zeros([6, 7]).astype(int)
    for r in range(0, 6):
        for c in range(0, 7):
            if state[0][r][c] == 1:
                decoded_state[r][c] = 1
            if state[1][r][c] == 1:
                decoded_state[r][c] = 2
    return decoded_state, turn

def encode_states(samples, turns):
    n_samples = len(samples)
    encoded = np.zeros([n_samples,3,6,7]).astype(int)
    for i in range(0, n_samples):
        s = samples[i]
        for r in range(0, 6):
            for c in range(0, 7):
                if s[r][c] != 0:
                    channel = s[r][c] - 1
                    encoded[i][channel][r][c] = 1
        encoded[i,2,:,:] = turns[i]
    return encoded