#include "Game.h"
#include "BFS.h"
#include "DFS.h"
#include "IDDFS.h"
#include <iostream>
#include <chrono>
#include <algorithm>
#include <cmath>

Game::Game()
    : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Sokoban"),
    tileSize(96),
    isAnimating(false),
    animationStep(0),
    showStats(false),
    playerScale(1.0f),
    boxScale(1.0f),
    wallScale(1.0f),
    targetScale(1.0f),
    floorScale(1.0f),
    playerOffset(0, 0),
    boxOffset(0, 0),
    wallOffset(0, 0),
    targetOffset(0, 0),
    floorOffset(0, 0) {

    std::cout << "\n=== SOKOBAN ===" << std::endl;
    std::cout << "Tile size: " << tileSize << " pixels" << std::endl;

    // Включаем вертикальную синхронизацию для устранения разрывов
    window.setVerticalSyncEnabled(true);

    loadTextures();
    updateScales();

    std::vector<std::string> level = {
    "#######",
    "#     #",
    "#@$   #",
    "#  .  #",
    "#     #",
    "#     #",
    "#######"
    };

    loadLevel(level);
    setAlgorithm("BFS");

    std::cout << "Ready! Use WASD/Arrows to move, B/N for search, Tab for stats, F1 for help" << std::endl;
}

void Game::run() {
    sf::Clock clock;

    while (window.isOpen()) {
        processInput();
        update();
        render();
        clock.restart();
    }
}

bool Game::loadTextures() {
    bool success = true;

    // Включаем сглаживание (с отступами оно не будет создавать артефакты)
    playerTexture.setSmooth(true);
    boxTexture.setSmooth(true);
    wallTexture.setSmooth(true);
    targetTexture.setSmooth(true);
    floorTexture.setSmooth(true);

    // Загрузка текстур с добавлением отступа в 1 пиксель
    loadTextureWithPadding(playerTexture, "Assets/Egor/Idle/South/Слой 1.png", 1);
    loadTextureWithPadding(boxTexture, "Assets/Stone/Stone.png", 1);
    loadTextureWithPadding(wallTexture, "Assets/Flora/Stump.png", 1);
    loadTextureWithPadding(targetTexture, "Assets/Flora/Pine.png", 1);
    loadTextureWithPadding(floorTexture, "Assets/Level/BG.png", 1);

    // Проверка загрузки
    if (playerTexture.getSize().x == 0) {
        std::cout << "Warning: Player texture failed to load" << std::endl;
        playerTexture.create(tileSize, tileSize);
        success = false;
    }
    if (boxTexture.getSize().x == 0) {
        std::cout << "Warning: Box texture failed to load" << std::endl;
        boxTexture.create(tileSize, tileSize);
        success = false;
    }
    if (wallTexture.getSize().x == 0) {
        std::cout << "Warning: Wall texture failed to load" << std::endl;
        wallTexture.create(tileSize, tileSize);
        success = false;
    }
    if (targetTexture.getSize().x == 0) {
        std::cout << "Warning: Target texture failed to load" << std::endl;
        targetTexture.create(tileSize, tileSize);
        success = false;
    }
    if (floorTexture.getSize().x == 0) {
        std::cout << "Warning: Floor texture failed to load" << std::endl;
        floorTexture.create(tileSize, tileSize);
        success = false;
    }

    return success;
}

