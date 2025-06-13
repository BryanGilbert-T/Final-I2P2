#ifndef ITEM_H
#define ITEM_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include "Engine/utility.hpp"
#include "Player/Player.hpp"

class Item {
private:
    int x, y;
    int w, h;
    int dw, dh;
    ALLEGRO_BITMAP* idle_sheet;
    ALLEGRO_FONT* font;
    Animation animation;
    int price;

public:
    bool playerIsNear;
    Item(int x, int y, int w, int h, int dw, int dh, int price, std::string name, int anim_frame, float anim_rate);
    ~Item();
    void Draw(Camera cam);
    void Update(float dt, const Player& player);
};

#endif