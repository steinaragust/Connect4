import numpy as np
from cppyy import ll

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