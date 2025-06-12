#include "Shop.hpp"
#include <string>

const std::string filename;

const int shopwidth = 200;
const int shopheight = 200;

Shop::Shop(int x, int y) {
    this->x = x;
    this->y = y;
    bmp = al_load_bitmap(filename.c_str());
}

void Shop::Draw(Camera cam) {

}

void Shop::Update(float dt) {

}