#pragma once
#include "State.h"
#include <vector>
#include <unordered_map>
#include <chrono>
#include <iostream>

class SearchAlgorithm {
public:
    struct Result {
        std::vector<State> path;
        int iterations = 0;
        int maxOpenListSize = 0;
        int finalOpenListSize = 0;
        int maxMemoryUsage = 0;
        bool success = false;
        bool timeout = false;      // превышен лимит времени
        int solutionLength = 0;
    };

    // Ќастройки прерывани€
    struct Limits {
        int maxIterations = 50000;      // максимум итераций
        int maxTimeMs = 5000;           // максимум времени в миллисекундах
        bool enableTimeout = true;      // включить прерывание по времени
    };

    virtual ~SearchAlgorithm() = default;
    virtual Result search(const State& start,
        const std::vector<sf::Vector2i>& walls,
        const std::vector<sf::Vector2i>& targets) = 0;

    void setLimits(const Limits& limits) { m_limits = limits; }
    void setMaxIterations(int maxIter) { m_limits.maxIterations = maxIter; }
    void setMaxTimeMs(int maxTime) { m_limits.maxTimeMs = maxTime; }
    void enableTimeout(bool enable) { m_limits.enableTimeout = enable; }

protected:
    Limits m_limits;
    std::chrono::steady_clock::time_point m_startTime;

    bool shouldStop(int currentIterations) {
        if (currentIterations >= m_limits.maxIterations) {
            std::cout << "  Stopped: max iterations (" << m_limits.maxIterations << ")" << std::endl;
            return true;
        }

        if (m_limits.enableTimeout) {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_startTime).count();
            if (elapsed >= m_limits.maxTimeMs) {
                std::cout << "  Stopped: timeout (" << elapsed << " ms)" << std::endl;
                return true;
            }
        }

        return false;
    }

    void startTimer() {
        m_startTime = std::chrono::steady_clock::now();
    }

    std::vector<State> reconstructPath(
        const std::unordered_map<State, State, StateHash>& parent,
        const State& goal);

    void updateStatistics(Result& result, size_t openListSize, size_t closedSetSize);
};