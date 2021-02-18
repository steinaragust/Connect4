import numpy as np
from cppyy import ll
from array import array

def encode_for_training(states, turns, policies, values):
    states = encode_states(states, turns)
    policies = np.array(policies, dtype=float)
    list_to_save = np.array([(states[i], policies[i], values[i]) for i in range (len(states))], dtype=object)
    return list_to_save

# def encode_for_training_0(samples):
#     last_index = len(samples) - 1
#     encoded = np.zeros([last_index,3,6,7]).astype(int)
#     for i in range(0, last_index):
#         s = samples[i]
#         for r in range(0, 6):
#             for c in range(0, 7):
#                 if s[r][c] != 0:
#                     channel = s[r][c] - 1
#                     encoded[i][channel][r][c] = 1
#         encoded[i,2,:,:] = samples[last_index][i]
#     return encoded


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