void Game::updateScales() {
    auto calculateScaleAndOffset = [this](sf::Texture& texture, float& scale, sf::Vector2f& offset) {
        if (texture.getSize().x == 0 || texture.getSize().y == 0) {
            scale = 1.0f;
            offset = sf::Vector2f(0, 0);
            return;
        }

        // Для текстур с отступами масштабируем так же
        float scaleX = static_cast<float>(tileSize) / texture.getSize().x;
        float scaleY = static_cast<float>(tileSize) / texture.getSize().y;
        scale = std::min(scaleX, scaleY);

        float scaledWidth = texture.getSize().x * scale;
        float scaledHeight = texture.getSize().y * scale;

        offset = sf::Vector2f(
            std::round((tileSize - scaledWidth) / 2.0f),
            std::round((tileSize - scaledHeight) / 2.0f)
        );

        // Корректируем масштаб для целых пикселей
        int targetWidth = static_cast<int>(std::round(texture.getSize().x * scale));
        if (targetWidth > 0) {
            scale = static_cast<float>(targetWidth) / texture.getSize().x;
        }
        };

    calculateScaleAndOffset(playerTexture, playerScale, playerOffset);
    calculateScaleAndOffset(boxTexture, boxScale, boxOffset);
    calculateScaleAndOffset(wallTexture, wallScale, wallOffset);
    calculateScaleAndOffset(targetTexture, targetScale, targetOffset);
    calculateScaleAndOffset(floorTexture, floorScale, floorOffset);

    std::cout << "\n=== Texture Scales (with padding) ===" << std::endl;
    std::cout << "Player: size=" << playerTexture.getSize().x << "x" << playerTexture.getSize().y
        << ", scale=" << playerScale << ", offset=(" << playerOffset.x << "," << playerOffset.y << ")" << std::endl;
    std::cout << "Box: size=" << boxTexture.getSize().x << "x" << boxTexture.getSize().y
        << ", scale=" << boxScale << ", offset=(" << boxOffset.x << "," << boxOffset.y << ")" << std::endl;
    std::cout << "Wall: size=" << wallTexture.getSize().x << "x" << wallTexture.getSize().y
        << ", scale=" << wallScale << ", offset=(" << wallOffset.x << "," << wallOffset.y << ")" << std::endl;
    std::cout << "Target: size=" << targetTexture.getSize().x << "x" << targetTexture.getSize().y
        << ", scale=" << targetScale << ", offset=(" << targetOffset.x << "," << targetOffset.y << ")" << std::endl;
    std::cout << "Floor: size=" << floorTexture.getSize().x << "x" << floorTexture.getSize().y
        << ", scale=" << floorScale << ", offset=(" << floorOffset.x << "," << floorOffset.y << ")" << std::endl;
}

sf::Vector2f Game::getDrawPosition(const sf::Vector2i& gridPos, const sf::Vector2f& offset) {
    // Округляем позицию до целых пикселей для устранения артефактов
    return sf::Vector2f(
        std::round(static_cast<float>(gridPos.x * tileSize) + offset.x),
        std::round(static_cast<float>(gridPos.y * tileSize) + offset.y)
    );
}

sf::Image Game::cropAndAddPadding(const sf::Image& image, int padding) {
    sf::Vector2u size = image.getSize();

    // Находим непрозрачные границы
    int left = size.x, right = 0, top = size.y, bottom = 0;

    for (unsigned int y = 0; y < size.y; y++) {
        for (unsigned int x = 0; x < size.x; x++) {
            if (image.getPixel(x, y).a > 0) {  // Не полностью прозрачный
                left = std::min(left, (int)x);
                right = std::max(right, (int)x);
                top = std::min(top, (int)y);
                bottom = std::max(bottom, (int)y);
            }
        }
    }

    // Если нет непрозрачных пикселей, возвращаем оригинал
    if (left > right || top > bottom) {
        return image;
    }

    // Вычисляем размеры обрезанного изображения
    int croppedWidth = right - left + 1;
    int croppedHeight = bottom - top + 1;

    // Создаём изображение с отступами
    int newWidth = croppedWidth + padding * 2;
    int newHeight = croppedHeight + padding * 2;

    sf::Image paddedImage;
    paddedImage.create(newWidth, newHeight, sf::Color::Transparent);

    // Копируем непрозрачную часть в центр с отступами
    for (int y = 0; y < croppedHeight; y++) {
        for (int x = 0; x < croppedWidth; x++) {
            sf::Color pixel = image.getPixel(left + x, top + y);
            paddedImage.setPixel(padding + x, padding + y, pixel);
        }
    }

    return paddedImage;
}

void Game::loadTextureWithPadding(sf::Texture& texture, const std::string& path, int padding) {
    sf::Image image;
    if (image.loadFromFile(path)) {
        sf::Image padded = cropAndAddPadding(image, padding);
        texture.loadFromImage(padded);

        std::cout << "Loaded texture: " << path << " -> original: "
            << image.getSize().x << "x" << image.getSize().y
            << ", with padding: " << padded.getSize().x << "x" << padded.getSize().y << std::endl;
    }
    else {
        std::cout << "Failed to load texture: " << path << std::endl;
        texture.create(tileSize, tileSize);
    }
}

void Game::loadLevel(const std::vector<std::string>& levelLines) {
    LevelParser parser;
    levelData = parser.parse(levelLines);

    walls = levelData.walls;
    targets = levelData.targets;

    initialState = State(levelData.playerStart, levelData.boxes);
    currentState = initialState;

    std::cout << "\n=== Level Loaded ===" << std::endl;
    std::cout << "Size: " << levelData.width << "x" << levelData.height << std::endl;
    std::cout << "Walls: " << walls.size() << std::endl;
    std::cout << "Targets: " << targets.size() << std::endl;
    std::cout << "Boxes: " << currentState.getBoxes().size() << std::endl;
    std::cout << "Player at: (" << currentState.getPlayerPos().x << "," << currentState.getPlayerPos().y << ")" << std::endl;
}

