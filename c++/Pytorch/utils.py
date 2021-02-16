import numpy as np
from cppyy import ll
from array import array

def encode_for_training_1(states, turns, policies, values):
    states = encode_states(states, turns)
    policies =[list(x) for x in policies] 
    values = list(values)
    return np.array([states, np.array(policies), np.array(values)], dtype=object)


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


def encode_for_training(samples):
    last_index = len(samples) - 1
    encoded = np.zeros([last_index,3,6,7]).astype(int)
    for i in range(0, last_index):
        s = samples[i]
        for r in range(0, 6):
            for c in range(0, 7):
                if s[r][c] != 0:
                    channel = s[r][c] - 1
                    encoded[i][channel][r][c] = 1
        encoded[i,2,:,:] = samples[last_index][i]
    return encoded
    
def encode_for_predict(states, turn, n_states):
    encoded = np.zeros([n_states,3,6,7]).astype(int)
    for i in range(0, n_states):
        sp = ll.cast['int**'](states[i])
        for r in range(0, 6):
            rp = ll.cast['int*'](sp[r])
            rp.reshape((7,))
            rp = list(rp)
            for c in range(0, 7):
                if rp[c] != 0:
                    channel = rp[c] - 1
                    encoded[i][channel][r][c] = 1
        encoded[i,2,:,:] = turn
    return encoded