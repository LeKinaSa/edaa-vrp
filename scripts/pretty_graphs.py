import pandas as pd, seaborn as sns, matplotlib.pyplot as plt

sns.set(font_scale=1.5) # Adgust this if necessary

CVRP_FILES = [
    ("data/cvrp-benchmarks/greedy_analysis.csv",        "Greedy"),
    ("data/cvrp-benchmarks/gts_analysis.csv",           "Tabu Search"),
    ("data/cvrp-benchmarks/sa_analysis.csv",            "Simulated Annealing"),
    ("data/cvrp-benchmarks/aco_analysis.csv",           "Ant Colony Optimization"),
    ("data/cvrp-benchmarks/clarke_wright_analysis.csv", "Clarke Wright Savings"),
    ("data/cvrp-benchmarks/cp_analysis.csv",            "Constraint Programming"),
    ("data/cvrp-benchmarks/sa_clarke_wright.csv",       "Simulated Annealing (Clarke Wright)"),
    ("data/cvrp-benchmarks/sa_greedy.csv",              "Simulated Annealing (greedy)"),
    ("data/cvrp-benchmarks/sa_trivial.csv",             "Simulated Annealing (trivial)"),
]

def get_data() -> pd.DataFrame:
    dataframes = []

    for path, approach in CVRP_FILES:
        dt = pd.read_csv(path)
        dt["method"] = approach

        dataframes.append(
            dt
        )
    
    return pd.concat(dataframes, ignore_index=True)

if __name__ == "__main__":
    data = get_data()
    print(data)
    print(data[data["method"] == "Constraint Programming"])

    sns.lineplot(data=data, x="num_deliveries", y="time_us", hue="method", lw=4).set(title="Time performance (μs) (lower is better)")
    plt.show()

    sns.barplot(data=data, x="num_deliveries", y="solution_length", hue="method").set(title="Solution quality (lower is better)")
    plt.show()

    sns.barplot(data=data, x="num_deliveries", y="num_vehicles", hue="method").set(title="Number of trucks (lower is better)")
    plt.show()