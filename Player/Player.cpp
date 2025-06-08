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
#include "Player.hpp"
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


void Player::Create(int hp, int x, int y){
    flag = 0;
    idle_sheet = al_load_bitmap("Resource/images/character/idle-sheet.png");
    if (!idle_sheet) {
        std::cerr << "Failed to load player_bitmap" << std::endl;
    }
    int frameW = al_get_bitmap_width(idle_sheet) / IDLE_FRAME_COUNT;
    int frameH = al_get_bitmap_height(idle_sheet);
    Animation idleAnim(IDLE_FRAME_RATE);
    for (int i = 0; i < IDLE_FRAME_COUNT; ++i) {
        ALLEGRO_BITMAP* f = al_create_sub_bitmap(
            idle_sheet, i * frameW, 0, frameW, frameH
        );
        idleAnim.frames.push_back(f);
    }
    animations[IDLE] = std::move(idleAnim);

    animations[JUMP] = animations[IDLE];

    this->hp = hp;
    this->x = x;
    this->y = y;
    this->speed = SPEED;
    this->dir = RIGHT;
    this->jump = 0;
    state = IDLE;

}

void Player::setState(State s) {
    if (state != s) {
        state = s;
        auto &A = animations[state];
        A.current = 0;
        A.timer   = 0.0;
    }
}


void Player::Update(float deltaTime) {
   PlayScene *scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetScene("play"));
    if (!scene) return;

    // 1) Apply gravity (vy will grow by GRAVITY each frame):
    vy += JUMP_ACCELERATION;

    // 2) Figure out total vertical movement this frame:
    //    For example, if vy==−12, you want to move up 12 pixels; if vy==+8, move down 8.
    int totalDY   = vy;                      // could be negative (rising) or positive (falling)
    int signDY    = (totalDY >= 0 ? +1 : -1);
    int remaining = std::abs(totalDY);

    // 3) Move one pixel at a time in the direction of signDY:
    //    This loop guarantees you never “tunnel” through a tile.
    while (remaining > 0) {
        // test a one-pixel step
        int testY = y + signDY;
        int leftX = x;
        int rightX = x + PLAYER_SIZE - 1;
        bool collided = false;

        if (signDY > 0) {
            // moving down: check the two bottom corners
            // world-coords = (leftX, testY+PLAYER_SIZE-1) and (rightX, testY+PLAYER_SIZE-1)
            if (scene->map.IsCollision(leftX,           testY + PLAYER_SIZE - 1) ||
                scene->map.IsCollision(rightX,          testY + PLAYER_SIZE - 1))
            {
                // We’ve hit the ground. Land here:
                vy = 0;
                jump = 0;     // reset jumpCount so we can jump again next time
                collided = true;
            }
        }
        else {
            // moving up: check the two top corners
            // world-coords = (leftX, testY) and (rightX, testY)
            if (scene->map.IsCollision(leftX,           testY) ||
                scene->map.IsCollision(rightX,          testY))
            {
                // We’ve hit a ceiling. Stop upward momentum:
                vy = 0;
                collided = true;
            }
        }

        if (collided) {
            break;  // stop any further vertical movement this frame
        } else {
            // safe to move that one pixel
            y = testY;
            remaining--;
        }
    }
    if (jump > 0 && vy < 0)        setState(JUMP);
    else if (jump > 0 && vy > 0)   setState(JUMP);  // falling could be separate
    else if (state != IDLE)        setState(IDLE);

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

Player::Player(){
    hp = 100;
    x = 0;
    y = 0;
    speed = SPEED;
    dir = RIGHT;
    jump = 0;
    vy = 0;
}

Player::~Player() {
    al_destroy_bitmap(idle_sheet);

    // then free each frame
    for (auto &kv : animations) {
        for (auto* f : kv.second.frames) {
            al_destroy_bitmap(f);
        }
    }
}

void Player::move(int keyCode) {
    PlayScene *scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"));
    int dx = x;
    int dy = y;
    if (keyCode == ALLEGRO_KEY_A) {
        dx -= this->speed;
        flag = 1;
    } else if (keyCode == ALLEGRO_KEY_S) {
        dy += this->speed;
    } else if (keyCode == ALLEGRO_KEY_D) {
        dx += this->speed;
        flag = 0;
    }
    if (dx >= 0 && dy >= 0 &&
        dx + PLAYER_SIZE - 1 < scene->MapWidth * scene->BlockSize && dy + PLAYER_SIZE - 1 < scene->MapHeight * scene->BlockSize &&
        !scene->map.IsCollision(dx, dy) && !scene->map.IsCollision(dx + PLAYER_SIZE - 1, dy + PLAYER_SIZE - 1) &&
        !scene->map.IsCollision(dx, dy + PLAYER_SIZE - 1) && !scene->map.IsCollision(dx + PLAYER_SIZE - 1, dy)) {
        x = dx;
        y = dy;
    }
}

void Player::Jump() {
    if (jump < 2) {
        jump++;
        vy = -INITIAL_JUMP_SPEED;
    }
}

void Player::Draw(Camera cam){
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