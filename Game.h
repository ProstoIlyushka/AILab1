#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <string>
#include "State.h"
#include "SearchAlgorithm.h"
#include "LevelParser.h"

class Game {
private:
    sf::RenderWindow window;
    int tileSize;

    State currentState;
    State initialState;
    std::vector<sf::Vector2i> walls;
    std::vector<sf::Vector2i> targets;
    LevelData levelData;

    std::unique_ptr<SearchAlgorithm> searchAlgorithm;
    std::vector<State> solutionPath;
    size_t animationStep;
    bool isAnimating;
    bool showStats;

    // Текстуры
    sf::Texture playerTexture;
    sf::Texture boxTexture;
    sf::Texture wallTexture;
    sf::Texture targetTexture;
    sf::Texture floorTexture;

    // Масштабы для каждой текстуры
    float playerScale;
    float boxScale;
    float wallScale;
    float targetScale;
    float floorScale;

    // Смещения для спрайтов
    sf::Vector2f playerOffset;
    sf::Vector2f boxOffset;
    sf::Vector2f wallOffset;
    sf::Vector2f targetOffset;
    sf::Vector2f floorOffset;

    static constexpr int WINDOW_WIDTH = 1024;
    static constexpr int WINDOW_HEIGHT = 768;

public:
    Game();
    void run();

private:
    void processInput();
    void update();
    void render();
    void loadLevel(const std::vector<std::string>& levelLines);
    void resetGame();
    void solve();
    void animateSolution();
    void handleMovement(const sf::Vector2i& direction);
    void setAlgorithm(const std::string& name);
    void drawUI();
    std::string getStatsText(const SearchAlgorithm::Result& result);
    bool loadTextures();
    void updateScales();
    sf::Vector2f getDrawPosition(const sf::Vector2i& gridPos, const sf::Vector2f& offset);
    sf::Image cropAndAddPadding(const sf::Image& image, int padding = 1);
    void loadTextureWithPadding(sf::Texture& texture, const std::string& path, int padding = 1);
};