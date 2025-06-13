#include "Shop.hpp"
#include "Engine/GameEngine.hpp"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <string>

const std::string filename = "Resource/images/play-scene/shop/shop.png";

const int shopwidth = 260;
const int shopheight = 200;

const int recw = 300;
const int rech = 150;

Shop::Shop(int x, int y) {
    this->x = x;
    this->y = y;
    bmp = al_load_bitmap(filename.c_str());

    font = al_load_font("Resource/fonts/imfell.ttf", 24, 0);
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
}