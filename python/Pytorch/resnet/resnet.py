import numpy as np
import torch
import torch.nn as nn
import torch.tensor as tensor
import matplotlib.pyplot as plt
from torch.utils.data import Dataset, DataLoader
import torch.nn.functional as F
from torch.nn.utils import clip_grad_norm_
import torch.optim as optim

# constants
num_blocks = 19
epochs = 40
batch_size = 4
update_size = 4
learning_rate = 0.001
max_norm = 1.0
gradient_acc_step = 1

class board_data(Dataset):
    def __init__(self, dataset): # dataset = np.array of (s, p, v)
        self.X = dataset[:,0]
        self.y_p, self.y_v = dataset[:,1], dataset[:,2]
    
    def __len__(self):
        return len(self.X)
    
    def __getitem__(self,idx):
        return np.int64(self.X[idx]), self.y_p[idx], self.y_v[idx]

class ConvBlock(nn.Module):
    def __init__(self):
        super(ConvBlock, self).__init__()
        self.action_size = 7
        self.conv1 = nn.Conv2d(3, 128, 3, stride=1, padding=1)
        self.bn1 = nn.BatchNorm2d(128)
    
    def forward(self, s):
        s = s.view(-1, 3, 6, 7)  # batch_size x channels x board_x x board_y
        uno = self.conv1(s)
        dos = self.bn1(uno)
        s = F.relu(dos)
        return s


class BasicBlock(nn.Module):
    def __init__(self, in_planes = 128, planes = 128, stride=1):
        super(BasicBlock, self).__init__()
        self.conv1 = nn.Conv2d(
            in_planes, planes, kernel_size=3, stride=stride, padding=1, bias=False)
        self.bn1 = nn.BatchNorm2d(planes)
        self.conv2 = nn.Conv2d(planes, planes, kernel_size=3,
                               stride=1, padding=1, bias=False)
        self.bn2 = nn.BatchNorm2d(planes)

    def forward(self, x):
        residual = x
        out = self.conv1(x)
        out = F.relu(self.bn1(out))
        out = self.conv2(out)
        out = self.bn2(out)
        out += residual
        out = F.relu(out)
        return out

class OutputBlock(nn.Module):
    def __init__(self):
        super(OutputBlock, self).__init__()
        # Value head
        self.conv = nn.Conv2d(128, 3, 1)
        self.bn = nn.BatchNorm2d(3)
        self.fc1 = nn.Linear(3*6*7, 32)
        self.fc2 = nn.Linear(32, 1)

        # Policy head
        self.conv1 = nn.Conv2d(128, 32, 1)
        self.bn1 = nn.BatchNorm2d(32)
        self.logsoftmax = nn.LogSoftmax(dim=1)
        self.fc = nn.Linear(6*7*32, 7)

    def forward(self, s):
        # Value head
        value = F.relu(self.bn(self.conv(s)))
        value = value.view(-1, 3*6*7) # batch_size X channel X height X width
        value = F.relu(self.fc1(value))
        value = torch.tanh(self.fc2(value))

        # Policy head
        policy = F.relu(self.bn1(self.conv1(s)))
        policy = policy.view(-1, 6*7*32)
        policy = self.fc(policy)
        policy = self.logsoftmax(policy).exp()
        return policy, value

class ResNet(nn.Module):
    def __init__(self):
        super(ResNet, self).__init__()
        self.conv = ConvBlock()
        for block in range(num_blocks):
            setattr(self, "res_%i" % block, BasicBlock())
        self.outout_block = OutputBlock()

    def forward(self, s):
        s = self.conv(s)
        for block in range(num_blocks):
            s = getattr(self, "res_%i" % block)(s)
        s = self.outout_block(s)
        return s

class AlphaLoss(torch.nn.Module):
    def __init__(self):
        super(AlphaLoss, self).__init__()

    def forward(self, y_value, value, y_policy, policy):
        value_error = (value - y_value) ** 2
        policy_error = torch.sum((-policy* 
                                (1e-8 + y_policy.float()).float().log()), 1)
        total_error = (value_error.view(-1).float() + policy_error).mean()
        return total_error

