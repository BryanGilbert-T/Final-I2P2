#include "Shop.hpp"
#include "Engine/GameEngine.hpp"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <string>
#include <iostream>

const std::string filename = "Resource/images/play-scene/shop/shop-sheet.png";

const int shopwidth = 188;
const int shopheight = 200;

const int recw = 300;
const int rech = 150;

const int ANIM_FRAME_COUNT = 6;
const float ANIM_FRAME_RATE = 0.25f;

Shop::Shop(int x, int y) {
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

Shop::~Shop() {
    if (idle_sheet) {
        al_destroy_bitmap(idle_sheet);
    }

    if (font) {
        al_destroy_font(font);
    }
}


void Shop::Draw(Camera cam) {
    int dx = x - cam.x;
    int dy = y - cam.y;

    if (playerIsNear) {
        int screenW = Engine::GameEngine::GetInstance().getVirtW();
        int screenH = Engine::GameEngine::GetInstance().getVirtH();

        int dx = x - cam.x - (recw - shopwidth) / 2;
        int dy = y - cam.y - rech;

        al_draw_filled_rectangle(dx, dy, dx + recw, dy + rech, al_map_rgba_f(0, 0, 0, 0.7f));
        // border
        al_draw_rectangle(dx, dy, dx + recw, dy + rech, al_map_rgb(255,255,255), 2);

        std::string content = "Press F to enter";
        int texth = al_get_font_line_height(font);
        al_draw_text(font, al_map_rgb(255,255,255),
                     dx + recw/2, dy + rech/2 - texth, ALLEGRO_ALIGN_CENTER, content.c_str());
    }

    ALLEGRO_BITMAP* bmp = animation.frames[animation.current];

    al_draw_scaled_bitmap(bmp,
        0, 0, shopwidth, shopheight,
        dx, dy, shopwidth, shopheight,
        0);
}

void Shop::Update(float dt, const Player& player) {
    int px = player.x + 157 / 2;
    int py = player.y + 100 / 2;
    if (px >= x && px <= x + shopwidth &&
        py >= y && py <= y + shopheight) {
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