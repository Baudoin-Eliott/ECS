#pragma once

#include "../ECS.h"
#include "SDL2/SDL.h"
#include <vector>
#include <string>
#include <map>

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
    int renderOrder = 0;


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

struct TiledObject {
    std::string name;
    std::string type;
    std::string objectGroup;

    float x, y;
    float width, height;

    std::map<std::string, std::string> properties;

    TiledObject() : x(0), y(0), width(0), height(0) {}

    TiledObject(const std::string& name_,const std::string& type_, const std::string& group_, float x_, float y_, float w_, float h_) : name(name_), type(type_), objectGroup(group_), x(x_), y(y_), width(w_), height(h_) {}

    bool hasProperty(const std::string& key) const {
        return properties.find(key) != properties.end();
    }

    std::string getProperty(const std::string& key) const {
        auto it = properties.find(key);
        return (it != properties.end()) ? it->second : "";
    }

    void print() const {
        std::cout << "[TiledObject] " << objectGroup << "/" << type 
                  << " '" << name << "' at (" << x << ", " << y 
                  << ") size: " << width << "x" << height << "\n";
    }

};





class TileMapComponent : public ECS::Component
{

public:
    std::vector<TileSet> tilesets;
    std::vector<Layer> layers;
    std::vector<TiledObject> objects;

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

    std::vector<TiledObject*> getObjectsByGroup(const std::string& group)  {
        std::vector<TiledObject*> result;
        for (auto& obj : objects){
            if (obj.objectGroup == group){
                result.push_back(&obj);
            }
        }
        return result;
    }

    std::vector<TiledObject*> getObjectsByType(const std::string& type)  {
        std::vector<TiledObject*> result;
        for (auto& obj : objects){
            if (obj.type == type){
                result.push_back(&obj);
            }
        }
        return result;
    }

    TiledObject* getObjectByName(const std::string& name)  {
        for (auto& obj : objects){
            if (obj.name == name){
                return &obj;
            }
        }
        return nullptr;
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



};