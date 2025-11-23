#pragma once

#include "../Components/TileMapComponent.h"
#include <tinyxml2.h>
#include <SDL2/SDL.h>
#include <sstream>
#include <iostream>

class TiledParser
{
public:
    static bool loadFromFile(const std::string &path, TileMapComponent &tileMapComponent, SDL_Renderer *renderer)
    {
        tinyxml2::XMLDocument doc;
        if (doc.LoadFile(path.c_str()) != tinyxml2::XML_SUCCESS)
        {
            std::cout << "[TiledParser] Error while loading: " << path << "\n";
            return false;
        }
        tinyxml2::XMLElement *map = doc.FirstChildElement("map");
        if (!map)
        {
            std::cout << "[TiledParser] Error, element map not found\n";
            return false;
        }
        tileMapComponent.mapWidth = map->IntAttribute("width");
        tileMapComponent.mapHeight = map->IntAttribute("height");
        tileMapComponent.tileWidth = map->IntAttribute("tilewidth");
        tileMapComponent.tileHeight = map->IntAttribute("tileheight");

        tinyxml2::XMLElement *currentTileset = map->FirstChildElement("tileset");
        std::string directory = path.substr(0, path.find_last_of("/\\") + 1);
        while (currentTileset)
        {
            parseTileset(currentTileset, tileMapComponent, renderer, directory);
            currentTileset = currentTileset->NextSiblingElement("tileset");
        }

        tinyxml2::XMLElement *currentLayer = map->FirstChildElement("layer");

        while (currentLayer)
        {
            parseLayer(currentLayer, tileMapComponent);
            currentLayer = currentLayer->NextSiblingElement("layer");
        }

        tinyxml2::XMLElement *currentObjectGroup = map->FirstChildElement("objectgroup");

        while (currentObjectGroup)
        {
            parseObjectGroup(currentObjectGroup, tileMapComponent);
            currentObjectGroup = currentObjectGroup->NextSiblingElement("objectgroup");
        }

        return true;
    }

private:
    static bool parseTileset(tinyxml2::XMLElement *tileset, TileMapComponent &tileMapComponent, SDL_Renderer *renderer, const std::string &baseDirectory)
    {
        TileSet thisTileset = {};
        thisTileset.firstGID = tileset->IntAttribute("firstgid");
        const char *source = tileset->Attribute("source");

        if (source)
        {

            std::string fullPath = baseDirectory + std::string(source);
            tinyxml2::XMLDocument tsxDoc;

            if (tsxDoc.LoadFile(fullPath.c_str()) != tinyxml2::XML_SUCCESS)
            {
                std::cout << "[TiledParser] Error loading external TSX: " << fullPath << "\n";
                return false;
            }

            tinyxml2::XMLElement *tsxTileset = tsxDoc.FirstChildElement("tileset");
            if (!tsxTileset)
            {
                std::cout << "[TiledParser] No <tileset> element in TSX\n";
                return false;
            }

            thisTileset.tileWidth = tsxTileset->IntAttribute("tilewidth");
            thisTileset.tileHeight = tsxTileset->IntAttribute("tileheight");
            thisTileset.columns = tsxTileset->IntAttribute("columns");
            thisTileset.tileCount = tsxTileset->IntAttribute("tilecount");

            tinyxml2::XMLElement *image = tsxTileset->FirstChildElement("image");
            if (!image)
            {
                std::cerr << "[TiledParser] ERROR: No <image> in tileset!\n";
                return false;
            }

            const char *imageSource = image->Attribute("source");
            if (!imageSource)
            {
                std::cerr << "[TiledParser] ERROR: <image> has no source attribute!\n";
                return false;
            }

            std::string imagePath = baseDirectory + std::string(imageSource);
            thisTileset.texture = loadTexture(imagePath.c_str(), renderer);

            if (!thisTileset.texture)
            {
                std::cerr << "[TiledParser] ERROR: Failed to load texture: " << imagePath << "\n";
                return false;
            }
        }
        else
        {

            thisTileset.tileWidth = tileset->IntAttribute("tilewidth");
            thisTileset.tileHeight = tileset->IntAttribute("tileheight");

            tinyxml2::XMLElement *image = tileset->FirstChildElement("image");
            if (!image)
            {
                std::cerr << "[TiledParser] ERROR: No <image> in tileset!\n";
                return false;
            }

            const char *imageSource = image->Attribute("source");
            if (!imageSource)
            {
                std::cerr << "[TiledParser] ERROR: <image> has no source attribute!\n";
                return false;
            }

            std::string imagePath = baseDirectory + std::string(imageSource);
            thisTileset.texture = loadTexture(imagePath.c_str(), renderer);

            if (!thisTileset.texture)
            {
                std::cerr << "[TiledParser] ERROR: Failed to load texture: " << imagePath << "\n";
                return false;
            }

            thisTileset.columns = image->IntAttribute("width") / thisTileset.tileWidth;
            thisTileset.tileCount = thisTileset.columns * (image->IntAttribute("height") / thisTileset.tileHeight);
        }

        tileMapComponent.tilesets.push_back(thisTileset);

        return true;
    }

