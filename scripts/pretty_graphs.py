import pandas as pd, seaborn as sns, matplotlib.pyplot as plt
from os import makedirs
from os.path import exists

sns.set(font_scale=1.5) # Adjust this if necessary

CVRP_FILES = [
    ('data/cvrp-benchmarks/greedy_analysis.csv',        'Greedy'),
    ('data/cvrp-benchmarks/gts_analysis.csv',           'Tabu Search'),
    ('data/cvrp-benchmarks/sa_analysis.csv',            'Simulated Annealing'),
    ('data/cvrp-benchmarks/aco_analysis.csv',           'Ant Colony Optimization'),
    ('data/cvrp-benchmarks/clarke_wright_analysis.csv', 'Clarke Wright Savings'),
    ('data/cvrp-benchmarks/sa_clarke_wright.csv',       'Simulated Annealing (Clarke Wright)'),
    ('data/cvrp-benchmarks/sa_greedy.csv',              'Simulated Annealing (greedy)'),
    ('data/cvrp-benchmarks/sa_trivial.csv',             'Simulated Annealing (trivial)'),
]

def get_data() -> pd.DataFrame:
    dataframes = []

    for path, approach in CVRP_FILES:
        dt = pd.read_csv(path)
        dt['method'] = approach

        dataframes.append(
            dt
        )

    return pd.concat(dataframes, ignore_index=True)

if __name__ == '__main__':
    data = get_data()

    data['time_us'] /= 1000
    data['solution_length'] /= 1000
    data = data.rename(columns={'name': 'instance'})

    data_full = data.copy()

    sa_specific = {
        'Simulated Annealing (Clarke Wright)',
        'Simulated Annealing (greedy)',
        'Simulated Annealing (trivial)'
    }

    data = data[~data['method'].isin(sa_specific)]
    data['bar_plot_labels'] = data.apply(lambda row: f'{row.instance} ({row.num_deliveries})', axis=1)

    if not exists('data/pretty_graphs'):
        makedirs('data/pretty_graphs')

    figsize = (22, 11)
    i = 1
    plt.figure(figsize=figsize)
    ax = sns.lineplot(data=data, x='num_deliveries', y='time_us', hue='method', lw=4)
    ax.set(
        title='Total Processing Time (lower is better)',
        xlabel='Number of Delivery Locations',
        ylabel='Processing Time (ms)'
    )
    ax.legend(title='Method')
    plt.savefig(f'data/pretty_graphs/graph{i}.png', bbox_inches='tight')

    i += 1
    plt.figure(figsize=figsize)
    ax = sns.lineplot(data=data, x='num_deliveries', y='time_us', hue='method', lw=4)
    ax.set(
        title='Total Processing Time (lower is better)',
        xlabel='Number of Delivery Locations',
        ylabel='Processing Time (ms, log scale)',
        yscale='log'
    )
    ax.legend(title='Method')
    plt.savefig(f'data/pretty_graphs/graph{i}.png', bbox_inches='tight')

    i += 1
    plt.figure(figsize=figsize)
    ax = sns.barplot(data=data, x='bar_plot_labels', y='solution_length', hue='method')
    ax.set(
        title='Total Solution Length (lower is better)',
        xlabel='CVRP Instance (Number of Deliveries)',
        ylabel='Solution Length (km)'
    )
    ax.legend(title='Method')
    plt.savefig(f'data/pretty_graphs/graph{i}.png', bbox_inches='tight')

    i += 1
    plt.figure(figsize=figsize)
    ax = sns.barplot(data=data, x='bar_plot_labels', y='num_vehicles', hue='method')
    ax.set(
        title='Number of Vehicles Used (lower is better)',
        xlabel='CVRP Instance (Number of Deliveries)',
        ylabel='Vehicles Used'
    )
    ax.legend(title='Method')
    plt.savefig(f'data/pretty_graphs/graph{i}.png', bbox_inches='tight')

    i += 1
    plt.figure(figsize=figsize)
    ax = sns.barplot(data=data, x='bar_plot_labels', y='average_load', hue='method')
    ax.set(
        title='Average Vehicle Load (higher is better)',
        xlabel='CVRP Instance (Number of Deliveries)',
        ylabel='Average Vehicle Load'
    )
    ax.legend(title='Method', loc='lower right')
    plt.savefig(f'data/pretty_graphs/graph{i}.png', bbox_inches='tight')

    instances = list(data[data['method'] == 'Greedy']['instance'])

    for instance in instances:
        i += 1
        instance_data = data[data['instance'] == instance]
        plt.figure(figsize=figsize)

        ax = sns.scatterplot(data=instance_data, x='time_us', y='solution_length', hue='method', s=200)
        ax.set(
            title=f'Processing Time vs. Solution Length ({instance})',
            xlabel='Processing Time (ms, log scale)',
            ylabel='Solution Length (km)',
            ylim=(0, None),
            xscale='log'
        )
        ax.legend(title='Method', loc='lower right')

        plt.savefig(f'data/pretty_graphs/graph{i}.png', bbox_inches='tight')
