import numpy as np

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
    