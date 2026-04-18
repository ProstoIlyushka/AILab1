#pragma once
#include "Game.h"
#include "LevelGenerator.h"
#include <vector>
#include <string>
#include <fstream>
#include <random>

struct TestResult {
    int d;                      // целевая глубина
    int testId;                 // номер теста
    std::string algorithm;      // название алгоритма
    int iterations;             // количество итераций
    int maxMemory;              // максимальная память
    double timeMs;              // время в миллисекундах
    bool success;               // найдено ли решение
    int solutionLength;         // длина найденного пути
    double branchingFactor;     // эффективный коэффициент ветвления
};

class Test {
private:
    Game& game;
    LevelGenerator generator;
    std::mt19937 rng;
    std::vector<TestResult> results;

    // Генерация случайного уровня с проверкой достижимости
    LevelData generateTestLevel(int targetD, int& actualD);

    // Запуск одного теста
    TestResult runSingleTest(const LevelData& level,
        const std::string& algorithmName,
        int targetD, int testId);

    // Расчёт эффективного коэффициента ветвления
    double calculateBranchingFactor(int nodes, int depth);

    // Сохранение результатов в файл
    void saveToFile();

    // Вывод результатов в консоль
    void printSummary();

public:
    Test(Game& gameRef);

    // Запуск всех тестов
    void runAllTests();

    // Тест на нерешаемую задачу
    void runUnsolvableTest();
};