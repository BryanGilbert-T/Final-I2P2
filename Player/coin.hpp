#ifndef COIN_H
#define COIN_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include "Engine/utility.hpp"
#include "Player/Player.hpp"

class Coin {
private:
    int x;
    int y;
    ALLEGRO_BITMAP* idle_sheet;
    ALLEGRO_FONT* font;
    Animation animation;

public:
    bool playerIsNear;
    Coin(int x, int y);
    ~Coin();
    void Draw(Camera cam);
    void Update(float dt, const Player& player);
};

#endif