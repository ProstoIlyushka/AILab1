#pragma once
#include <SFML/Graphics.hpp>
#include "State.h"
#include <vector>

class Renderer {
private:
    int tileSize;
    sf::Texture playerTexture;
    sf::Texture boxTexture;
    sf::Texture wallTexture;
    sf::Texture targetTexture;
    sf::Texture floorTexture;

    bool loadTextures();

public:
    Renderer(int tileSize);

    void renderLevel(sf::RenderWindow& window,
        const std::vector<sf::Vector2i>& walls,
        const std::vector<sf::Vector2i>& targets,
        const State& state);

    void renderPlayer(sf::RenderWindow& window, const sf::Vector2i& pos);
    void renderBoxes(sf::RenderWindow& window,
        const std::vector<sf::Vector2i>& boxes,
        const std::vector<sf::Vector2i>& targets);
};