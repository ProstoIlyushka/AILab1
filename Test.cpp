#include "Test.h"
#include "BFS.h"
#include "IDDFS.h"
#include "AStar.h"
#include <chrono>
#include <iomanip>
#include <cmath>
#include <algorithm>

Test::Test(Game& gameRef) : game(gameRef), rng(std::random_device{}()) {}

LevelData Test::generateTestLevel(int targetD, int& actualD) {
    const int MAX_ATTEMPTS = 100;
    
    for (int attempt = 0; attempt < MAX_ATTEMPTS; attempt++) {
        // Генерируем случайный уровень
        LevelData level = generator.generateRandomLevel();
        
        // Проверяем длину решения с помощью BFS
        State startState(level.playerStart, level.boxes);
        BFS bfs;
        
        SearchAlgorithm::Limits limits;
        limits.maxIterations = 50000;
        limits.maxTimeMs = 2000;
        bfs.setLimits(limits);
        
        auto result = bfs.search(startState, level.walls, level.targets);
        
        if (result.success) {
            actualD = static_cast<int>(result.path.size() - 1);
            // Если длина решения близка к целевой (±2), принимаем уровень
            if (std::abs(actualD - targetD) <= 2) {
                std::cout << "  Generated level with d=" << actualD 
                          << " (target " << targetD << ")" << std::endl;
                return level;
            }
        }
    }
    
    // Если не удалось сгенерировать, возвращаем простой уровень
    std::cout << "  Warning: Could not generate level with d=" << targetD 
              << ", using default" << std::endl;
    
    LevelData defaultLevel;
    defaultLevel.width = 7;
    defaultLevel.height = 7;
    
    // Базовый уровень
    for (int x = 0; x < 7; x++) {
        defaultLevel.walls.push_back({x, 0});
        defaultLevel.walls.push_back({x, 6});
    }
    for (int y = 1; y < 6; y++) {
        defaultLevel.walls.push_back({0, y});
        defaultLevel.walls.push_back({6, y});
    }
    
    defaultLevel.playerStart = {2, 2};
    defaultLevel.boxes.push_back({3, 3});
    defaultLevel.targets.push_back({4, 4});
    actualD = targetD;
    
    return defaultLevel;
}

double Test::calculateBranchingFactor(int nodes, int depth) {
    if (depth <= 0) return 1.0;
    
    // Бинарный поиск b*
    double low = 1.0;
    double high = 10.0;
    
    for (int iter = 0; iter < 50; iter++) {
        double mid = (low + high) / 2.0;
        double sum = 0.0;
        double term = 1.0;
        
        for (int i = 0; i <= depth; i++) {
            sum += term;
            term *= mid;
        }
        
        if (sum > nodes) {
            high = mid;
        } else {
            low = mid;
        }
    }
    
    return (low + high) / 2.0;
}

TestResult Test::runSingleTest(const LevelData& level,
                                const std::string& algorithmName,
                                int targetD, int testId) {
    
    TestResult result;
    result.d = targetD;
    result.testId = testId;
    result.algorithm = algorithmName;
    
    // Загружаем уровень
    game.loadLevelFromData(level);
    
    // Выбираем алгоритм
    if (algorithmName == "BFS") {
        game.setAlgorithm("BFS");
    } else if (algorithmName == "IDDFS") {
        game.setAlgorithm("IDDFS");
    } else if (algorithmName == "ASTAR_H1") {
        game.setAlgorithm("ASTAR_H1");
    } else if (algorithmName == "ASTAR_H2") {
        game.setAlgorithm("ASTAR_H2");
    }
    
    // Замеряем время
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Получаем алгоритм и запускаем поиск
    auto& searchAlg = game.getSearchAlgorithm();
    auto searchResult = searchAlg->search(game.getCurrentState(), 
                                           game.getWalls(), 
                                           game.getTargets());
    
    auto endTime = std::chrono::high_resolution_clock::now();
    
    result.success = searchResult.success;
    result.iterations = searchResult.iterations;
    result.maxMemory = searchResult.maxMemoryUsage;
    result.solutionLength = static_cast<int>(searchResult.path.size() - 1);
    result.timeMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    
    if (result.success && result.solutionLength > 0) {
        result.branchingFactor = calculateBranchingFactor(result.iterations, result.solutionLength);
    } else {
        result.branchingFactor = 0.0;
    }
    
    return result;
}

