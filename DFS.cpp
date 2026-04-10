#include "DFS.h"
#include <stack>
#include <unordered_set>
#include <unordered_map>

SearchAlgorithm::Result DFS::search(const State& start,
    const std::vector<sf::Vector2i>& walls,
    const std::vector<sf::Vector2i>& targets) {

    SearchAlgorithm::Result result;
    std::stack<State> frontier;
    std::unordered_set<State, StateHash> closedSet;
    std::unordered_map<State, State, StateHash> parent;

    frontier.push(start);
    parent[start] = start;

    while (!frontier.empty()) {
        updateStatistics(result, frontier.size(), closedSet.size());

        State current = frontier.top();
        frontier.pop();

        if (current.isGoal(targets)) {
            result.success = true;
            result.path = reconstructPath(parent, current);
            result.finalOpenListSize = static_cast<int>(frontier.size());
            return result;
        }

        if (closedSet.find(current) != closedSet.end()) {
            continue;
        }

        closedSet.insert(current);

        for (const auto& next : current.getSuccessors(walls, targets)) {
            if (closedSet.find(next) == closedSet.end()) {
                frontier.push(next);
                parent[next] = current;
            }
        }
    }

    result.success = false;
    result.finalOpenListSize = static_cast<int>(frontier.size());
    return result;
}