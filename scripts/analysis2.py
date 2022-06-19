import time
from typing import Tuple, List

from or_tools_solution import cvrp

class Solver:
    def solve(distance_matrix_path: str, cvrp_problem_path: str, **args) -> Tuple[float, List[List[int]], float]:
        """Returns the time it took to solve (in seconds), list of paths, total travelled distance"""
        raise Exception("Not implemented")


class OrToolsSolver(Solver):
    def solve(distance_matrix_path: str, cvrp_problem_path: str) -> Tuple[float, List[List[int]], float]:
        start_time = time.time()
        (data, manager, routing, solution) = cvrp(distance_matrix_path, cvrp_problem_path, 1)
        end_time = time.time()
        total_distance = 0
        routes = []
        for vehicle_id in range(len(data['distance_matrix'])):
            route = []
            index = routing.Start(vehicle_id)
            while not routing.IsEnd(index):
                node_index = manager.IndexToNode(index)
                route.append(node_index)
                previous_index = index
                index = solution.Value(routing.NextVar(index))
                total_distance += routing.GetArcCostForVehicle(
                    previous_index, index, vehicle_id) / 100
            routes.append(route)
        return (end_time - start_time, routes, total_distance)

class SimulatedAnnealing(Solver):
    def solve(distance_matrix_path: str, cvrp_problem_path: str) -> Tuple[float, List[List[int]], float]:
        pass

class Greedy(Solver):
    def solve(distance_matrix_path: str, cvrp_problem_path: str) -> Tuple[float, List[List[int]], float]:
        pass

class TabuSearch(Solver):
    def solve(distance_matrix_path: str, cvrp_problem_path: str) -> Tuple[float, List[List[int]], float]:
        pass

class AntColony(Solver):
    def solve(distance_matrix_path: str, cvrp_problem_path: str) -> Tuple[float, List[List[int]], float]:
        pass

if __name__=="__main__":
    pass