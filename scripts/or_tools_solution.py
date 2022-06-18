# Code adapted from https://developers.google.com/optimization/routing/cvrp

from ortools.constraint_solver import routing_enums_pb2
from ortools.constraint_solver import pywrapcp

import argparse
import json

def create_data_model(args):
    """Stores the data for the problem."""
    data = {}
    
    lines = []
    with open(args.dist_filename) as file:
        lines = file.readlines()
    data['distance_matrix'] = [[int(round(float(element) * 100, 0)) for element in line.split(' ')[:-1]] for line in lines]

    with open(args.cvrp_filename) as file:
        cvrp_info = json.load(file)
    data['demands'] = [0] + [delivery['size'] for delivery in cvrp_info['deliveries']]
    data['vehicle_capacity'] = cvrp_info['vehicle_capacity']

    return data


def print_solution(data, manager, routing, solution):
    """Prints solution on console."""
    print(f'Objective: {solution.ObjectiveValue()}')
    total_distance = 0
    total_load = 0
    for vehicle_id in range(len(data['distance_matrix'])):
        index = routing.Start(vehicle_id)
        plan_output = 'Route for vehicle {}:\n'.format(vehicle_id)
        route_distance = 0
        route_load = 0
        while not routing.IsEnd(index):
            node_index = manager.IndexToNode(index)
            route_load += data['demands'][node_index]
            plan_output += ' {0} Load({1}) ->'.format(node_index, route_load)
            previous_index = index
            index = solution.Value(routing.NextVar(index))
            route_distance += routing.GetArcCostForVehicle(
                previous_index, index, vehicle_id) / 100
        plan_output += ' {0} Load({1})\n'.format(manager.IndexToNode(index), route_load)
        plan_output += 'Distance of the route: {}m\n'.format(route_distance)
        plan_output += 'Load of the route: {}\n'.format(route_load)

        if route_load == 0:
            continue

        print(plan_output)
        total_distance += route_distance
        total_load += route_load
    print('Total distance of all routes: {:.2f}m'.format(total_distance))
    print('Total load of all routes: {}'.format(total_load))


def main():
    """Solve the CVRP problem."""
    parser = argparse.ArgumentParser(description='Process some integers.')
    parser.add_argument('cvrp_filename', type=str, help='path to the CVRP file')
    parser.add_argument('dist_filename', type=str, help='path for the file with the distance matrix')

    args = parser.parse_args()

    # Instantiate the data problem.
    data = create_data_model(args)

    # Create the routing index manager.
    manager = pywrapcp.RoutingIndexManager(len(data['distance_matrix']), len(data['distance_matrix']), 0)

    # Create Routing Model.
    routing = pywrapcp.RoutingModel(manager)

    # Create and register a transit callback.
    def distance_callback(from_index, to_index):
        """Returns the distance between the two nodes."""
        # Convert from routing variable Index to distance matrix NodeIndex.
        from_node = manager.IndexToNode(from_index)
        to_node = manager.IndexToNode(to_index)
        return data['distance_matrix'][from_node][to_node]

    transit_callback_index = routing.RegisterTransitCallback(distance_callback)

    # Define cost of each arc.
    routing.SetArcCostEvaluatorOfAllVehicles(transit_callback_index)


    # Add Capacity constraint.
    def demand_callback(from_index):
        """Returns the demand of the node."""
        # Convert from routing variable Index to demands NodeIndex.
        from_node = manager.IndexToNode(from_index)
        return data['demands'][from_node]

    demand_callback_index = routing.RegisterUnaryTransitCallback(demand_callback)
    routing.AddDimension(
        demand_callback_index, 
        0,  # null capacity slack
        data['vehicle_capacity'], # vehicle maximum capacities
        True, # start cumul to zero
        "Capacity"
    )

    # Setting first solution heuristic.
    search_parameters = pywrapcp.DefaultRoutingSearchParameters()
    search_parameters.first_solution_strategy = (routing_enums_pb2.FirstSolutionStrategy.PATH_CHEAPEST_ARC)
    search_parameters.local_search_metaheuristic = (routing_enums_pb2.LocalSearchMetaheuristic.GUIDED_LOCAL_SEARCH)
    search_parameters.time_limit.FromSeconds(1)

    # Solve the problem.
    solution = routing.SolveWithParameters(search_parameters)

    # Print solution on console.
    if solution:
        print_solution(data, manager, routing, solution)


if __name__ == '__main__':
    main()