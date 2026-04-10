#pragma once
#include "State.h"
#include <vector>
#include <unordered_map>

class SearchAlgorithm {
public:
    struct Result {
        std::vector<State> path;
        int iterations = 0;
        int maxOpenListSize = 0;
        int finalOpenListSize = 0;
        int maxMemoryUsage = 0;
        bool success = false;
    };

    virtual ~SearchAlgorithm() = default;
    virtual Result search(const State& start,
        const std::vector<sf::Vector2i>& walls,
        const std::vector<sf::Vector2i>& targets) = 0;

protected:
    std::vector<State> reconstructPath(
        const std::unordered_map<State, State, StateHash>& parent,
        const State& goal);

    void updateStatistics(Result& result, size_t openListSize, size_t closedSetSize);
};