#include "SearchAlgorithm.h"
#include <algorithm>

std::vector<State> SearchAlgorithm::reconstructPath(
    const std::unordered_map<State, State, StateHash>& parent,
    const State& goal) {

    std::vector<State> path;
    State current = goal;

    while (parent.find(current) != parent.end() && !(parent.at(current) == current)) {
        path.push_back(current);
        current = parent.at(current);
    }
    path.push_back(current);
    std::reverse(path.begin(), path.end());

    return path;
}

void SearchAlgorithm::updateStatistics(Result& result, size_t openListSize, size_t closedSetSize) {
    result.iterations++;
    result.maxOpenListSize = std::max(result.maxOpenListSize, static_cast<int>(openListSize));
    result.maxMemoryUsage = std::max(result.maxMemoryUsage,
        static_cast<int>(openListSize + closedSetSize));
}