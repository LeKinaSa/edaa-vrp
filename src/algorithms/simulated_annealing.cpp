
#include <cmath>
#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <time.h>
#include "simulated_annealing.hpp"
#include "../cvrp/cvrp.hpp"

using namespace std;

const double INITIAL_TEMPERATURE = 5000;
const double TEMPERATURE_DECREASE_PER_ITERATION = 0.1;

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

vector<int> randomValidNeighbor(vector<int> oldSolution, CvrpInstance instance) {
    vector<int> newSolution;
    bool newSolutionIsValid = false;
    while (!newSolutionIsValid) {
        newSolution = randomNeighbor(oldSolution);
        newSolutionIsValid = instance.isValid(newSolution);
    }
    return newSolution;
}

vector<int> simulatedAnnealingAlgorithm(CvrpInstance instance) {
    srand(time(NULL));

    vector<int> oldSolution = instance.getInitialSolution();
    double oldDistance = instance.calculateDistance(oldSolution);
    
    vector<int> bestSolution = oldSolution;
    double bestDistance = oldDistance;

    for(double temperature = INITIAL_TEMPERATURE; temperature > 0; temperature -= TEMPERATURE_DECREASE_PER_ITERATION) {
        vector<int> newSolution = randomValidNeighbor(oldSolution, instance);
        double newDistance = instance.calculateDistance(newSolution);
        
        if (newDistance < bestDistance) {
            bestSolution = newSolution;
            bestDistance = newDistance;
        }

        double deltaDistance = newDistance - oldDistance;
        double randomProbability = (float) rand() / RAND_MAX;
        if (deltaDistance <= 0 || exp(-deltaDistance / temperature) > randomProbability) {
            oldSolution = move(newSolution);
            oldDistance = newDistance;
        }
    }

    return bestSolution;
}

vector<int> simulatedAnnealing(CvrpInstance instance) {
    vector<int> solution = simulatedAnnealingAlgorithm(instance);
    vector<int> cleanSolution = vector<int>();
    for (int i = -1, j = 0; j < solution.size(); ++i, ++j) {
        int valueI = i >= 0 ? solution[i] : 0;
        int valueJ = solution[j];
        if (valueI + valueJ > 0) {
            cleanSolution.push_back(solution[j]);
        }
    }
    if (cleanSolution.back() == 0) {
        cleanSolution.pop_back();
    }
    return cleanSolution;
}
