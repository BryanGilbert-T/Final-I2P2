#include <Engine/map.hpp>
#include <allegro5/allegro.h>
#include <memory>

#include "Engine/Point.hpp"
#include "Engine/GameEngine.hpp"
#include "Sprite.hpp"
#include "UI/Component/Image.hpp"
#include "Engine/GameEngine.hpp"
#include <iostream>

using namespace Engine;

const int TILE_SIZE = 64;

void Map::Init(int width, int height, MapType state) {
    dirt_bitmap = al_load_bitmap("Resource/images/stage-select/dirt.png");
    if (!dirt_bitmap) {
        std::cerr << "Failed to load dirt." << std::endl;
    }
    sky_bitmap = al_load_bitmap("Resource/images/stage-select/floor.png");
    if (!sky_bitmap) {
        std::cerr << "Failed to load floor." << std::endl;
    }

    MapWidth = width;
    MapHeight = height;
    MapState = state;
}

Map::~Map() {
    al_destroy_bitmap(dirt_bitmap);
    al_destroy_bitmap(sky_bitmap);
}

void Map::DrawMap(Camera cam) {
    int w = Engine::GameEngine::GetInstance().getVirtW();
    int h = Engine::GameEngine::GetInstance().getVirtH();

    for (int i = 0; i < MapHeight; i++) {
        for (int j = 0; j < MapWidth; j++) {
            int dy = i * TILE_SIZE - cam.y;
            int dx = j * TILE_SIZE - cam.x;

            switch (MapState[i][j]) {
                case TILE_DIRT:
                    al_draw_scaled_bitmap(dirt_bitmap,
                        0, 0, 64, 64,
                        dx, dy, TILE_SIZE, TILE_SIZE, 0);
                    break;
                case TILE_SKY:
                    al_draw_scaled_bitmap(sky_bitmap,
                        0, 0, 64, 64,
                        dx, dy, TILE_SIZE, TILE_SIZE, 0);
                    break;
                default:
                    break;
            }
        }
    }
}

void Map::UpdateMap() {

}

void Map::get_map_offset() {

}
