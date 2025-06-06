#ifndef MAP_HPP
#define MAP_HPP

#include <allegro5/allegro_audio.h>
#include <list>
#include <memory>
#include <utility>
#include <vector>

#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/utility.hpp"

using MapType = std::vector<std::vector<TileType>>;

namespace Engine {
    class Map{
    public:
        int MapWidth;
        int MapHeight;
        MapType MapState;
        std::vector<std::vector<int>> offset_asset;
        void Init(int width, int height, MapType state);
        bool IsCollision(int x, int y);
        Map() {
            MapWidth = 0;
            MapHeight = 0;
            MapState = MapType(0);
        }
        ~Map();
        ALLEGRO_BITMAP* dirt_bitmap;
        ALLEGRO_BITMAP* sky_bitmap;

        void DrawMap(Camera cam);
        void UpdateMap();
        void get_map_offset();
    };
}


#endif