arr = np.array([   
                [
                [1,1,2,1,2,1,0],
                [0,1,2,2,1,0,0],
                [0,0,2,2,1,0,0],
                [0,0,1,2,1,0,0],
                [0,0,0,2,2,0,0],
                [0,0,0,0,0,0,0],
                ],
                [
                [1,1,2,1,2,2,2],
                [1,2,1,2,2,2,1],
                [1,1,2,2,1,1,1],
                [2,0,1,2,2,2,1],
                [1,0,2,1,1,1,2],
                [1,0,2,2,0,1,0],
                ],
                [
                [2,1,1,2,1,1,2],
                [2,2,1,2,1,1,2],
                [1,1,2,2,2,1,1],
                [1,0,2,1,1,2,0],
                [1,0,1,1,2,1,0],
                [2,0,2,2,2,2,0],
                ],
                [
                [1,2,1,2,1,1,1],
                [1,0,2,0,1,0,1],
                [2,0,2,0,2,0,0],
                [2,0,1,0,2,0,0],
                [2,0,0,0,1,0,0],
                [2,0,0,0,0,0,0],
                ],
                [
                [2,2,1,1,1,2,0],
                [0,2,2,1,1,1,0],
                [0,2,1,1,1,0,0],
                [0,1,0,2,2,0,0],
                [0,2,0,2,1,0,0],
                [0,0,0,0,0,0,0],
                ],
                [
                [2,2,0,1,2,0,0],
                [1,1,0,1,0,0,0],
                [1,2,0,1,0,0,0],
                [2,0,0,1,0,0,0],
                [2,0,0,0,0,0,0],
                [0,0,0,0,0,0,0],
                ],
                [
                [2,1,1,1,2,1,2],
                [1,2,2,1,2,1,2],
                [1,2,1,2,2,1,2],
                [2,2,2,1,1,2,1],
                [0,0,1,1,2,2,0],
                [0,0,1,1,0,1,0],
                ],
                [
                [0,1,1,1,1,2,2],
                [0,0,0,0,2,0,0],
                [0,0,0,0,0,0,0],
                [0,0,0,0,0,0,0],
                [0,0,0,0,0,0,0],
                [0,0,0,0,0,0,0],
                ],
                [
                [2,1,1,2,0,2,1],
                [2,1,2,1,0,1,2],
                [2,1,2,2,0,2,0],
                [0,1,1,1,0,1,0],
                [0,0,0,0,0,0,0],
                [0,0,0,0,0,0,0],
                ],
                [
                [2,1,2,1,1,1,2],
                [2,2,1,1,1,2,2],
                [1,1,2,2,1,1,1],
                [2,2,1,1,2,1,1],
                [2,1,1,2,0,0,0],
                [0,2,2,2,0,0,0],
                ],
                [
                [2,2,0,1,2,1,2],
                [2,1,0,1,1,2,1],
                [2,1,0,2,2,1,1],
                [1,2,0,1,2,1,0],
                [2,1,0,2,1,2,0],
                [0,2,0,1,1,0,0],
                ],
                [
                [1,1,2,1,2,2,2],
                [0,0,1,2,1,1,0],
                [0,0,1,1,2,2,0],
                [0,0,2,1,1,1,0],
                [0,0,0,2,0,2,0],
                [0,0,0,0,0,0,0],
                ],
                [
                [1,2,2,1,1,1,2],
                [0,0,2,2,2,1,1],
                [0,0,0,2,2,2,1],
                [0,0,0,1,2,1,0],
                [0,0,0,0,1,0,0],
                [0,0,0,0,0,0,0],
                ],
                [
                [0,2,2,1,1,1,1],
                [0,0,0,0,2,0,0],
                [0,0,0,0,0,0,0],
                [0,0,0,0,0,0,0],
                [0,0,0,0,0,0,0],
                [0,0,0,0,0,0,0],
                ],
                [
                [1,1,2,1,2,1,0],
                [0,1,2,2,2,2,0],
                [0,0,1,0,0,0,0],
                [0,0,0,0,0,0,0],
                [0,0,0,0,0,0,0],
                [0,0,0,0,0,0,0],
                ],
                [
                [0,1,2,1,2,0,2],
                [0,0,2,1,0,0,0],
                [0,0,0,1,0,0,0],
                [0,0,0,1,0,0,0],
                [0,0,0,0,0,0,0],
                [0,0,0,0,0,0,0],
                ],
                [
                [1,2,2,2,2,1,0],
                [0,0,1,0,0,0,0],
                [0,0,1,0,0,0,0],
                [0,0,0,0,0,0,0],
                [0,0,0,0,0,0,0],
                [0,0,0,0,0,0,0],
                ],
                [
                [2,1,1,1,1,2,0],
                [0,0,0,0,0,2,0],
                [0,0,0,0,0,0,0],
                [0,0,0,0,0,0,0],
                [0,0,0,0,0,0,0],
                [0,0,0,0,0,0,0],
                ],
                [
                [2,0,1,2,1,2,2],
                [0,0,1,1,2,2,2],
                [0,0,2,2,1,1,0],
                [0,0,1,2,1,2,0],
                [0,0,1,1,1,1,0],
                [0,0,0,2,0,1,0],
                ],
                [
                [1,0,2,2,2,2,1],
                [0,0,0,0,0,1,0],
                [0,0,0,0,0,1,0],
                [0,0,0,0,0,0,0],
                [0,0,0,0,0,0,0],
                [0,0,0,0,0,0,0],
                ]
    ]
)