void Game::resetGame() {
    currentState = initialState;
    solutionPath.clear();
    isAnimating = false;
    animationStep = 0;
    std::cout << "Game reset!" << std::endl;
}

void Game::setAlgorithm(const std::string& name) {
    if (name == "BFS") {
        searchAlgorithm = std::make_unique<BFS>();
        std::cout << "Algorithm set to BFS" << std::endl;
    }
    else if (name == "DFS") {
        searchAlgorithm = std::make_unique<DFS>();
        std::cout << "Algorithm set to DFS" << std::endl;
    }
    else if (name == "IDDFS") {  // Добавить эту секцию
        searchAlgorithm = std::make_unique<IDDFS>();
        std::cout << "Algorithm set to IDDFS" << std::endl;
    }
}

void Game::solve() {
    if (isAnimating) return;

    std::cout << "\n=== Solving ===" << std::endl;

    auto startTime = std::chrono::high_resolution_clock::now();
    auto result = searchAlgorithm->search(currentState, walls, targets);
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    std::cout << "Time: " << duration.count() << " ms" << std::endl;
    std::cout << getStatsText(result) << std::endl;

    if (result.success) {
        solutionPath = result.path;
        animationStep = 0;
        isAnimating = true;
        std::cout << "Solution found! Path length: " << result.path.size() - 1 << std::endl;
    }
    else {
        std::cout << "No solution found!" << std::endl;
    }
}

void Game::animateSolution() {
    static sf::Clock animationClock;
    static float frameTime = 0.0f;

    frameTime += animationClock.restart().asSeconds();

    if (frameTime >= 0.3f) {
        frameTime = 0.0f;

        if (animationStep < solutionPath.size() - 1) {
            animationStep++;
            currentState = solutionPath[animationStep];
        }
        else {
            isAnimating = false;
            solutionPath.clear();
            std::cout << "Animation finished!" << std::endl;
        }
    }
}

void Game::handleMovement(const sf::Vector2i& direction) {
    if (isAnimating) return;

    auto successors = currentState.getSuccessors(walls, targets);
    sf::Vector2i newPos = currentState.getPlayerPos() + direction;

    for (const auto& nextState : successors) {
        if (nextState.getPlayerPos() == newPos) {
            currentState = nextState;

            if (currentState.isGoal(targets)) {
                std::cout << "=== VICTORY! ===" << std::endl;
            }
            return;
        }
    }
}

void Game::processInput() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        else if (event.type == sf::Event::KeyPressed) {
            // Движение
            if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::W) {
                handleMovement({ 0, -1 });
            }
            else if (event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::S) {
                handleMovement({ 0, 1 });
            }
            else if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::A) {
                handleMovement({ -1, 0 });
            }
            else if (event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::D) {
                handleMovement({ 1, 0 });
            }
            // Сброс
            else if (event.key.code == sf::Keyboard::R) {
                resetGame();
            }
            // BFS поиск
            else if (event.key.code == sf::Keyboard::B) {
                setAlgorithm("BFS");
                solve();
            }
            // DFS поиск
            else if (event.key.code == sf::Keyboard::N) {
                setAlgorithm("DFS");
                solve();
            }
            else if (event.key.code == sf::Keyboard::M) {
                setAlgorithm("IDDFS");
                solve();
            }
            // Статистика
            else if (event.key.code == sf::Keyboard::Tab) {
                showStats = !showStats;
                std::cout << "Stats: " << (showStats ? "ON" : "OFF") << std::endl;
            }
            // Помощь
            else if (event.key.code == sf::Keyboard::F1) {
                std::cout << "\n=== CONTROLS ===" << std::endl;
                std::cout << "W/Up    - Move up" << std::endl;
                std::cout << "S/Down  - Move down" << std::endl;
                std::cout << "A/Left  - Move left" << std::endl;
                std::cout << "D/Right - Move right" << std::endl;
                std::cout << "R       - Reset level" << std::endl;
                std::cout << "B       - BFS search" << std::endl;
                std::cout << "N       - DFS search" << std::endl;
                std::cout << "Tab     - Toggle statistics" << std::endl;
                std::cout << "F1      - Show this help" << std::endl;
                std::cout << "=================" << std::endl;
            }
        }
    }
}

void Game::update() {
    if (isAnimating) {
        animateSolution();
    }
}

