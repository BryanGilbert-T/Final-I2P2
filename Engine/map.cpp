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
    const std::string base = "Resource/images/play-scene/";
    const char* paths[3][3] = {
        { "floor-top-left.png", "floor-top-mid.png", "floor-top-right.png"},
        { "floor-mid-left.png", "floor-mid-mid.png", "floor-mid-right.png"},
        { "floor-bot-left.png", "floor-bot-mid.png", "floor-bot-right.png"}
    };
    for(int r = 0; r < 3; ++r) {
        for(int c = 0; c < 3; ++c) {
            floor_bitmap[r][c] = al_load_bitmap((base + paths[r][c]).c_str());
            if (!floor_bitmap[r][c]) {
                std::cerr << "Failed to load " << paths[r][c] << std::endl;
            }
        }
    }

    MapWidth = width;
    MapHeight = height;
    MapState = state;

    offset_asset.assign(MapHeight, std::vector<int>(MapWidth, -1));
}

Map::~Map() {
    al_destroy_bitmap(dirt_bitmap);
    al_destroy_bitmap(sky_bitmap);
}

void Map::DrawMap(Camera cam) {
    get_map_offset();
    int w = Engine::GameEngine::GetInstance().getVirtW();
    int h = Engine::GameEngine::GetInstance().getVirtH();

    for (int i = 0; i < MapHeight; i++) {
        for (int j = 0; j < MapWidth; j++) {
            int dy = i * TILE_SIZE - cam.y;
            int dx = j * TILE_SIZE - cam.x;

            int sw = 0;
            int sh = 0;
            switch (MapState[i][j]) {
                case TILE_DIRT: {
                    ALLEGRO_BITMAP* center = floor_bitmap[1][1];
                    al_draw_scaled_bitmap(center,
                        0,0,
                        al_get_bitmap_width(center),
                        al_get_bitmap_height(center),
                        dx,dy,
                        TILE_SIZE,TILE_SIZE,
                        0
                    );

                    // 2) if we’re on an edge or corner, draw that on top
                    int idx = offset_asset[i][j];
                    if (idx != 4) {
                        int row = idx/3;
                        int col = idx%3;
                        ALLEGRO_BITMAP* variant = floor_bitmap[row][col];
                        al_draw_scaled_bitmap(variant,
                            0,0,
                            al_get_bitmap_width(variant),
                            al_get_bitmap_height(variant),
                            dx,dy,
                            TILE_SIZE,TILE_SIZE,
                            0
                        );
                    }
                    break;
                }
                case TILE_SKY:
                    // sw = al_get_bitmap_width(sky_bitmap);
                    // sh = al_get_bitmap_height(sky_bitmap);
                    //
                    // al_draw_scaled_bitmap(sky_bitmap,
                    //     0, 0, sw, sh,
                    //     dx, dy, TILE_SIZE, TILE_SIZE, 0);
                    break;
                default:
                    break;
            }
        }
    }
}

bool Map::IsCollision(int x, int y) {
    int dx = x / TILE_SIZE;
    int dy = y / TILE_SIZE;

    if (MapState[dy][dx] == TILE_DIRT) {
        std::cout << dy << " " << dx << std::endl;
        return true;
    }

    return false;
}

void Map::UpdateMap() {

}

void Map::get_map_offset() {
    for(int i = 0; i < MapHeight; ++i) {
        for(int j = 0; j < MapWidth; ++j) {

            if (MapState[i][j] != TILE_DIRT) {
                offset_asset[i][j] = -1;
                continue;
            }
            bool up    = (i>0            && MapState[i-1][j] == TILE_DIRT);
            bool down  = (i<MapHeight-1  && MapState[i+1][j] == TILE_DIRT);
            bool left  = (j>0            && MapState[i][j-1] == TILE_DIRT);
            bool right = (j<MapWidth-1   && MapState[i][j+1] == TILE_DIRT);

            // pick row by vertical neighbours:
            //   up&down → middle row (1)
            //   up only   → bottom row (2) so the top edge points up
            //   down only → top row    (0)
            int row;
            if (up && down)        row = 1;
            else if (up)           row = 2;
            else if (down)         row = 0;
            else                   row = 1;  // isolated floor

            // pick col by horizontal neighbours:
            //   left&right → middle col (1)
            //   left only   → right col  (2) so the left edge points left
            //   right only  → left col   (0)
            int col;
            if (left && right)     col = 1;
            else if (left)         col = 2;
            else if (right)        col = 0;
            else                   col = 1;

            // flatten 3×3 to 0..8
            offset_asset[i][j] = row * 3 + col;
        }
    }
}
