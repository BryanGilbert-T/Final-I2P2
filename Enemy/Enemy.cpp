#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include <cmath>
#include <utility>

#include "Enemy/Enemy.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/IObject.hpp"
#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"
#include "Scene/PlayScene.hpp"
#include "Enemy.hpp"
#include "Engine/map.hpp"

#include <iostream>
#include <ostream>

const int PLAYER_SIZE = 100;
const int SPEED = PLAYER_SIZE / 16;

const int GRAVITY = 8;
const float JUMP_ACCELERATION = 1;
const int INITIAL_JUMP_SPEED = 16;

const int IDLE_FRAME_COUNT = 2;
const double IDLE_FRAME_RATE = 0.3;

const int JUMP_FRAME_COUNT = 2;
const double JUMP_FRAME_RATE = 0.3;


void Enemy::setState(State s) {
    if (state != s) {
        state = s;
        auto &A = animations[state];
        A.current = 0;
        A.timer   = 0.0;
    }
}


void Enemy::Update(float deltaTime) {
    PlayScene *scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetScene("play"));
    if (!scene) return;

    // 1) Apply gravity (vy will grow by GRAVITY each frame):
    vy += JUMP_ACCELERATION;

    // 2) Figure out total vertical movement this frame:
    //    For example, if vy==−12, you want to move up 12 pixels; if vy==+8, move down 8.
    int totalDY   = vy;                      // could be negative (rising) or positive (falling)
    int signDY    = (totalDY >= 0 ? +1 : -1);
    int remaining = std::abs(totalDY);

    auto &A = animations[state];
    A.timer += deltaTime;
    if (A.timer >= A.frame_time) {
        A.timer -= A.frame_time;
        A.current = (A.current + 1) % A.frames.size();
    }

    // 4) We do not change x here—that only happens when move(keyCode) is called.

    // 5) Clamp inside world bounds (so you can’t fall off the map):
    int mapPixelW = scene->MapWidth * scene->BlockSize;
    int mapPixelH = scene->MapHeight * scene->BlockSize;
    if (x < 0)                           x = 0;
    if (x + PLAYER_SIZE > mapPixelW)     x = mapPixelW - PLAYER_SIZE;
    if (y < 0)                           y = 0;
    if (y + PLAYER_SIZE > mapPixelH)     y = mapPixelH - PLAYER_SIZE;
}

Enemy::Enemy(int hp, int x, int y, int speed, int damage){
    this->damage = damage;
    this->hp = hp;
    this->x = x;
    this->y = y;
    this->speed = speed;
    this->dir = RIGHT;
    this->jump = 0;
    state = IDLE;
}

Enemy::~Enemy() {
    al_destroy_bitmap(idle_sheet);

    // then free each frame
    for (auto &kv : animations) {
        for (auto* f : kv.second.frames) {
            al_destroy_bitmap(f);
        }
    }
}

void Enemy::move(int keyCode) {
    PlayScene *scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"));
    int dx = x;
    int dy = y;

    // move it

    if (dx >= 0 && dy >= 0 &&
        dx + PLAYER_SIZE - 1 < scene->MapWidth * scene->BlockSize && dy + PLAYER_SIZE - 1 < scene->MapHeight * scene->BlockSize &&
        !scene->map.IsCollision(dx, dy) && !scene->map.IsCollision(dx + PLAYER_SIZE - 1, dy + PLAYER_SIZE - 1) &&
        !scene->map.IsCollision(dx, dy + PLAYER_SIZE - 1) && !scene->map.IsCollision(dx + PLAYER_SIZE - 1, dy)) {
        x = dx;
        y = dy;
    }
}

void Enemy::Jump() {
    if (jump < 1) {
        jump++;
        vy = -INITIAL_JUMP_SPEED;
    }
}

void Enemy::Draw(Camera cam){
    int dx = x - cam.x;
    int dy = y - cam.y;
    auto &A = animations[state];
    ALLEGRO_BITMAP* bmp = A.frames[A.current];
    al_draw_scaled_bitmap(
        bmp,
        0, 0,
        al_get_bitmap_width(bmp), al_get_bitmap_height(bmp),
        dx, dy,
        PLAYER_SIZE, PLAYER_SIZE,
        flag
    );

}