void Game::render() {
    window.clear(sf::Color(30, 30, 40));

    sf::Sprite sprite;

    // Пол (заполняет всю клетку) - рисуем первым слоем
    sprite.setTexture(floorTexture);
    sprite.setScale(floorScale, floorScale);
    for (int y = 0; y < levelData.height; y++) {
        for (int x = 0; x < levelData.width; x++) {
            sprite.setPosition(getDrawPosition({ x, y }, floorOffset));
            window.draw(sprite);
        }
    }

    // Стены
    sprite.setTexture(wallTexture);
    sprite.setScale(wallScale, wallScale);
    for (const auto& wall : walls) {
        sprite.setPosition(getDrawPosition(wall, wallOffset));
        window.draw(sprite);
    }

    // Метки (цели)
    sprite.setTexture(targetTexture);
    sprite.setScale(targetScale, targetScale);
    for (const auto& target : targets) {
        sprite.setPosition(getDrawPosition(target, targetOffset));
        window.draw(sprite);
    }

    // Ящики
    sprite.setTexture(boxTexture);
    sprite.setScale(boxScale, boxScale);
    for (const auto& box : currentState.getBoxes()) {
        sprite.setPosition(getDrawPosition(box, boxOffset));
        window.draw(sprite);
    }

    // Игрок (рисуем последним, чтобы был поверх всех)
    sprite.setTexture(playerTexture);
    sprite.setScale(playerScale, playerScale);
    sf::Vector2i playerPos = currentState.getPlayerPos();
    sprite.setPosition(getDrawPosition(playerPos, playerOffset));
    window.draw(sprite);

    // UI
    if (showStats) {
        drawUI();
    }

    window.display();
}

void Game::drawUI() {
    static sf::Font font;
    static bool fontLoaded = false;

    if (!fontLoaded) {
        if (font.loadFromFile("Shafarik/Shafarik-Regular.otf")) {
            fontLoaded = true;
            std::cout << "Font loaded!" << std::endl;
        }
        else if (font.loadFromFile("arial.ttf")) {
            fontLoaded = true;
            std::cout << "Font loaded: arial.ttf" << std::endl;
        }
        else {
            if (font.loadFromFile("D:/СИИ/Лабы 1-3/AILab1-3/AILab1-3/Shafarik/Shafarik-Regular.otf")) {
                fontLoaded = true;
                std::cout << "Font loaded from full path!" << std::endl;
            }
            else {
                std::cout << "No font found! UI text disabled." << std::endl;
                return;
            }
        }
    }

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(18);
    text.setFillColor(sf::Color::White);
    text.setOutlineColor(sf::Color::Black);
    text.setOutlineThickness(1.0f);
    text.setPosition(10, 10);

    std::string info = "SOKOBAN\n";
    info += "Controls:\n";
    info += "  W/Up    - Move up\n";
    info += "  S/Down  - Move down\n";
    info += "  A/Left  - Move left\n";
    info += "  D/Right - Move right\n";
    info += "  R       - Reset level\n";
    info += "  B       - BFS search\n";
    info += "  N       - DFS search\n";
    info += "  M       - IDDFS search\n";
    info += "  Tab     - Toggle stats\n";
    info += "  F1      - Help\n";

    if (currentState.isGoal(targets)) {
        info += "\n  *** VICTORY! ***\n";
    }

    if (showStats) {
        info += "\n=== STATISTICS ===\n";
        info += "Boxes: " + std::to_string(currentState.getBoxes().size()) + "\n";
        info += "Targets: " + std::to_string(targets.size()) + "\n";

        int boxesOnTargets = 0;
        for (const auto& box : currentState.getBoxes()) {
            for (const auto& target : targets) {
                if (box == target) {
                    boxesOnTargets++;
                    break;
                }
            }
        }
        info += "On targets: " + std::to_string(boxesOnTargets) + "/" + std::to_string(targets.size()) + "\n";

        if (dynamic_cast<BFS*>(searchAlgorithm.get())) {
            info += "Algorithm: BFS\n";
        }
        else if (dynamic_cast<DFS*>(searchAlgorithm.get())) {
            info += "Algorithm: DFS\n";
        }
    }

    text.setString(info);
    window.draw(text);
}

std::string Game::getStatsText(const SearchAlgorithm::Result& result) {
    std::string stats = "=== Statistics ===\n";
    stats += "Iterations: " + std::to_string(result.iterations) + "\n";
    stats += "Path length: " + std::to_string(result.path.size() - 1) + "\n";
    stats += "Max Open List Size: " + std::to_string(result.maxOpenListSize) + "\n";
    stats += "Final Open List Size: " + std::to_string(result.finalOpenListSize) + "\n";
    stats += "Max Memory Usage: " + std::to_string(result.maxMemoryUsage) + " nodes\n";
    return stats;
}