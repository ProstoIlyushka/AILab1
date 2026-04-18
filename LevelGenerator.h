#pragma once
#include "LevelParser.h"
#include <random>
#include <vector>

class LevelGenerator {
private:
    std::mt19937 rng;
    int width;
    int height;

    // √енераци€ пустого уровн€ с рамкой из стен
    LevelData generateEmptyLevel();

    // ƒобавление случайных внутренних стен
    void addRandomWalls(LevelData& level, int wallCount);

    // ƒобавление игрока, €щиков и целей
    void addPlayerBoxesAndTargets(LevelData& level, int numBoxes);

    // ѕроверка, что все клетки достижимы
    bool isReachable(const LevelData& level);

    // ѕроверка, что €щики не застр€ли в углах (не на цели)
    bool isAnyBoxInCorner(const LevelData& level);

    // ѕроверка разрешимости через BFS с возвратом длины решени€
    int getSolutionLength(const LevelData& level);

public:
    LevelGenerator();

    // √енераци€ уровн€ с заданным количеством стен и €щиков
    LevelData generateLevel(int wallCount, int numBoxes, int minSolutionLength = 0);

    // √енераци€ случайного уровн€ с параметрами
    LevelData generateRandomLevel(int minSolutionLength = 0);

    // √енераци€ заведомо нерешаемого уровн€
    LevelData generateUnsolvableLevel();
};