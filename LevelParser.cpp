#include "LevelParser.h"
#include <algorithm>

LevelData LevelParser::parse(const std::vector<std::string>& levelLines) {
    LevelData data;
    data.height = static_cast<int>(levelLines.size());
    data.width = 0;

    for (const auto& line : levelLines) {
        data.width = std::max(data.width, static_cast<int>(line.length()));
    }

    for (int y = 0; y < data.height; y++) {
        const std::string& line = levelLines[y];
        for (int x = 0; x < static_cast<int>(line.length()); x++) {
            char c = line[x];
            sf::Vector2i pos(x, y);

            switch (c) {
            case '#':
                data.walls.push_back(pos);
                break;
            case '.':
                data.targets.push_back(pos);
                break;
            case '@':
                data.playerStart = pos;
                break;
            case '$':
                data.boxes.push_back(pos);
                break;
            case '*':
                data.boxes.push_back(pos);
                data.targets.push_back(pos);
                break;
            case '+':
                data.playerStart = pos;
                data.targets.push_back(pos);
                break;
            default:
                break;
            }
        }
    }

    return data;
}