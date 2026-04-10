#include "State.h"
#include <algorithm>
#include <iostream>
#include <functional>

State::State()
    : playerPos(0, 0), boxes() {
}

State::State(const sf::Vector2i& player, const std::vector<sf::Vector2i>& boxList)
    : playerPos(player), boxes(boxList) {
}

bool State::isGoal(const std::vector<sf::Vector2i>& targets) const {
    for (const auto& box : boxes) {
        if (std::find(targets.begin(), targets.end(), box) == targets.end()) {
            return false;
        }
    }
    return true;
}

std::vector<State> State::getSuccessors(
    const std::vector<sf::Vector2i>& walls,
    const std::vector<sf::Vector2i>& targets) const {

    std::vector<State> successors;

    const std::vector<sf::Vector2i> directions = {
        {0, -1}, {0, 1}, {-1, 0}, {1, 0}
    };

    for (const auto& dir : directions) {
        sf::Vector2i newPos = playerPos + dir;

        // Проверка стены
        bool isWall = false;
        for (const auto& wall : walls) {
            if (wall == newPos) {
                isWall = true;
                break;
            }
        }
        if (isWall) continue;

        // Проверка ящика
        bool isBox = false;
        int boxIndex = -1;
        for (size_t i = 0; i < boxes.size(); i++) {
            if (boxes[i] == newPos) {
                isBox = true;
                boxIndex = static_cast<int>(i);
                break;
            }
        }

        if (isBox) {
            sf::Vector2i pushPos = newPos + dir;

            // Проверка стены на позиции толчка
            bool pushWall = false;
            for (const auto& wall : walls) {
                if (wall == pushPos) {
                    pushWall = true;
                    break;
                }
            }

            // Проверка другого ящика на позиции толчка
            bool pushBox = false;
            for (const auto& box : boxes) {
                if (box == pushPos) {
                    pushBox = true;
                    break;
                }
            }

            if (!pushWall && !pushBox) {
                std::vector<sf::Vector2i> newBoxes = boxes;
                if (boxIndex >= 0) {
                    newBoxes.erase(newBoxes.begin() + boxIndex);
                }
                newBoxes.push_back(pushPos);
                successors.push_back(State(newPos, newBoxes));
            }
        }
        else {
            successors.push_back(State(newPos, boxes));
        }
    }

    return successors;
}

bool State::operator==(const State& other) const {
    if (playerPos != other.playerPos) return false;
    if (boxes.size() != other.boxes.size()) return false;

    auto sorted1 = boxes;
    auto sorted2 = other.boxes;

    std::sort(sorted1.begin(), sorted1.end(), [](const sf::Vector2i& a, const sf::Vector2i& b) {
        return a.x < b.x || (a.x == b.x && a.y < b.y);
        });
    std::sort(sorted2.begin(), sorted2.end(), [](const sf::Vector2i& a, const sf::Vector2i& b) {
        return a.x < b.x || (a.x == b.x && a.y < b.y);
        });

    return sorted1 == sorted2;
}

void State::print() const {
    std::cout << "Player: (" << playerPos.x << ", " << playerPos.y << ")" << std::endl;
    std::cout << "Boxes: ";
    for (const auto& box : boxes) {
        std::cout << "(" << box.x << ", " << box.y << ") ";
    }
    std::cout << std::endl;
}

// Реализация Vector2iHash
size_t Vector2iHash::operator()(const sf::Vector2i& v) const {
    std::hash<int> intHash;
    return intHash(v.x) ^ (intHash(v.y) << 1);
}

// Реализация StateHash
size_t StateHash::operator()(const State& s) const {
    Vector2iHash vecHash;

    size_t h = vecHash(s.getPlayerPos());

    for (const auto& box : s.getBoxes()) {
        h ^= vecHash(box) + 0x9e3779b9 + (h << 6) + (h >> 2);
    }

    return h;
}