#include "Renderer.h"
#include <iostream>

Renderer::Renderer(int tileSize) : tileSize(tileSize) {
    if (!loadTextures()) {
        std::cerr << "Warning: Could not load textures!" << std::endl;
    }
}

bool Renderer::loadTextures() {
    // Загрузка текстур (пути нужно подставить ваши)
    bool success = true;

    if (!playerTexture.loadFromFile("textures/player.png")) {
        // Создаем текстуру-заглушку
        playerTexture.create(tileSize, tileSize);
        success = false;
    }

    if (!boxTexture.loadFromFile("textures/box.png")) {
        boxTexture.create(tileSize, tileSize);
        success = false;
    }

    if (!wallTexture.loadFromFile("textures/wall.png")) {
        wallTexture.create(tileSize, tileSize);
        success = false;
    }

    if (!targetTexture.loadFromFile("textures/target.png")) {
        targetTexture.create(tileSize, tileSize);
        success = false;
    }

    if (!floorTexture.loadFromFile("textures/floor.png")) {
        floorTexture.create(tileSize, tileSize);
        success = false;
    }

    return success;
}

void Renderer::renderLevel(sf::RenderWindow& window,
    const std::vector<sf::Vector2i>& walls,
    const std::vector<sf::Vector2i>& targets,
    const State& state) {

    sf::Sprite sprite;

    // Отрисовка пола
    sprite.setTexture(floorTexture);
    for (int y = 0; y < 10; y++) {
        for (int x = 0; x < 10; x++) {
            sprite.setPosition(x * tileSize, y * tileSize);
            window.draw(sprite);
        }
    }

    // Отрисовка стен
    sprite.setTexture(wallTexture);
    for (const auto& wall : walls) {
        sprite.setPosition(wall.x * tileSize, wall.y * tileSize);
        window.draw(sprite);
    }

    // Отрисовка меток
    sprite.setTexture(targetTexture);
    for (const auto& target : targets) {
        sprite.setPosition(target.x * tileSize, target.y * tileSize);
        window.draw(sprite);
    }
}

void Renderer::renderPlayer(sf::RenderWindow& window, const sf::Vector2i& pos) {
    sf::Sprite sprite;
    sprite.setTexture(playerTexture);
    sprite.setPosition(pos.x * tileSize, pos.y * tileSize);
    window.draw(sprite);
}

void Renderer::renderBoxes(sf::RenderWindow& window,
    const std::vector<sf::Vector2i>& boxes,
    const std::vector<sf::Vector2i>& targets) {

    sf::Sprite sprite;
    sprite.setTexture(boxTexture);

    for (const auto& box : boxes) {
        sprite.setPosition(box.x * tileSize, box.y * tileSize);
        window.draw(sprite);
    }
}