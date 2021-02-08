import numpy as np
import torch
import torch.nn as nn
import torch.tensor as tensor
import torch.nn.functional as F

num_blocks = 19

class ConvBlock(nn.Module):
    def __init__(self):
        super(ConvBlock, self).__init__()
        self.number_of_columns = 7
        self.conv1 = nn.Conv2d(3, 128, 3, stride=1, padding=1)
        self.bn1 = nn.BatchNorm2d(128)
    
    def forward(self, s):
        s = s.view(-1, 3, 6, 7)  # batch_size x channels x board_x x board_y Þarf þetta ??
        s = F.relu(self.bn1(self.conv1(s)))
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

# [samples, rows, columns, channels]
encoded = np.zeros([20,3,6,7])

for i0, s in enumerate(arr):
    for i in range(0, 6):
        for j in range(0, 7):
            encoded[i0][arr[i0][i][j]][i][j] = 1

net = ResNet()
tensor_arr = tensor(encoded, dtype=torch.float)
y = net(tensor_arr)
print(y)