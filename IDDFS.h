#pragma once
#include "SearchAlgorithm.h"
#include <unordered_set>
#include <unordered_map>
#include <chrono>
#include <iostream>

class IDDFS : public SearchAlgorithm {
private:
    bool depthLimitedSearch(const State& current,
        const std::vector<sf::Vector2i>& walls,
        const std::vector<sf::Vector2i>& targets,
        int depth,
        int limit,
        std::unordered_map<State, State, StateHash>& parent,
        std::unordered_set<State, StateHash>& visited,
        Result& result,
        int& currentMaxFrontier);

public:
    Result search(const State& start,
        const std::vector<sf::Vector2i>& walls,
        const std::vector<sf::Vector2i>& targets) override;
};