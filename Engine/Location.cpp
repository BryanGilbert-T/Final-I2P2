//
// Created by Arwen Rosetta Njoto on 13-Jun-25.
//
#include <Engine/map.hpp>
#include <allegro5/allegro.h>
#include <memory>

#include "Engine/Point.hpp"
#include "Engine/GameEngine.hpp"
#include "Sprite.hpp"
#include "UI/Component/Image.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Location.hpp"
#include <iostream>

using namespace Engine;

void Location::Initialize() {
    elapsedTime = 0.0;
    Locations = {
        al_load_bitmap("Resource/images/play-scene/ui/loc-mount-huaguo.png"),
        al_load_bitmap("Resource/images/play-scene/ui/loc-eastern.png"),
        al_load_bitmap("Resource/images/play-scene/ui/loc-moon.png")
        //, al_load_bitmap("Resource/images/play-scene/ui/loc-thunder.png")
    };
}


void Location::Update(float deltaTime) {
    elapsedTime += deltaTime;
}

void Location::Draw(int mapIdx) {
    int w = Engine::GameEngine::GetInstance().getVirtW();
    int h = Engine::GameEngine::GetInstance().getVirtH();
    int halfW = w / 2;

    const float delay = 1.0f;
    const float fade  = 1.0f;
    const float hold  = 3.0f;

    // compute the 4 phase boundaries:
    const float t0 = delay;           // end of delay
    const float t1 = t0 + fade;       // end of fade-in
    const float t2 = t1 + hold;       // end of hold
    const float t3 = t2 + fade;       // end of fade-out

    float alpha;
    if      (elapsedTime < t0)        alpha = 0.0f;                                        // still in delay
    else if (elapsedTime < t1)        alpha = (elapsedTime - t0) / fade;                   // fade-in (0→1)
    else if (elapsedTime < t2)        alpha = 1.0f;                                        // hold at full alpha
    else if (elapsedTime < t3)        alpha = 1.0f - (elapsedTime - t2) / fade;            // fade-out (1→0)
    else                              alpha = 0.0f;                                        // done

    // now draw using that alpha:
    ALLEGRO_BITMAP* bmp = Locations[mapIdx - 1];
    al_draw_tinted_scaled_bitmap(
        bmp,
        al_map_rgba_f(1, 1, 1, alpha),
        0, 0,
        al_get_bitmap_width(bmp),
        al_get_bitmap_height(bmp),
        halfW - al_get_bitmap_width(bmp)/2,
        h * 0.07f,
        al_get_bitmap_width(bmp),
        al_get_bitmap_height(bmp),
        0
    );
}

