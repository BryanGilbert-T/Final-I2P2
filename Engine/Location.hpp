//
// Created by Arwen Rosetta Njoto on 13-Jun-25.
//

#ifndef LOCATION_HPP
#define LOCATION_HPP


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
    class Location{
    public:
        float elapsedTime;
        int mapIdx;
        std::vector<ALLEGRO_BITMAP*> Locations;
        void Initialize();
        void Draw(int mapIdx);
        void Update(float deltaTime);

    };
}


#endif //LOCATION_HPP