p_values = np.array([[0.0788, 0.1832, 0.0895, 0.2085, 0.1741, 0.1060, 0.1599],
        [0.0843, 0.1194, 0.0539, 0.1999, 0.1819, 0.2135, 0.1471],
        [0.0449, 0.1293, 0.0766, 0.1874, 0.2746, 0.1761, 0.1112],
        [0.0703, 0.1295, 0.1049, 0.2999, 0.1920, 0.1155, 0.0880],
        [0.0713, 0.1842, 0.1359, 0.2130, 0.1260, 0.1855, 0.0841],
        [0.0884, 0.1164, 0.1629, 0.2139, 0.1338, 0.1679, 0.1166],
        [0.0957, 0.1633, 0.1084, 0.1306, 0.1995, 0.1926, 0.1099],
        [0.1073, 0.1940, 0.1073, 0.2058, 0.1203, 0.1343, 0.1310],
        [0.0447, 0.2527, 0.0725, 0.2052, 0.1804, 0.1149, 0.1297],
        [0.0390, 0.1444, 0.0976, 0.1983, 0.2072, 0.2270, 0.0864],
        [0.0399, 0.1226, 0.0880, 0.2754, 0.1693, 0.1772, 0.1277],
        [0.0808, 0.2439, 0.0771, 0.2809, 0.1091, 0.1235, 0.0848],
        [0.0958, 0.1267, 0.1959, 0.1408, 0.2487, 0.0823, 0.1097],
        [0.0965, 0.1623, 0.1068, 0.1979, 0.1458, 0.1609, 0.1297],
        [0.1214, 0.1942, 0.1427, 0.1811, 0.1107, 0.1304, 0.1195],
        [0.0849, 0.1312, 0.1619, 0.2215, 0.1234, 0.1503, 0.1269],
        [0.1071, 0.1630, 0.1427, 0.2124, 0.1488, 0.1077, 0.1182],
        [0.0718, 0.1902, 0.1269, 0.2021, 0.1376, 0.1772, 0.0942],
        [0.1642, 0.1092, 0.1049, 0.1836, 0.1356, 0.1444, 0.1582],
        [0.0849, 0.1713, 0.1175, 0.1824, 0.0789, 0.1993, 0.1656]], dtype=float)

v_values = np.array([-0.0367,
        -0.1132,
        -0.1674,
        -0.2383,
        -0.1550,
        -0.1499,
        -0.0872,
        -0.0135,
        -0.1255,
        -0.2459,
        -0.1322,
        -0.1134,
        -0.2970,
        -0.0201,
        -0.0458,
        -0.0325,
        -0.0412,
        -0.0109,
        -0.0487,
        0.0777], dtype=float)

encoded = np.zeros([20,3,6,7]).astype(int)

for i0, s in enumerate(arr):
    for i in range(0, 6):
        for j in range(0, 7):
            encoded[i0][arr[i0][i][j]][i][j] = 1

dataset_values = []

for i in range(0, 20):
    dataset_values.append((encoded[i], p_values[i], v_values[i]))

test_set = board_data(np.array(dataset_values, dtype=object))

train_loader = DataLoader(test_set, batch_size=batch_size, shuffle=False, num_workers=0, pin_memory=False)

net = ResNet()
torch.manual_seed(0)
net.train()
optimizer = optim.Adam(net.parameters(), lr=learning_rate, betas=(0.8, 0.999))
scheduler = optim.lr_scheduler.MultiStepLR(optimizer, milestones=[50,100,150,200,250,300,400], gamma=0.77)
criterion = AlphaLoss()

losses_per_epoch = []
for epoch in range(epochs):
    total_loss = 0.0
    losses_per_batch = []
    for i,data in enumerate(train_loader):
        state, policy, value = data
        state, policy, value = state.float(), policy.float(), value.float()
        policy_pred, value_pred = net(state)
        loss = criterion(value_pred[:,0], value, policy_pred, policy)
        loss = loss
        loss.backward()
        clip_grad_norm_(net.parameters(), max_norm)
        optimizer.step()
        optimizer.zero_grad()

        total_loss += loss.item()
        losses_per_batch.append(gradient_acc_step*total_loss/update_size)
        print('[Epoch: %d, %5d/ %d points] total loss per batch: %.3f' %
                (epoch + 1, (i + 1)*batch_size, len(test_set), losses_per_batch[-1]))
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
plt.savefig('plots/results.png')
plt.show()



# try predict
encoded = np.zeros([1,3,6,7])
pred =  np.array([
                [
                [1,1,2,1,2,1,0],
                [0,1,2,2,1,0,0],
                [0,0,2,2,1,0,0],
                [0,0,1,2,1,0,0],
                [0,0,0,2,2,0,0],
                [0,0,0,0,0,0,0],
                ]])

for i0, s in enumerate(pred):
    for i in range(0, 6):
        for j in range(0, 7):
            encoded[i0][pred[i0][i][j]][i][j] = 1

tensor_arr = tensor(encoded,dtype=torch.float)
y_pred = net(tensor_arr)
print(y_pred)



# [samples, rows, columns, channels]