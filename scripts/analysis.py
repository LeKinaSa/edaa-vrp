
import pandas as pd
import seaborn as sb
import numpy as np
import matplotlib.pyplot as plt

from copy import deepcopy
from math import log

def parse_tree_file(path):
    idx = []
    kd_tree = { 'C': {}, 'NN': {} }
    quadtree = { 'C': {}, 'NN': {} }
    construction = True
    curr_idx = None

    def add_time(d, k, t):
        if curr_idx in d[k]:
            d[k][curr_idx].append(t)
        else:
            d[k][curr_idx] = [t]

    with open(path, 'r') as f:
        for line in f.readlines():
            line = line.strip()
            if line.startswith('>'):
                curr_idx = int(line[1:])
                idx.append(curr_idx)
                construction = True
            elif construction:
                if line == 'NN':
                    construction = False
                else:
                    t = list(map(int, line.split()))
                    add_time(kd_tree, 'C', t[0])
                    add_time(quadtree, 'C', t[1])
            else:
                t = list(map(int, line.split()))
                add_time(kd_tree, 'NN', t[0])
                add_time(quadtree, 'NN', t[1])
    
    return idx, kd_tree, quadtree

idx, kd_tree, quadtree = parse_tree_file('tree_ca.txt')

base = {'K': {}, 'Q': {}}
minimum = deepcopy(base)
q25 = deepcopy(base)
med = deepcopy(base)
avg = deepcopy(base)
q75 = deepcopy(base)
maximum = deepcopy(base)

tree_map = {'K': kd_tree, 'Q': quadtree}
for key, tree in tree_map.items():
    for op in ['C', 'NN']:
        df = pd.DataFrame(tree[op])

        minimum[key][op] = df.min()
        minimum[key][op].name = 'Minimum'

        q25[key][op] = df.quantile(0.25)
        q25[key][op].name = '25th Quartile'

        med[key][op] = df.median()
        med[key][op].name = 'Median'

        avg[key][op] = df.mean()
        avg[key][op].name = 'Average'

        q75[key][op] = df.quantile(0.75)
        q75[key][op].name = '75th Quartile'

        maximum[key][op] = df.max()
        maximum[key][op].name = 'Maximum'

# Nearest Neighbor plot

titles = {'K': 'K-d Tree', 'Q': 'Quadtree'}
op_titles = {'C': 'Construction', 'NN': 'Nearest Neighbor'}
units = {'C': 'microseconds', 'NN': 'nanoseconds'}

# for k, v in titles.items():
#     for opk, opv in op_titles.items():
#         df = pd.concat([minimum[k][opk], q25[k][opk], med[k][opk],
#             avg[k][opk], q75[k][opk]], axis=1)

#         #print(df)

#         ax = sb.lineplot(data=df, palette=['crimson', 'tomato', 'orange', 'black', 'gold'],
#             dashes=False, linewidth=2.0)
#         plt.fill_between(idx, q25[k][opk], med[k][opk], color='tomato', alpha=0.25)
#         plt.fill_between(idx, med[k][opk], q75[k][opk], color='orange', alpha=0.25)
#         ax.set_title(v + ' - ' + opv)
#         ax.set_xlabel('Points')
#         ax.set_ylabel('Execution Time (' + units[opk] + ')')
#         plt.show()

# df = pd.concat([avg['K']['C'] / 1000, avg['Q']['C'] / 1000], axis=1)
# df.columns = ['K-d Tree', 'Quadtree']
# ax = sb.lineplot(data=df, dashes=False, linewidth=2.0)
# ax.set_title('Construction')
# ax.set_xlabel('Points')
# ax.set_ylabel('Execution Time (milliseconds)')
# plt.show()

# df = pd.DataFrame([kd_tree['NN'][idx[-1]], quadtree['NN'][idx[-1]]]).transpose()
# df.columns = ['K-d Tree', 'Quadtree']
# df = pd.melt(df)
# ax = sb.boxplot(data=df, x='variable', y='value', showfliers=False)
# ax.set_title('Nearest Neighbor')
# ax.set_xlabel('Data Structure')
# ax.set_ylabel('Execution Time (nanoseconds)')
# plt.show()


