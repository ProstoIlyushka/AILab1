#pragma once
#include <SFML/System/Vector2.hpp>
#include <vector>

class State {
private:
    sf::Vector2i playerPos;
    std::vector<sf::Vector2i> boxes;

public:
    State();
    State(const sf::Vector2i& player, const std::vector<sf::Vector2i>& boxes);

    sf::Vector2i getPlayerPos() const { return playerPos; }
    const std::vector<sf::Vector2i>& getBoxes() const { return boxes; }

    void setPlayerPos(const sf::Vector2i& pos) { playerPos = pos; }
    void setBoxes(const std::vector<sf::Vector2i>& newBoxes) { boxes = newBoxes; }

    bool isGoal(const std::vector<sf::Vector2i>& targets) const;
    std::vector<State> getSuccessors(
        const std::vector<sf::Vector2i>& walls,
        const std::vector<sf::Vector2i>& targets) const;

    bool operator==(const State& other) const;
    void print() const;
};

// Хеш-функция для sf::Vector2i
struct Vector2iHash {
    size_t operator()(const sf::Vector2i& v) const;
};

// Хеш-функция для State
struct StateHash {
    size_t operator()(const State& s) const;
};