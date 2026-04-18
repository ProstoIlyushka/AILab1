#include "AStar.h"
#include <cmath>
#include <limits>
#include <algorithm>
#include <iostream>

AStar::AStar() : currentHeuristic(H1) {}

void AStar::setHeuristicH1() {
    currentHeuristic = H1;
    std::cout << "A* using heuristic H1 (min distance to any target)" << std::endl;
}

void AStar::setHeuristicH2() {
    currentHeuristic = H2;
    std::cout << "A* using heuristic H2 (greedy assignment)" << std::endl;
}

// Эвристика H1: сумма минимальных манхэттенских расстояний от каждого ящика до ближайшей цели
int AStar::heuristic1(const State& state, const std::vector<sf::Vector2i>& targets) const {
    if (targets.empty() || state.getBoxes().empty()) {
        return 0;
    }

    int total = 0;
    for (const auto& box : state.getBoxes()) {
        int bestDist = std::numeric_limits<int>::max();
        for (const auto& target : targets) {
            int dist = std::abs(box.x - target.x) + std::abs(box.y - target.y);
            if (dist < bestDist) {
                bestDist = dist;
            }
        }
        total += bestDist;
    }
    return total;
}

// Эвристика H2: жадное назначение ящиков на цели (минимизация суммы расстояний)
int AStar::heuristic2(const State& state, const std::vector<sf::Vector2i>& targets) const {
    if (targets.empty() || state.getBoxes().empty()) {
        return 0;
    }

    std::vector<sf::Vector2i> boxes = state.getBoxes();
    std::vector<bool> usedTarget(targets.size(), false);
    int total = 0;

    for (const auto& box : boxes) {
        int bestDist = std::numeric_limits<int>::max();
        int bestIndex = -1;

        for (size_t i = 0; i < targets.size(); i++) {
            if (usedTarget[i]) continue;
            int dist = std::abs(box.x - targets[i].x) + std::abs(box.y - targets[i].y);
            if (dist < bestDist) {
                bestDist = dist;
                bestIndex = static_cast<int>(i);
            }
        }

        if (bestIndex >= 0) {
            total += bestDist;
            usedTarget[bestIndex] = true;
        }
    }

    return total;
}

// Выбор текущей эвристики
int AStar::heuristic(const State& state, const std::vector<sf::Vector2i>& targets) const {
    if (currentHeuristic == H1) {
        return heuristic1(state, targets);
    }
    else {
        return heuristic2(state, targets);
    }
}

// Восстановление пути
std::vector<State> AStar::reconstructPath(
    const std::unordered_map<State, State, StateHash>& cameFrom,
    const State& current) {

    std::vector<State> path;
    State node = current;

    while (cameFrom.find(node) != cameFrom.end() && !(cameFrom.at(node) == node)) {
        path.push_back(node);
        node = cameFrom.at(node);
    }
    path.push_back(node);
    std::reverse(path.begin(), path.end());

    return path;
}

// Основной метод поиска A*
SearchAlgorithm::Result AStar::search(const State& start,
    const std::vector<sf::Vector2i>& walls,
    const std::vector<sf::Vector2i>& targets) {

    Result result;

    // Очередь с приоритетом (min-heap)
    std::priority_queue<AStarNode, std::vector<AStarNode>, std::greater<AStarNode>> openSet;

    // gScore: стоимость пути от start до состояния
    std::unordered_map<State, int, StateHash> gScore;

    // cameFrom: для восстановления пути
    std::unordered_map<State, State, StateHash> cameFrom;

    // closedSet: уже обработанные состояния
    std::unordered_set<State, StateHash> closedSet;

    // Инициализация
    int startHeuristic = heuristic(start, targets);
    openSet.push({ start, 0, startHeuristic });
    gScore[start] = 0;
    cameFrom[start] = start;

    std::cout << "A* search started with heuristic: "
        << (currentHeuristic == H1 ? "H1" : "H2") << std::endl;

    while (!openSet.empty()) {
        // Обновляем статистику
        updateStatistics(result, openSet.size(), closedSet.size());

        // Берём узел с наименьшим f
        AStarNode current = openSet.top();
        openSet.pop();

        // Проверка на целевое состояние
        if (current.state.isGoal(targets)) {
            result.success = true;
            result.path = reconstructPath(cameFrom, current.state);
            result.finalOpenListSize = static_cast<int>(openSet.size());

            std::cout << "A* solution found!" << std::endl;
            std::cout << "Path length: " << result.path.size() - 1 << std::endl;
            std::cout << "Iterations: " << result.iterations << std::endl;

            return result;
        }

        // Пропускаем уже обработанные
        if (closedSet.find(current.state) != closedSet.end()) {
            continue;
        }

        // Добавляем в обработанные
        closedSet.insert(current.state);

        // Раскрываем состояние
        for (const auto& next : current.state.getSuccessors(walls, targets)) {
            // Стоимость пути до next
            int tentativeG = gScore[current.state] + 1;

            // Если нашли лучший путь к next
            if (gScore.find(next) == gScore.end() || tentativeG < gScore[next]) {
                cameFrom[next] = current.state;
                gScore[next] = tentativeG;
                int h = heuristic(next, targets);
                openSet.push({ next, tentativeG, h });
            }
        }
    }

    // Решение не найдено
    result.success = false;
    std::cout << "A*: No solution found!" << std::endl;

    return result;
}