    static bool parseLayer(tinyxml2::XMLElement *layer, TileMapComponent &tileMapComponent)
    {
        Layer thisLayer = {};

        thisLayer.name = layer->Attribute("name");
        thisLayer.width = layer->IntAttribute("width");
        thisLayer.height = layer->IntAttribute("height");

        tinyxml2::XMLElement *propertiesElem = layer->FirstChildElement("properties");
        if (propertiesElem)
        {
            tinyxml2::XMLElement *property = propertiesElem->FirstChildElement("property");
            while (property)
            {
                std::string propName = property->Attribute("name");
                if (propName == "renderOrder")
                {
                    thisLayer.renderOrder = property->IntAttribute("value");
                    std::cout << "[TiledParser] Layer '" << thisLayer.name
                              << "' renderOrder: " << thisLayer.renderOrder << "\n";
                }
                property = property->NextSiblingElement("property");
            }
        }

        tinyxml2::XMLElement *data = layer->FirstChildElement("data");
        if (!data)
        {
            std::cout << "[TiledParser] Error while loading data in one layer.\n";
            return false;
        }
        if (std::string(data->Attribute("encoding")) != "csv")
        {
            std::cout << "[TiledParser] Error, encoding != csv in one layer.\n";
            return false;
        }
        std::string intData = data->GetText();
        thisLayer.tiles = parseCSV(intData);
        if (thisLayer.tiles.size() == 0)
        {
            std::cout << "[TiledParser] error, data is null in one layer.\n";
            return false;
        }
        tileMapComponent.layers.push_back(thisLayer);

        return true;
    }

    static std::vector<int> parseCSV(std::string entry)
    {
        std::vector<int> result;
        std::stringstream ss(entry);
        std::string token;
        while (std::getline(ss, token, ','))
        {
            if (token.empty())
                continue;

            bool hasDigit = false;
            for (char c : token)
            {
                if (std::isdigit(c))
                {
                    hasDigit = true;
                    break;
                }
            }

            if (hasDigit)
            {

                try
                {
                    int value = std::stoi(token);
                    result.push_back(value);
                }
                catch (const std::exception &e)
                {
                    std::cerr << "[TiledParser] error parsing number:" << token << "\n";
                }
            }
        }

        return result;
    }

    static bool parseObjectGroup(tinyxml2::XMLElement *objectGroup, TileMapComponent &tileMapComponent)
    {
        std::string name = objectGroup->Attribute("name");
        tinyxml2::XMLElement *obj = objectGroup->FirstChildElement("object");
        while (obj)
        {
            TiledObject object = parseObject(obj, name);
            tileMapComponent.objects.push_back(object);
            obj = obj->NextSiblingElement("object");
        }
        return true;
    }

    static TiledObject parseObject(tinyxml2::XMLElement *object, std::string groupName)
    {
        TiledObject obj = {};
        if (object->Attribute("name"))
            obj.name = object->Attribute("name");
        obj.objectGroup = groupName;
        obj.x = object->FloatAttribute("x");
        obj.y = object->FloatAttribute("y");
        obj.width = object->FloatAttribute("width");
        obj.height = object->FloatAttribute("height");
        tinyxml2::XMLElement *propertiesElem = object->FirstChildElement("properties");
        if (propertiesElem)
        {
            tinyxml2::XMLElement *property = propertiesElem->FirstChildElement("property");
            while (property)
            {

                parseProperties(property, obj);
                property = property->NextSiblingElement("property");
            }
        }

        return obj;
    }

    static bool parseProperties(tinyxml2::XMLElement *property, TiledObject &object)
    {
        object.properties[property->Attribute("name")] = property->Attribute("value");
        return true;
    }

    static SDL_Texture *loadTexture(const char *filepath, SDL_Renderer *renderer)
    {

        SDL_Surface *surface = IMG_Load(filepath);

        if (!surface)
        {
            std::cerr << "[Game] Failed to load image :" << filepath << "--" << IMG_GetError() << std::endl;
            return nullptr;
        }

        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

        if (!texture)
        {
            std::cerr << "[Game] failed to create Texture from Surface -" << SDL_GetError() << std::endl;
            return nullptr;
        }

        SDL_FreeSurface(surface);
        std::cout << "[Game] Texture load from :" << filepath << std::endl;
        return texture;
    }
};