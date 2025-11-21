#pragma once

#include "../ECS.h"
#include "SDL2/SDL.h"
#include <vector>
#include <string>

struct TileSet
{
    int firstGID;
    int tileWidth;
    int tileHeight;
    int columns;
    int tileCount;
    SDL_Texture *texture;

    TileSet()
        : firstGID(0),
          tileHeight(0),
          tileWidth(0),
          columns(0),
          tileCount(0),
          texture(nullptr) {}

    SDL_Rect getTileRect(int localID) const
    {
        SDL_Rect rect;

        int col = localID % columns;
        int row = localID / columns;

        rect.x = col * tileWidth;
        rect.y = row * tileHeight;
        rect.w = tileWidth;
        rect.h = tileHeight;

        return rect;
    }
};

struct Layer
{
    std::string name;
    int width;
    int height;
    std::vector<int> tiles;

    Layer() : name(""), width(0), height(0) {}

    int getTileAt(int x, int y) const
    {

        if (x < 0 || x >= width || y < 0 || y >= height)
        {
            return 0;
        }

        int index = y * width + x;
        return tiles[index];
    }
    void setTileAt(int x, int y, int tileId)
    {
        if (x < 0 || x >= width || y < 0 || y >= height)
        {
            return;
        }
        int index = y * width + x;
        tiles[index] = tileId;
    }
};

class TileMapComponent : public ECS::Component
{

public:
    std::vector<TileSet> tilesets;
    std::vector<Layer> layers;

    int mapWidth;
    int mapHeight;
    int tileWidth;
    int tileHeight;

    TileMapComponent()
        : mapWidth(0), mapHeight(0), tileHeight(0), tileWidth(0) {}

    TileSet *getTilesetFromGID(int gid)
    {

        TileSet *result = nullptr;

        for (auto &ts : tilesets)
        {
            if (gid >= ts.firstGID)
                result = &ts;
        }
        return result;
    }

    Layer *getLayerByName(const std::string& name)
    {

        Layer *result = nullptr;

        for (auto &layer : layers)
        {
            if (layer.name == name)
                result = &layer;
        }
        return result;
    }

    int getMapWidthInPixels() const {
        return mapWidth * tileWidth;
    }

    int getMapHeightInPixels() const {
        return mapHeight * tileHeight;
    }

    void init() override {
        std::cout << "[TileMapComponent] Initialized - Map Size: " << mapWidth << "x" << mapHeight << " (" << getMapWidthInPixels() << "x" << getMapHeightInPixels() <<" pixels)\n";
        std::cout << "[TileMapComponent] Tilesets :" << tilesets.size() << ", layers: " << layers.size() << "\n"; 
    }

};