def parse_heap_file(path):
    data = {
        'B': {'I': {}, 'EM': {}, 'DK': {}},
        'F': {'I': {}, 'EM': {}, 'DK': {}}
    }
    curr_op = None
    curr_idx = None

    def add_time(d, ds, op, t):
        if curr_idx in d[ds][op]:
            d[ds][op][curr_idx].append(t)
        else:
            d[ds][op][curr_idx] = [t]

    with open(path, 'r') as f:
        for line in f.readlines():
            line = line.strip()
            if line.startswith('>'):
                curr_idx = int(line[1:])
            elif line in data['B'].keys():
                curr_op = line
            else:
                t = list(map(int, line.split()))
                add_time(data, 'B', curr_op, t[0])
                add_time(data, 'F', curr_op, t[1])

    return data

data = parse_heap_file("heap_ca.txt")
idx = list(data['F']['I'].keys())

titles = {'B': 'Binary Heap', 'F': 'Fibonacci Heap'}
op_titles = {'I': 'Insertion', 'EM': 'Extract Min', 'DK': 'Decrease Key'}

base = {'F': {'I': {}, 'EM': {}, 'DK': {}}, 'B': {'I': {}, 'EM': {}, 'DK': {}}}
minimum = deepcopy(base)
q25 = deepcopy(base)
med = deepcopy(base)
avg = deepcopy(base)
q75 = deepcopy(base)
maximum = deepcopy(base)

for ds, ops in data.items():
    for op, times in ops.items():
        df = pd.DataFrame(times)

        minimum[ds][op] = df.min()
        minimum[ds][op].name = 'Minimum'

        q25[ds][op] = df.quantile(0.25)
        q25[ds][op].name = '25th Quartile'

        med[ds][op] = df.median()
        med[ds][op].name = 'Median'

        avg[ds][op] = df.mean()
        avg[ds][op].name = 'Average'

        q75[ds][op] = df.quantile(0.75)
        q75[ds][op].name = '75th Quartile'

        maximum[ds][op] = df.max()
        maximum[ds][op].name = 'Maximum'

# for dsk, dsv in titles.items():
#     for opk, opv in op_titles.items():
#         df = pd.concat([minimum[dsk][opk], q25[dsk][opk], med[dsk][opk],
#             avg[dsk][opk], q75[dsk][opk]], axis=1)

#         print(df)

#         ax = sb.lineplot(data=df, palette=['crimson', 'tomato', 'orange', 'black', 'gold'],
#             dashes=False, linewidth=2.0)
#         plt.fill_between(idx, q25[dsk][opk], med[dsk][opk], color='tomato', alpha=0.2)
#         plt.fill_between(idx, med[dsk][opk], q75[dsk][opk], color='orange', alpha=0.2)
#         ax.set_title(dsv + ' - ' + opv)
#         ax.set_xlabel('Nodes')
#         ax.set_ylabel('Execution Time (nanoseconds)')
#         plt.show()

# df = pd.DataFrame([kd_tree['NN'][idx[-1]], quadtree['NN'][idx[-1]]]).transpose()
# df.columns = ['K-d Tree', 'Quadtree']
# df = pd.melt(df)
# ax = sb.boxplot(data=df, x='variable', y='value', showfliers=False)
# ax.set_title('Nearest Neighbor')
# ax.set_xlabel('Data Structure')
# ax.set_ylabel('Execution Time (nanoseconds)')
# plt.show()

# with open('matching_kd.txt', 'r') as f:
#     kd_data = list(map(int, f.read().split()))
# with open('matching_quad.txt', 'r') as f:
#     quad_data = list(map(int, f.read().split()))

# df = pd.DataFrame([kd_data, quad_data]).transpose()
# df.columns = ['K-d Tree', 'Quadtree']
# df = pd.melt(df)

# ax = sb.boxplot(data=df, x='variable', y='value', showfliers=False)
# ax.set_xlabel('Data Structure')
# ax.set_ylabel('Average Execution Time (nanoseconds)')
# ax.set_title('Nearest Neighbor (Real Data)')
# plt.show()

# with open('shortest_paths_bin.txt', 'r') as f:
#     bin_data = list(map(int, f.read().split()))
# with open('shortest_paths_fib.txt', 'r') as f:
#     fib_data = list(map(int, f.read().split()))

# df = pd.DataFrame([bin_data, fib_data]).transpose()
# df.columns = ['Binary Heap', 'Fibonacci Heap']
# df /= 1000
# df = pd.melt(df)

