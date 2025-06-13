#ifndef UTILITY_HPP
#define UTILITY_HPP

static int MapBefore = 1;

class Camera {
public:
    float x;
    float y;
    float width;
    float height;

    Camera(float x, float y, float width, float height) : x(x), y(y), width(width), height(height) {
    }

    Camera() {
        x = 0;
        y = 0;
        width = 800;
        height = 800;
    }

    void Update(float x, float y) {
        this->x = x;
        this->y = y;
    }
};
enum TileType {
    TILE_DIRT,
    TILE_FLOOR,
    TILE_OCCUPIED,
    TILE_SKY,
    SHOP_DIRT,
    SHOP_SKY,
};

#endif