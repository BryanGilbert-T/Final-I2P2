#include "coin.hpp"
#include "Engine/GameEngine.hpp"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <string>
#include <iostream>

const std::string filename = "Resource/images/play-scene/shop/Apple.png";

const int coinwidth = 32;
const int coinheight = 32;

const int ANIM_FRAME_COUNT = 8;
const float ANIM_FRAME_RATE = 0.25f;

Coin::Coin(int x, int y) {
    this->x = x;
    this->y = y;

    idle_sheet = al_load_bitmap(filename.c_str());
    if (!idle_sheet) {
        std::cerr << "Failed to load player_bitmap(walk)" << std::endl;
    }
    int frameW = al_get_bitmap_width(idle_sheet)/ANIM_FRAME_COUNT;
    int frameH = al_get_bitmap_height(idle_sheet);
    Animation Anim(ANIM_FRAME_RATE);
    for (int i = 0; i < ANIM_FRAME_COUNT; ++i) {
        ALLEGRO_BITMAP* f = al_create_sub_bitmap(
            idle_sheet, i * frameW, 0, frameW, frameH
            );
        Anim.frames.push_back(f);
    }
    animation = std::move(Anim);

    font = al_load_font("Resource/fonts/imfell.ttf", 24, 0);
}

Coin::~Coin() {
    if (idle_sheet) {
        al_destroy_bitmap(idle_sheet);
    }

    if (font) {
        al_destroy_font(font);
    }
}


void Coin::Draw(Camera cam) {
    int dx = x - cam.x;
    int dy = y - cam.y;

    ALLEGRO_BITMAP* bmp = animation.frames[animation.current];

    al_draw_scaled_bitmap(bmp,
        0, 0, coinwidth, coinheight,
        dx, dy, 64, 64,
        0);
}

void Coin::Update(float dt, const Player& player) {
    const int offset = 10;
    int px = player.x + 157 / 2;
    int py = player.y + 100 / 2;
    if (px >= x - offset && px <= x + coinwidth + offset &&
        py >= y - coinheight && py <= y + coinheight) {
        playerIsNear = true;
    } else {
        playerIsNear = false;
    }

    auto &A = animation;
    A.timer += dt;
    if (A.timer >= A.frame_time) {
        A.timer -= A.frame_time;
        A.current = (A.current + 1) % A.frames.size();
    }
}