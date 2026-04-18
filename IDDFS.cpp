#include "IDDFS.h"
#include <unordered_set>
#include <unordered_map>

bool IDDFS::depthLimitedSearch(const State& current,
    const std::vector<sf::Vector2i>& walls,
    const std::vector<sf::Vector2i>& targets,
    int depth,
    int limit,
    std::unordered_map<State, State, StateHash>& parent,
    std::unordered_set<State, StateHash>& visited,
    Result& result,
    int& currentMaxFrontier) {

    // Проверка прерывания
    if (shouldStop(result.iterations)) {
        return false;
    }

    result.iterations++;
    currentMaxFrontier = std::max(currentMaxFrontier, static_cast<int>(visited.size()));
    result.maxMemoryUsage = std::max(result.maxMemoryUsage,
        static_cast<int>(visited.size() + parent.size()));

    if (current.isGoal(targets)) {
        result.success = true;
        result.path = reconstructPath(parent, current);
        result.solutionLength = static_cast<int>(result.path.size() - 1);
        return true;
    }

    if (depth >= limit) {
        return false;
    }

    auto successors = current.getSuccessors(walls, targets);

    for (const auto& next : successors) {
        if (visited.find(next) != visited.end()) {
            continue;
        }

        parent[next] = current;
        visited.insert(next);

        if (depthLimitedSearch(next, walls, targets, depth + 1, limit, parent, visited, result, currentMaxFrontier)) {
            return true;
        }

        visited.erase(next);
    }

    return false;
}

SearchAlgorithm::Result IDDFS::search(const State& start,
    const std::vector<sf::Vector2i>& walls,
    const std::vector<sf::Vector2i>& targets) {

    Result result;
    startTimer();

    for (int limit = 1; limit <= 100; limit++) {
        // Проверка прерывания перед каждой итерацией
        if (shouldStop(result.iterations)) {
            result.timeout = true;
            return result;
        }

        std::unordered_map<State, State, StateHash> parent;
        std::unordered_set<State, StateHash> visited;
        int currentMaxFrontier = 0;

        parent[start] = start;
        visited.insert(start);

        std::cout << "IDDFS: Trying depth limit " << limit << std::endl;

        if (depthLimitedSearch(start, walls, targets, 0, limit, parent, visited, result, currentMaxFrontier)) {
            result.maxOpenListSize = currentMaxFrontier;
            result.finalOpenListSize = static_cast<int>(visited.size());
            return result;
        }
    }

    result.success = false;
    return result;
}