#include "LevelGenerator.h"
#include "AStar.h"
#include "State.h"
#include <queue>
#include <set>
#include <iostream>
#include <algorithm>

LevelGenerator::LevelGenerator() : rng(std::random_device{}()) {
    width = 9;
    height = 9;
}

LevelData LevelGenerator::generateEmptyLevel() {
    LevelData level;
    level.width = width;
    level.height = height;

    // Добавляем стены по краям
    for (int x = 0; x < width; x++) {
        level.walls.push_back({ x, 0 });
        level.walls.push_back({ x, height - 1 });
    }
    for (int y = 1; y < height - 1; y++) {
        level.walls.push_back({ 0, y });
        level.walls.push_back({ width - 1, y });
    }

    return level;
}

void LevelGenerator::addRandomWalls(LevelData& level, int wallCount) {
    std::uniform_int_distribution<int> distX(1, width - 2);
    std::uniform_int_distribution<int> distY(1, height - 2);

    int added = 0;
    while (added < wallCount) {
        sf::Vector2i pos(distX(rng), distY(rng));

        auto it = std::find(level.walls.begin(), level.walls.end(), pos);
        if (it != level.walls.end()) continue;

        level.walls.push_back(pos);
        added++;
    }
}

void LevelGenerator::addPlayerBoxesAndTargets(LevelData& level, int numBoxes) {
    std::uniform_int_distribution<int> distX(1, width - 2);
    std::uniform_int_distribution<int> distY(1, height - 2);

    auto isWall = [&level](const sf::Vector2i& pos) {
        return std::find(level.walls.begin(), level.walls.end(), pos) != level.walls.end();
        };

    // Исправленная лямбда - захватываем все необходимые переменные по ссылке
    auto isOccupied = [&](const sf::Vector2i& pos) {
        if (isWall(pos)) return true;
        if (pos == level.playerStart) return true;
        for (const auto& box : level.boxes) {
            if (box == pos) return true;
        }
        for (const auto& target : level.targets) {
            if (target == pos) return true;
        }
        return false;
        };

    // Генерируем позицию игрока (не на стене)
    do {
        level.playerStart = { distX(rng), distY(rng) };
    } while (isWall(level.playerStart));

    // Генерируем ящики и цели
    for (int i = 0; i < numBoxes; i++) {
        // Генерируем ящик
        sf::Vector2i boxPos;
        do {
            boxPos = { distX(rng), distY(rng) };
        } while (isOccupied(boxPos));
        level.boxes.push_back(boxPos);

        // Генерируем цель
        sf::Vector2i targetPos;
        do {
            targetPos = { distX(rng), distY(rng) };
        } while (isOccupied(targetPos));
        level.targets.push_back(targetPos);
    }
}

bool LevelGenerator::isReachable(const LevelData& level) {
    auto isWall = [&level](const sf::Vector2i& pos) {
        return std::find(level.walls.begin(), level.walls.end(), pos) != level.walls.end();
        };

    auto bfs = [&](const sf::Vector2i& start, const sf::Vector2i& target, bool ignoreBoxes = false) -> bool {
        std::queue<sf::Vector2i> queue;
        std::set<std::pair<int, int>> visited;

        queue.push(start);
        visited.insert({ start.x, start.y });

        std::vector<sf::Vector2i> dirs = { {0,1}, {0,-1}, {1,0}, {-1,0} };

        while (!queue.empty()) {
            sf::Vector2i current = queue.front();
            queue.pop();

            if (current == target) return true;

            for (const auto& dir : dirs) {
                sf::Vector2i next = current + dir;

                if (next.x < 0 || next.x >= width || next.y < 0 || next.y >= height) continue;
                if (isWall(next)) continue;
                if (visited.count({ next.x, next.y })) continue;

                // Проверка на ящики
                bool isBox = false;
                if (!ignoreBoxes) {
                    isBox = std::find(level.boxes.begin(), level.boxes.end(), next) != level.boxes.end();
                }
                if (isBox && next != target) continue;

                visited.insert({ next.x, next.y });
                queue.push(next);
            }
        }
        return false;
        };

    // Проверяем, что игрок может дойти до всех ящиков
    for (const auto& box : level.boxes) {
        if (!bfs(level.playerStart, box, false)) {
            std::cout << "  Player cannot reach a box!" << std::endl;
            return false;
        }
    }

    // Проверяем, что игрок может дойти до всех целей
    for (const auto& target : level.targets) {
        if (!bfs(level.playerStart, target, true)) {
            std::cout << "  Player cannot reach a target!" << std::endl;
            return false;
        }
    }

    return true;
}

