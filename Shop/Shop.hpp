#ifndef SHOP_H
#define SHOP_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include "Engine/utility.hpp"
#include "Player/Player.hpp"

class Shop {
private:
    int x;
    int y;
    ALLEGRO_BITMAP* idle_sheet;
    ALLEGRO_FONT* font;
    Animation animation;

public:
    bool playerIsNear;
    Shop(int x, int y);
    ~Shop();
    void Draw(Camera cam);
    void Update(float dt, const Player& player);
};

#endif