
#include <cmath>
#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <time.h>

using namespace std;

const int NUMBER_OF_ITERATIONS = 10000;
const double INITIAL_TEMPERATURE = 1000;
const double TEMPERATURE_DECREASE_PER_ITERATION = 0.1;
const double PROBABILITY_TO_CHANGE_SOLUTIONS = 0.5;

vector<int> getInitialSolution(int locations) {
    vector<int> initialSolution = vector<int>();

    for (int i = 1; i <= locations; ++i) {
        initialSolution.push_back(i);
        initialSolution.push_back(0);
    }

    initialSolution.pop_back();
    return initialSolution;
}

bool isValid(vector<int> solution, vector<int> demands, int vehicleCapacity) {
    int capacity = 0;

    for (int location : solution) {
        if (location == 0) {
            capacity = 0;
            continue;
        }
        capacity += demands[location];
        if (capacity > vehicleCapacity) {
            return false;
        }
    }

    return true;
}

vector<int> randomNeighbor(vector<int> oldSolution) {
    vector<int> newSolution = vector<int>(oldSolution);
    int randomValue = rand() % 4;
    int one, two;
    switch (randomValue) {
        case 0: { // Swap
            one = rand() % newSolution.size();
            two = rand() % newSolution.size();
            swap(newSolution[one], newSolution[two]);
            break;
        }
        case 1: { // Scramble
            one = rand() %  newSolution.size();
            two = rand() % (newSolution.size() - one) + one;
            random_shuffle(newSolution.begin() + one, newSolution.begin() + two);
            break;
        }
        case 2: { // Insert
            one = rand() %  newSolution.size();
            two = rand() % (newSolution.size() - 1);
            int value = newSolution[one];
            newSolution.erase (newSolution.begin() + one);
            newSolution.insert(newSolution.begin() + two, value);
            break;
        }
        case 3: { // Revert
            one = rand() %  newSolution.size();
            two = rand() % (newSolution.size() - one) + one;
            reverse(newSolution.begin() + one, newSolution.begin() + two + 1);
            break;
        }
        default: {
            break;
        }
    }
    return newSolution;
}

vector<int> randomValidNeighbor(vector<int> oldSolution, vector<int> demands, int vehicleCapacity) {
    vector<int> newSolution;
    bool newSolutionIsValid = false;
    while (!newSolutionIsValid) {
        newSolution = randomNeighbor(oldSolution);
        newSolutionIsValid = isValid(newSolution, demands, vehicleCapacity);
    }
    return newSolution;
}

double calculateDistance(vector<int> solution, vector<vector<double>> distanceMatrix) {
    int distance = distanceMatrix[0][solution[0]];

    for (int i = 0, j = 1; j < solution.size(); ++i, ++j) {
        distance += distanceMatrix[solution[i]][solution[j]];
    }

    distance += distanceMatrix[solution[solution.size()-1]][0];
    return distance;
}

vector<int> simulatedAnnealing(vector<vector<double>> distanceMatrix, vector<int> demands, int vehicleCapacity) {
    srand(time(NULL));

    vector<int> oldSolution = getInitialSolution(distanceMatrix.size() - 1);
    double oldDistance = calculateDistance(oldSolution, distanceMatrix);
    
    vector<int> bestSolution = oldSolution;
    double bestDistance = oldDistance;

    for(double temperature = INITIAL_TEMPERATURE; temperature > 0; temperature -= TEMPERATURE_DECREASE_PER_ITERATION) {
        vector<int> newSolution = randomValidNeighbor(oldSolution, demands, vehicleCapacity);
        double newDistance = calculateDistance(newSolution, distanceMatrix);
        
        if (newDistance < bestDistance) {
            bestSolution = newSolution;
            bestDistance = newDistance;
        }

        double deltaDistance = newDistance - oldDistance;
        double randomProbability = 0;
        if (deltaDistance <= 0 || exp(-deltaDistance / temperature) > randomProbability) {
            oldSolution = newSolution; // move
            oldDistance = newDistance;
        }

        return bestSolution;
    }
}
