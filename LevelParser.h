#pragma once
#include <SFML/System.hpp>
#include <vector>
#include <string>

struct LevelData {
    sf::Vector2i playerStart;
    std::vector<sf::Vector2i> walls;
    std::vector<sf::Vector2i> targets;
    std::vector<sf::Vector2i> boxes;
    int width = 0;
    int height = 0;
};

class LevelParser {
public:
    LevelData parse(const std::vector<std::string>& levelLines);
};