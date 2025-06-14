#ifndef ITEM_H
#define ITEM_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include "Engine/utility.hpp"
#include "Player/Player.hpp"

enum ItemType {
    ITEM_APPLE,
    ITEM_BANANA,
};

class Item {
private:
    int x, y;
    int w, h;
    int dw, dh;
    ALLEGRO_BITMAP* idle_sheet;
    ALLEGRO_FONT* font;
    Animation animation;

public:
    bool playerIsNear;
    ItemType type;
    int price;

    Item(int x, int y, int w, int h, int dw, int dh, int price, std::string name, int anim_frame, float anim_rate, ItemType type);
    ~Item();
    void Draw(Camera cam);
    void Update(float dt, const Player& player);
};

#endif