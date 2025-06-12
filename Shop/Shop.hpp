#ifndef SHOP_H
#define SHOP_H

#include <allegro5/allegro.h>
#include "Engine/utility.hpp"

class Shop {
private:
    int x;
    int y;
    ALLEGRO_BITMAP* bmp;

public:
    Shop(int x, int y);
    void Draw(Camera cam);
    void Update(float dt);
};

#endif