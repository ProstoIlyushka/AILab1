#pragma once
#include "SearchAlgorithm.h"

class BFS : public SearchAlgorithm {
public:
    SearchAlgorithm::Result search(const State& start,
        const std::vector<sf::Vector2i>& walls,
        const std::vector<sf::Vector2i>& targets) override;
};