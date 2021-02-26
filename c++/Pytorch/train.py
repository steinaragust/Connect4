import numpy as np
import torch
import matplotlib.pyplot as plt
from torch.nn.utils import clip_grad_norm_
import torch.optim as optim
from resnet import AlphaLoss, board_data
from torch.utils.data import DataLoader

logs_path = 'data/logs'

# constants
epochs = 100
milestones = [i * 10 for i in range(1,10)]
# epochs = 10
# milestones = [2, 5, 7]
batch_size = 32
learning_rate = 0.001
max_norm = 1.0
gradient_acc_step = 1

def train_dataset(training_set, model, generation):
    nr_of_samples = len(training_set)
    generation_log_f = logs_path + '/logs_' + str(generation) + '.txt'
    with open(generation_log_f, 'a') as f:
        f.write('Training set samples: %d\n' % (nr_of_samples))
        f.write('Batch size: %d\n' % (batch_size))
    training_set = board_data(training_set)
    training_set = DataLoader(training_set, batch_size=batch_size, shuffle=False, num_workers=0, pin_memory=False)
    torch.manual_seed(0)
    model.train()
    optimizer = optim.Adam(model.parameters(), lr=learning_rate, betas=(0.8, 0.999))
    scheduler = optim.lr_scheduler.MultiStepLR(optimizer, milestones=milestones, gamma=0.77)
    criterion = AlphaLoss()
    update_size = len(training_set) // 5 if len(training_set) >= 5 else 1

    losses_per_epoch = []
    for epoch in range(epochs):
        total_loss = 0.0
        losses_per_batch = []
        for i,data in enumerate(training_set):
            state, policy, value = data
            state, policy, value = state.float(), policy.float(), value.float()
            policy_pred, value_pred = model(state)
            loss = criterion(value_pred[:,0], value, policy_pred, policy)
            loss = loss
            loss.backward()
            clip_grad_norm_(model.parameters(), max_norm)
            if epoch % gradient_acc_step == 0:
                optimizer.step()
                optimizer.zero_grad()
            total_loss += loss.item()
            if i % update_size == (update_size - 1):
                losses_per_batch.append(gradient_acc_step*total_loss/update_size)
                with open(generation_log_f, 'a') as f:
                    text = '[Epoch: %d, batch: %d/%d] total loss per batch: %.3f' % (epoch + 1, i + 1, len(training_set), losses_per_batch[-1])
                    text += '\nPolicy (actual, predicted): %d %d' % (policy[0].argmax().item(),policy_pred[0].argmax().item())
                    text += ('\nPolicy data: ' + str(policy[0]))
                    text += ('\nPolicy pred: ' + str(policy_pred[0]))
                    text += 'Value (actual, predicted): %.3f %.3f\n\n' % (value[0].item(), value_pred[0,0].item())
                    f.write(text)
                total_loss = 0.0
        scheduler.step()
        if len(losses_per_batch) >= 1:
            losses_per_epoch.append(sum(losses_per_batch)/len(losses_per_batch))

    fig = plt.figure()
    ax = fig.add_subplot(222)
    ax.scatter([e for e in range(0, len(losses_per_epoch))], losses_per_epoch)
    ax.set_xlabel("Epoch")
    ax.set_ylabel("Loss per batch")
    ax.set_title("Loss vs Epoch")
    plt.savefig('data/plots/loss_%d.png' % (generation))
    plt.clf()