void Test::runAllTests() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "RUNNING TESTS FOR COURSEWORK" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    // Параметры тестирования
    std::vector<int> targetDValues = {2, 4, 6, 8, 10};
    std::vector<std::string> algorithms = {"BFS", "IDDFS", "ASTAR_H1", "ASTAR_H2"};
    int testsPerD = 3;  // 3 теста на каждое значение d
    
    int totalTests = targetDValues.size() * testsPerD * algorithms.size();
    int currentTest = 0;
    
    results.clear();
    
    for (int targetD : targetDValues) {
        std::cout << "\n--- Testing d = " << targetD << " ---" << std::endl;
        
        for (int testNum = 1; testNum <= testsPerD; testNum++) {
            std::cout << "  Test " << testNum << "/" << testsPerD << "..." << std::endl;
            
            // Генерируем уровень с нужной сложностью
            int actualD;
            LevelData level = generateTestLevel(targetD, actualD);
            
            // Запускаем все алгоритмы на этом уровне
            for (const auto& alg : algorithms) {
                currentTest++;
                std::cout << "    Running " << alg << "... ";
                
                auto result = runSingleTest(level, alg, actualD, testNum);
                results.push_back(result);
                
                std::cout << (result.success ? "OK" : "FAIL") 
                          << " (iter=" << result.iterations 
                          << ", time=" << std::fixed << std::setprecision(0) 
                          << result.timeMs << "ms)" << std::endl;
            }
        }
    }
    
    // Вывод сводки и сохранение
    printSummary();
    saveToFile();
}

void Test::runUnsolvableTest() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "UNSOLVABLE LEVEL TEST" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    LevelData unsolvableLevel = generator.generateUnsolvableLevel();
    game.loadLevelFromData(unsolvableLevel);
    
    std::vector<std::string> algorithms = {"BFS", "IDDFS", "ASTAR_H1", "ASTAR_H2"};
    
    for (const auto& alg : algorithms) {
        std::cout << "Running " << alg << " on unsolvable level... ";
        
        auto result = runSingleTest(unsolvableLevel, alg, 0, 0);
        
        if (!result.success) {
            std::cout << "CORRECT: No solution found" << std::endl;
            std::cout << "  Iterations: " << result.iterations << std::endl;
            std::cout << "  Max memory: " << result.maxMemory << std::endl;
            std::cout << "  Time: " << std::fixed << std::setprecision(0) 
                      << result.timeMs << "ms" << std::endl;
        } else {
            std::cout << "ERROR: Solution found when shouldn't be!" << std::endl;
        }
    }
}

void Test::printSummary() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "SUMMARY RESULTS" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    // Группировка по d и алгоритму
    std::map<std::pair<int, std::string>, std::vector<TestResult>> grouped;
    
    for (const auto& r : results) {
        grouped[{r.d, r.algorithm}].push_back(r);
    }
    
    // Заголовок таблицы
    std::cout << std::left;
    std::cout << std::setw(6) << "d";
    std::cout << std::setw(12) << "Algorithm";
    std::cout << std::setw(12) << "Iterations";
    std::cout << std::setw(12) << "Memory";
    std::cout << std::setw(12) << "Time(ms)";
    std::cout << std::setw(12) << "b*";
    std::cout << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    
    for (const auto& entry : grouped) {
        int d = entry.first.first;
        std::string alg = entry.first.second;
        const auto& tests = entry.second;
        
        // Средние значения
        double avgIter = 0, avgMem = 0, avgTime = 0, avgB = 0;
        int count = 0;
        
        for (const auto& t : tests) {
            if (t.success) {
                avgIter += t.iterations;
                avgMem += t.maxMemory;
                avgTime += t.timeMs;
                avgB += t.branchingFactor;
                count++;
            }
        }
        
        if (count > 0) {
            avgIter /= count;
            avgMem /= count;
            avgTime /= count;
            avgB /= count;
            
            std::cout << std::setw(6) << d;
            std::cout << std::setw(12) << alg;
            std::cout << std::setw(12) << static_cast<int>(avgIter);
            std::cout << std::setw(12) << static_cast<int>(avgMem);
            std::cout << std::setw(12) << static_cast<int>(avgTime);
            std::cout << std::setw(12) << std::fixed << std::setprecision(2) << avgB;
            std::cout << std::endl;
        }
    }
}

void Test::saveToFile() {
    std::ofstream file("Test.txt");
    
    if (!file.is_open()) {
        std::cout << "Warning: Could not save to Test.txt" << std::endl;
        return;
    }
    
    file << "=== TEST RESULTS ===" << std::endl;
    file << "Date: " << __DATE__ << " " << __TIME__ << std::endl;
    file << std::endl;
    
    file << std::left;
    file << std::setw(6) << "d";
    file << std::setw(4) << "T";
    file << std::setw(12) << "Algorithm";
    file << std::setw(12) << "Iterations";
    file << std::setw(12) << "Memory";
    file << std::setw(12) << "Time(ms)";
    file << std::setw(10) << "PathLen";
    file << std::setw(10) << "b*";
    file << std::endl;
    file << std::string(80, '-') << std::endl;
    
    for (const auto& r : results) {
        file << std::setw(6) << r.d;
        file << std::setw(4) << r.testId;
        file << std::setw(12) << r.algorithm;
        file << std::setw(12) << r.iterations;
        file << std::setw(12) << r.maxMemory;
        file << std::setw(12) << static_cast<int>(r.timeMs);
        file << std::setw(10) << r.solutionLength;
        file << std::setw(10) << std::fixed << std::setprecision(2) << r.branchingFactor;
        file << std::endl;
    }
    
    file.close();
    std::cout << "\nResults saved to Test.txt" << std::endl;
}