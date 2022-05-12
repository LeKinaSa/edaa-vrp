from ortools.sat.python import cp_model

def create_data_model():
    """Stores the data for the problem."""
    data = {}
    data['distance_matrix'] = [
        [0,   548,  776,  696,  582,  274,  502,  194, 308,  194, 536,  502,  388,  354,  468,  776,  662 ],
        [548, 0,    684,  308,  194,  502,  730,  354, 696,  742, 1084, 594,  480,  674,  1016, 868,  1210],
        [776, 684,  0,    992,  878,  502,  274,  810, 468,  742, 400,  1278, 1164, 1130, 788,  1552, 754 ],
        [696, 308,  992,  0,    114,  650,  878,  502, 844,  890, 1232, 514,  628,  822,  1164, 560,  1358],
        [582, 194,  878,  114,  0,    536,  764,  388, 730,  776, 1118, 400,  514,  708,  1050, 674,  1244],
        [274, 502,  502,  650,  536,  0,    228,  308, 194,  240, 582,  776,  662,  628,  514,  1050, 708 ],
        [502, 730,  274,  878,  764,  228,  0,    536, 194,  468, 354,  1004, 890,  856,  514,  1278, 480 ],
        [194, 354,  810,  502,  388,  308,  536,  0,   342,  388, 730,  468,  354,  320,  662,  742,  856 ],
        [308, 696,  468,  844,  730,  194,  194,  342, 0,    274, 388,  810,  696,  662,  320,  1084, 514 ],
        [194, 742,  742,  890,  776,  240,  468,  388, 274,  0,   342,  536,  422,  388,  274,  810,  468 ],
        [536, 1084, 400,  1232, 1118, 582,  354,  730, 388,  342, 0,    878,  764,  730,  388,  1152, 354 ],
        [502, 594,  1278, 514,  400,  776,  1004, 468, 810,  536, 878,  0,    114,  308,  650,  274,  844 ],
        [388, 480,  1164, 628,  514,  662,  890,  354, 696,  422, 764,  114,  0,    194,  536,  388,  730 ],
        [354, 674,  1130, 822,  708,  628,  856,  320, 662,  388, 730,  308,  194,  0,    342,  422,  536 ],
        [468, 1016, 788,  1164, 1050, 514,  514,  662, 320,  274, 388,  650,  536,  342,  0,    764,  194 ],
        [776, 868,  1552, 560,  674,  1050, 1278, 742, 1084, 810, 1152, 274,  388,  422,  764,  0,    798 ],
        [662, 1210, 754,  1358, 1244, 708,  480,  856, 514,  468, 354,  844,  730,  536,  194,  798,  0   ],
    ]
    data['demands'] = [0, 1, 1, 2, 4, 2, 4, 8, 8, 1, 2, 1, 2, 4, 4, 8, 8]
    data['vehicle_capacity'] = 15
    return data

def diagonals_must_be_zero(model : cp_model.CpModel, matrix, n_vehicles, n_locations):
    for v in range(n_vehicles):
        for i in range(n_locations):
            model.Add(matrix[v][i][i] == 0)

def enforce_matrix_usage(model : cp_model.CpModel, matrix, matrix_usage, n_vehicles, n_locations):
    for v in range(n_vehicles):
        origin_row_sum    = sum(matrix[v][0][i] for i in range(n_locations))
        origin_column_sum = sum(matrix[v][i][0] for i in range(n_locations))

        model.Add(origin_row_sum    == matrix_usage[v])
        model.Add(origin_column_sum == matrix_usage[v])

        matrix_sum = sum(matrix[v][i][j] for i in range(n_locations) for j in range(n_locations))

        model.Add(matrix_sum <= matrix_usage[v] * n_locations)

def all_destinations_must_be_visited(model : cp_model.CpModel, matrix, n_vehicles, n_locations):
    for i in range(n_locations):
        for j in range(n_locations):
            s = sum(matrix[v][i][j] for v in range(n_vehicles))
            model.Add(s <= 1) # This one is more of a sanity check
    
    for i in range(1, n_locations):
        s = sum(matrix[v][i][j] for v in range(n_vehicles) for j in range(n_locations))
        model.Add(s == 1)
    
    for j in range(1, n_locations):
        s = sum(matrix[v][i][j] for v in range(n_vehicles) for i in range(n_locations))
        model.Add(s == 1)

def paths_must_make_sense(model : cp_model.CpModel, matrix, n_vehicles, n_locations):
    for v in range(n_vehicles):
        for i in range(n_locations):
            i_row_sum = sum(matrix[v][i][j] for j in range(n_locations))
            i_col_sum = sum(matrix[v][j][i] for j in range(n_locations))
            model.Add(i_col_sum == i_row_sum)

def minimize_travel_time(model : cp_model.CpModel, matrix, n_vehicles, n_locations, distance_matrix):
    min_sum = 0
    for v in range(n_vehicles):
        for i in range(n_locations):
            for j in range(n_locations):
                min_sum += matrix[v][i][j] * distance_matrix[i][j]
    
    model.Minimize(min_sum)

def enforce_maximum_demands(model : cp_model.CpModel, matrix, n_vehicles, n_locations, demands, vehicle_capacity):
    for v in range(n_vehicles):
        demand_sum = 0
        for i in range(n_locations):
            for j in range(n_locations):
                demand_sum += matrix[v][i][j] * demands[i]
        model.Add(demand_sum <= vehicle_capacity)


def solve(data_model):
    model = cp_model.CpModel()

    n_locations = len(data_model["distance_matrix"])
    n_vehicles = 5 # data_model['vehicle_capacity']

    matrix = [] # matrix[vehicle][from][to]
    for v in range(n_vehicles):
        square = []

        for i in range(n_locations):
            square.append([model.NewBoolVar(f"{v}-{i}-{j}") for j in range(n_locations)])
        matrix.append(square)
    
    matrix_usage = [model.NewBoolVar(f"u-{l}") for l in range(n_vehicles)]

    diagonals_must_be_zero(model, matrix, n_vehicles, n_locations)
    enforce_matrix_usage  (model, matrix, matrix_usage, n_vehicles, n_locations)
    all_destinations_must_be_visited(model, matrix, n_vehicles, n_locations)
    paths_must_make_sense(model, matrix, n_vehicles, n_locations)
    minimize_travel_time(model, matrix, n_vehicles, n_locations, data_model["distance_matrix"])
    enforce_maximum_demands(model, matrix, n_vehicles, n_locations, data_model["demands"], data_model['vehicle_capacity'])

    solver = cp_model.CpSolver()
    status = solver.Solve(model)

    if status == cp_model.OPTIMAL or status == cp_model.FEASIBLE:
        print("Objective:", solver.ObjectiveValue())
        print([solver.Value(i) for i in matrix_usage])

        for v in range(n_vehicles):
            if solver.Value(matrix_usage[v]) == 0:
                continue

            print(f"V{v}")
            for i in range(n_locations):
                print(" ".join(['1' if solver.Value(matrix[v][i][j]) else '.' for j in range(n_locations)]))

if __name__ == "__main__":
    solve(create_data_model())