bool LevelGenerator::isAnyBoxInCorner(const LevelData& level) {
    std::vector<sf::Vector2i> corners = {
        {1, 1}, {1, height - 2}, {width - 2, 1}, {width - 2, height - 2}
    };

    for (const auto& box : level.boxes) {
        for (const auto& corner : corners) {
            if (box == corner) {
                // Проверяем, что это не цель
                if (std::find(level.targets.begin(), level.targets.end(), box) == level.targets.end()) {
                    return true;
                }
            }
        }
    }

    return false;
}

int LevelGenerator::getSolutionLength(const LevelData& level) {
    State startState(level.playerStart, level.boxes);
    AStar astar;
    auto result = astar.search(startState, level.walls, level.targets);

    if (result.success) {
        return static_cast<int>(result.path.size() - 1);
    }
    return -1;
}

LevelData LevelGenerator::generateLevel(int wallCount, int numBoxes, int minSolutionLength) {
    const int MAX_ATTEMPTS = 100;

    for (int attempt = 0; attempt < MAX_ATTEMPTS; attempt++) {
        std::cout << "Generating level attempt " << attempt + 1
            << " (walls: " << wallCount << ", boxes: " << numBoxes
            << ", min length: " << minSolutionLength << ")..." << std::endl;

        LevelData level = generateEmptyLevel();
        addRandomWalls(level, wallCount);
        addPlayerBoxesAndTargets(level, numBoxes);

        // Проверка: ящики не должны быть в углах (если это не цели)
        if (isAnyBoxInCorner(level)) {
            std::cout << "  Box in corner, retrying..." << std::endl;
            continue;
        }

        // Проверка достижимости
        if (!isReachable(level)) {
            std::cout << "  Not reachable, retrying..." << std::endl;
            continue;
        }

        // Проверка разрешимости и длины решения
        int solutionLength = getSolutionLength(level);
        if (solutionLength >= minSolutionLength) {
            std::cout << "  Valid level generated! Solution length: " << solutionLength << std::endl;
            return level;
        }
        else {
            std::cout << "  Solution too short (" << solutionLength << " < " << minSolutionLength << "), retrying..." << std::endl;
        }
    }

    // Возвращаем простой гарантированно работающий уровень
    std::cout << "Failed to generate random level, using default..." << std::endl;
    LevelParser parser;
    std::vector<std::string> defaultLevel = {
        "#########",
        "#       #",
        "#  @$   #",
        "#  .    #",
        "#       #",
        "#       #",
        "#       #",
        "#       #",
        "#########"
    };
    return parser.parse(defaultLevel);
}

LevelData LevelGenerator::generateRandomLevel(int minSolutionLength) {
    std::uniform_int_distribution<int> wallDist(7, 15);
    std::uniform_int_distribution<int> boxDist(1, 2);  // 1 или 2 ящика

    int wallCount = wallDist(rng);
    int numBoxes = boxDist(rng);

    return generateLevel(wallCount, numBoxes, minSolutionLength);
}

LevelData LevelGenerator::generateUnsolvableLevel() {
    // Создаём уровень, где ящик заблокирован навсегда
    LevelData level;
    level.width = 7;
    level.height = 7;

    // Стены по краям
    for (int x = 0; x < level.width; x++) {
        level.walls.push_back({ x, 0 });
        level.walls.push_back({ x, level.height - 1 });
    }
    for (int y = 1; y < level.height - 1; y++) {
        level.walls.push_back({ 0, y });
        level.walls.push_back({ level.width - 1, y });
    }

    // Добавляем стену, которая блокирует ящик
    // Ящик будет в углу, а цель — с другой стороны стены
    level.walls.push_back({ 3, 2 });
    level.walls.push_back({ 3, 3 });

    // Игрок
    level.playerStart = { 2, 2 };

    // Ящик (в углу, заблокирован)
    level.boxes.push_back({ 4, 4 });

    // Цель (недостижима из-за стены)
    level.targets.push_back({ 5, 5 });

    return level;
}