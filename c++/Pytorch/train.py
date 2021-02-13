import numpy as np
import torch
import matplotlib.pyplot as plt
from torch.nn.utils import clip_grad_norm_
import torch.optim as optim
from resnet import AlphaLoss 

# constants
epochs = 100
milestones = [i * 10 for i in range(1,10)]
batch_size = 4
update_size = 4
learning_rate = 0.001
max_norm = 1.0
gradient_acc_step = 1


def train_dataset(dataset, model, generation):
  torch.manual_seed(0)
  model.train()
  optimizer = optim.Adam(model.parameters(), lr=learning_rate, betas=(0.8, 0.999))
  scheduler = optim.lr_scheduler.MultiStepLR(optimizer, milestones=milestones, gamma=0.77)
  criterion = AlphaLoss()

  losses_per_epoch = []
  for epoch in range(epochs):
      total_loss = 0.0
      losses_per_batch = []
      for i,data in enumerate(dataset):
          state, policy, value = data
          state, policy, value = state.float(), policy.float(), value.float()
          policy_pred, value_pred = model(state)
          loss = criterion(value_pred[:,0], value, policy_pred, policy)
          loss = loss
          loss.backward()
          clip_grad_norm_(model.parameters(), max_norm)
          optimizer.step()
          optimizer.zero_grad()

          total_loss += loss.item()
          losses_per_batch.append(gradient_acc_step*total_loss/update_size)
          print('[Epoch: %d, %5d/ %d points] total loss per batch: %.3f' %
                  (epoch + 1, (i + 1)*batch_size, len(dataset), losses_per_batch[-1]))
          print("Policy (actual, predicted):",policy[0].argmax().item(),policy_pred[0].argmax().item())
          print("Policy data:", policy[0]); print("Policy pred:", policy_pred[0])
          print("Value (actual, predicted):", value[0].item(), value_pred[0,0].item())
          print(" ")
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