#include "TileMapRenderSystem.h"
#include "../Components/TileMapComponent.h"
#include "../Components/CameraComponent.h"
#include <SDL2/SDL.h>
#include <iostream>

TileMapRenderSystem::TileMapRenderSystem(int renderOrder)
    : targetRenderOrder(renderOrder)
{
    requireComponent<TileMapComponent>();
}

void TileMapRenderSystem::render(SDL_Renderer *renderer)
{
    if (!camera)
    {
        std::cout << "[TileMapRenderSystem] no cam set";
        return;
    }

    for (auto &entity : getEntities())
    {
        auto &tilemap = entity->getComponent<TileMapComponent>();

        for (auto &layer : tilemap.layers)
        {
            if (layer.renderOrder == targetRenderOrder)
            {

                drawLayer(tilemap, &layer, renderer);
            }
        }
    }
}

void TileMapRenderSystem::drawLayer(TileMapComponent &tilemap, const Layer *layer, SDL_Renderer *renderer)
{
    if (camera == nullptr)
        return;

    int scaledTileWidth = static_cast<int>(tilemap.tileWidth * camera->zoom);
    int scaledTileHeight = static_cast<int>(tilemap.tileHeight * camera->zoom);

    int startCol = static_cast<int>(camera->position.x / tilemap.tileWidth);
    int startRow = static_cast<int>(camera->position.y / tilemap.tileHeight);
    int endCol = static_cast<int>((camera->position.x + camera->viewportWidth / camera->zoom) / tilemap.tileWidth) + 1;
    int endRow = static_cast<int>((camera->position.y + camera->viewportHeight / camera->zoom) / tilemap.tileHeight) + 1;

    startCol = startCol < 0 ? 0 : startCol;
    endCol = endCol > layer->width ? layer->width : endCol;
    startRow = startRow < 0 ? 0 : startRow;
    endRow = endRow > layer->height ? layer->height : endRow;

    for (int row = startRow; row < endRow; row++)
    {
        for (int col = startCol; col < endCol; col++)
        {
            int index = row * layer->width + col;
            int gid = layer->tiles[index];
            if (gid == 0)
                continue;
            TileSet *tileset = tilemap.getTilesetFromGID(gid);
            if (!tileset)
                continue;

            SDL_Rect srcRect = tileset->getTileRect(gid - tileset->firstGID);

            SDL_Rect destRect;
            float worldX = col * tilemap.tileWidth;
            float worldY = row * tilemap.tileHeight;

            destRect.x = static_cast<int>((worldX - camera->position.x) * camera->zoom);
            destRect.y = static_cast<int>((worldY - camera->position.y) * camera->zoom);
            destRect.w = scaledTileWidth;
            destRect.h = scaledTileHeight;

            SDL_RenderCopy(renderer, tileset->texture, &srcRect, &destRect);
        }
    }
}