# ax = sb.boxplot(data=df, x='variable', y='value', showfliers=False)
# ax.set_xlabel('Data Structure')
# ax.set_ylabel('Execution Time (milliseconds)')
# ax.set_title('Dijkstra\'s Algorithm - Priority Queue Comparison')
# plt.show()

# --- Memory consumption plots ---

x = np.linspace(0, 1000000, 101)

# Quadtrees vs K-d Trees

# D = 10km, s = 100m
yq1 = 72 * x * log(100, 4)
# D = 10km, s = 10m
yq2 = 72 * x * log(1000, 4)
# Optimized Quadtree
yqo = 56 * x

yk = 24 * x

df = pd.DataFrame([yq1, yq2, yqo, yk]).transpose()
df.index = x

# ax = sb.lineplot(data=df, palette='husl', dashes=False, linewidth=2.0)
# ax.set_title('Memory Usage Comparison')
# ax.legend(['Quadtree (D/s = 100)', 'Quadtree (D/s = 1000)', 'Compressed Quadtree w/ Optimizations', 'K-d Tree'])
# ax.set_xlabel('Points')
# ax.set_ylabel('Memory Usage')
# plt.show()

# Binary vs Fibonacci Heaps

yb = 32 * x
yf = 50 * x

df = pd.DataFrame([yb, yf]).transpose()
df.index = x

# ax = sb.lineplot(data=df, palette='husl', dashes=False, linewidth=2.0)
# ax.set_title('Memory Usage Comparison')
# ax.legend(['Binary Heap', 'Fibonacci Heap'])
# ax.set_xlabel('Points')
# ax.set_ylabel('Memory Usage')
# plt.show()

# df = pd.DataFrame({
#     'test': ['Belém, 357 Locations', 'Brasília, 853 Locations', 'Rio, 1901 Locations'],
#     'Fibonacci Heap': [5.955, 104.206, 179.589],
#     'Binary Heap': [6.845, 105.907, 181.507],
# })

# df = pd.melt(df, id_vars='test', var_name='Data Structure')

# ax = sb.barplot(data=df, palette='husl', x='test', y='value', hue='Data Structure')
# ax.set_title('Dijkstra Performance using Fibonacci and Binary Heaps')
# ax.set_xlabel('Instance')
# ax.set_ylabel('Execution Time (seconds)')
# for c in ax.containers:
#     ax.bar_label(c)
# plt.show()

with open('parallelism/sp_fib_total.txt') as f:
    fib_total = list(map(int, f.read().strip().split()))
with open('parallelism/sp_bin_total.txt') as f:
    bin_total = list(map(int, f.read().strip().split()))

div = lambda x: x / 1000
fib_total = list(map(div, fib_total))
bin_total = list(map(div, bin_total))

df = pd.DataFrame([
    list(range(1, 17)), fib_total, bin_total
]).transpose()
df.columns = ['Threads', 'Fibonacci Heap', 'Binary Heap']
df = pd.melt(df, id_vars='Threads', var_name='Data Structure', value_name='Execution Time')

ax = sb.lineplot(data=df, x='Threads', y='Execution Time',
    hue='Data Structure', palette='bright', linewidth=2.0)
ax.set_xticks(list(range(1, 17)))
ax.set_title('Parallelism Analysis for Dijkstra\'s Algorithm - Total Time')
ax.set_ylabel('Execution Time (Seconds)')
plt.show()

data = {
    'Threads': [],
    'Data Structure': [],
    'Execution Time': [],
}

for i in range(1, 17):
    with open(f'parallelism/shortest_paths_f{i}.txt') as f:
        fib_heap = list(map(int, f.read().strip().split()))
        for t in fib_heap:
            data['Threads'].append(i)
            data['Data Structure'].append('Fibonacci Heap')
            data['Execution Time'].append(t / 1000)

    with open(f'parallelism/shortest_paths_b{i}.txt') as f:
        bin_heap = list(map(int, f.read().strip().split()))
        for t in bin_heap:
            data['Threads'].append(i)
            data['Data Structure'].append('Binary Heap')
            data['Execution Time'].append(t / 1000)

df = pd.DataFrame(data)

ax = sb.boxplot(data=df, x='Threads', y='Execution Time', hue='Data Structure', palette='bright', showfliers=False)
ax.set_title('Parallelism Analysis for Dijkstra\'s Algorithm - Individual Time')
ax.set_ylabel('Execution Time (Milliseconds)')
plt.show()
