from pprint import pprint
from ortools.sat.python import cp_model
from typing import Any, List

DATA = [
    [0,   548,  776, 696,  582,  274,  502,  194, 308,  194, 536,  502,  388,  354,  468,  776,  662 ],
    [548, 0,    684, 308,  194,  502,  730,  354, 696,  742, 1084, 594,  480,  674,  1016, 868,  1210],
    [776, 684,  0,   992,  878,  502,  274,  810, 468,  742, 400,  1278, 1164, 1130, 788,  1552, 754 ],
    [696, 308,  992, 0,    114,  650,  878,  502, 844,  890, 1232, 514,  628,  822,  1164, 560,  1358],
    [582, 194,  878, 114,  0,    536,  764,  388, 730,  776, 1118, 400,  514,  708,  1050, 674,  1244],
    [274, 502,  502, 650,  536,  0,    228,  308, 194,  240, 582,  776,  662,  628,  514,  1050, 708 ],
    [502, 730,  274, 878,  764,  228,  0,    536, 194,  468, 354,  1004, 890,  856,  514,  1278, 480 ],
    [194, 354,  810, 502,  388,  308,  536,  0,   342,  388, 730,  468,  354,  320,  662,  742,  856 ],
    [308, 696,  468, 844,  730,  194,  194,  342, 0,    274, 388,  810,  696,  662,  320,  1084, 514 ],
    [194, 742,  742, 890,  776,  240,  468,  388, 274,  0,   342,  536,  422,  388,  274,  810,  468 ],
    [536, 1084, 400, 1232, 1118, 582,  354,  730, 388,  342, 0,    878,  764,  730,  388,  1152, 354 ],
    [502, 594,  1278, 514, 400,  776,  1004, 468, 810,  536, 878,  0,    114,  308,  650,  274,  844 ],
    [388, 480,  1164, 628, 514,  662,  890,  354, 696,  422, 764,  114,  0,    194,  536,  388,  730 ],
    [354, 674,  1130, 822, 708,  628,  856,  320, 662,  388, 730,  308,  194,  0,    342,  422,  536 ],
    [468, 1016, 788, 1164, 1050, 514,  514,  662, 320,  274, 388,  650,  536,  342,  0,    764,  194 ],
    [776, 868,  1552, 560, 674,  1050, 1278, 742, 1084, 810, 1152, 274,  388,  422,  764,  0,    798 ],
    [662, 1210, 754, 1358, 1244, 708,  480,  856, 514,  468, 354,  844,  730,  536,  194,  798,  0   ],
]
DEMANDS = [0, 1, 1, 2, 4, 2, 4, 8, 8, 1, 2, 1, 2, 4, 4, 8, 8]
VEHICLE_CAPACITY = 15

def minimize_travel_costs(model : cp_model.CpModel, matrix : List[List[List[Any]]], data : List[List[int]], n_locations : int):
    objective = 0
    for v in range(len(matrix)):
        for i in range(n_locations):
            for j in range(n_locations):
                objective += data[i][j] * matrix[v][i][j]    
    model.Minimize(objective)

def diagonals_must_be_zero(model : cp_model.CpModel, matrix : List[List[Any]], n_locations : int):
    for v in range(len(matrix)):
        for i in range(n_locations):
            model.Add(matrix[v][i][i] == 0)

def every_delivery_has_an_arrival_and_a_departure(model : cp_model.CpModel, matrix : List[List[Any]], n_locations : int):
    for row in range(1, n_locations):
        model.Add(sum(matrix[v][row][col] for v in range(len(matrix)) for col in range(n_locations)) == 1)
    for col in range(1, n_locations):
        model.Add(sum(matrix[v][row][col] for v in range(len(matrix)) for row in range(n_locations)) == 1)
    
    for v in range(len(matrix)):
        locations_sum = sum(matrix[v][row][col] for row in range(n_locations) for col in range(n_locations))
        model.Add(sum(matrix[v][0][col] for col in range(n_locations)) * n_locations >= locations_sum)
        model.Add(sum(matrix[v][row][0] for row in range(n_locations)) * n_locations >= locations_sum)

def paths_make_sense(model : cp_model.CpModel, matrix : List[List[Any]], n_locations : int):
    for v in range(len(matrix)):
        for f in range(n_locations):
            model.Add(sum(matrix[v][i][f] for i in range(n_locations)) == sum(matrix[v][f][j] for j in range(n_locations)))


def enforce_maximum_capacity(model : cp_model.CpModel, matrix, n_locations, demands, vehicle_capacity):
    for v in range(len(matrix)):
        demand_sum = 0
        for i in range(n_locations):
            for j in range(n_locations):
                demand_sum += matrix[v][i][j] * demands[i]
        model.Add(demand_sum <= vehicle_capacity)

def solve(data, demands, capacity, time_limit):
    model = cp_model.CpModel()

    n_locations = len(data)
    n_vehicles = int((sum(demands) / capacity) * 1.5) # This should be enough vehicles

    matrix = [] # matrix[vehicle][from][to]
    for v in range(n_vehicles):
        square = []

        for i in range(n_locations):
            square.append([model.NewBoolVar(f"{v}-{i}-{j}") for j in range(n_locations)])
        matrix.append(square)

    minimize_travel_costs(model, matrix, data, n_locations)
    diagonals_must_be_zero(model, matrix, n_locations)
    every_delivery_has_an_arrival_and_a_departure(model, matrix, n_locations)
    enforce_maximum_capacity(model, matrix, n_locations, demands, capacity)
    paths_make_sense(model, matrix, n_locations)

    # This is here to remove subcycles (MTZ formulation)
    deliveries = [model.NewIntVar(0, n_locations**4, f"d-{j}") for j in range(n_locations)]
    for v in range(n_vehicles):
        for i in range(1, n_locations):
            for j in range(1, n_locations):
                model.Add(deliveries[j] - deliveries[i] >= 1 + n_locations * (matrix[v][i][j] - 1))

    print("Solving...")
    solver = cp_model.CpSolver()
    solver.parameters.max_time_in_seconds = time_limit
    status = solver.Solve(model)

    if status == cp_model.OPTIMAL or status == cp_model.FEASIBLE:
        mileage = solver.ObjectiveValue()
        result = []

        for v in range(n_vehicles):
            path = [0]
            load = 0
            distance = 0

            point = 0
            while True:
                for x in range(n_locations):
                    if solver.Value(matrix[v][point][x]):
                        path.append(x)
                        distance += data[point][x]
                        load += demands[x]
                        point = x
                        break
                if point == 0:
                    break
            
            result.append({
                "path" : path,
                "load" : load,
                "distance" : distance,
            })
        
        print(f"Mileage: {mileage}")
        pprint(result)
        return result
    else:
        print("No solution found")
        

if __name__ == "__main__":
    solve(DATA, DEMANDS, VEHICLE_CAPACITY, 20)