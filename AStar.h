#pragma once
#include "SearchAlgorithm.h"
#include <queue>
#include <unordered_map>
#include <unordered_set>

struct AStarNode {
    State state;
    int g;  // стоимость пути от начала
    int h;  // эвристика

    int f() const { return g + h; }

    bool operator>(const AStarNode& other) const {
        return f() > other.f();
    }

    bool operator==(const AStarNode& other) const {
        return state == other.state;
    }
};

class AStar : public SearchAlgorithm {
private:
    // Тип эвристики
    enum HeuristicType {
        H1,  // базовая: сумма минимальных расстояний
        H2   // улучшенная: жадное назначение
    };

    HeuristicType currentHeuristic = H1;

    // Эвристические функции
    int heuristic1(const State& state, const std::vector<sf::Vector2i>& targets) const;
    int heuristic2(const State& state, const std::vector<sf::Vector2i>& targets) const;

    // Выбор текущей эвристики
    int heuristic(const State& state, const std::vector<sf::Vector2i>& targets) const;

    // Вспомогательный метод для восстановления пути
    std::vector<State> reconstructPath(
        const std::unordered_map<State, State, StateHash>& cameFrom,
        const State& current);

public:
    AStar();

    // Установка типа эвристики
    void setHeuristicH1();
    void setHeuristicH2();

    // Основной метод поиска
    Result search(const State& start,
        const std::vector<sf::Vector2i>& walls,
        const std::vector<sf::Vector2i>& targets) override;
};