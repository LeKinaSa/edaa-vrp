import time
import pandas as pd
from typing import Tuple

from or_tools_solution import cvrp

def solve(name:str, distance_matrix_path: str, cvrp_problem_path: str) -> Tuple[str, int, float, int, float, int]:
    start_time = time.time()
    (data, manager, routing, solution) = cvrp(distance_matrix_path, cvrp_problem_path, 3 * 60)
    end_time = time.time()
    total_distance = 0
    total_load = 0
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
            total_load += data['demands'][node_index]
        routes.append(route)
    return (name, len(data['demands']), total_distance, len(routes), total_load / len(routes), (end_time - start_time) * 10**6)

if __name__=="__main__":
    files = ['0-pa-61', '0-pa-25', '0-pa-34', '0-df-12', '0-df-15', '0-df-44', '2-rj-17']
    df = pd.DataFrame(columns=['name','num_deliveries','solution_length','num_vehicles','average_load','time_us'])
    for file in files:
        file_details = file.split('-')
        distance_matrix_path = f'data/distances/{file}.txt'
        cvrp_problem_path = f'data/train/{file_details[1]}-{file_details[0]}/cvrp-{file}.json'
        result = solve(file, distance_matrix_path, cvrp_problem_path)
        df.append(result)
    df.to_csv('data/cvrp-benchmarks/cp_analysis.csv')
    
"""
./cvrp --osm ../data/graphs/pa.xml --cvrp ../data/train/pa-0/cvrp-0-pa-61.json --dm ../data/distances/0-pa-61.txt
./cvrp --osm ../data/graphs/pa.xml --cvrp ../data/train/pa-0/cvrp-0-pa-25.json --dm ../data/distances/0-pa-25.txt
./cvrp --osm ../data/graphs/pa.xml --cvrp ../data/train/pa-0/cvrp-0-pa-34.json --dm ../data/distances/0-pa-34.txt
./cvrp --osm ../data/graphs/df.xml --cvrp ../data/train/df-0/cvrp-0-df-12.json --dm ../data/distances/0-df-12.txt
./cvrp --osm ../data/graphs/df.xml --cvrp ../data/train/df-0/cvrp-0-df-15.json --dm ../data/distances/0-df-15.txt
./cvrp --osm ../data/graphs/df.xml --cvrp ../data/train/df-0/cvrp-0-df-44.json --dm ../data/distances/0-df-44.txt
./cvrp --osm ../data/graphs/rj.xml --cvrp ../data/train/rj-2/cvrp-2-rj-17.json --dm ../data/distances/2-rj-17.txt
"""
