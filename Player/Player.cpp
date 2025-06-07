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

const std::string filename = "Resource/images/stage-select/player.png";
const int PLAYER_SIZE = 64;
const int SPEED = PLAYER_SIZE / 4;

const int GRAVITY = 8;
const float JUMP_ACCELERATION = 1;
const int INITIAL_JUMP_SPEED = 16;

void Player::Create(int hp, int x, int y){
    this->hp = hp;
    this->x = x;
    this->y = y;
    this->speed = SPEED;
    this->dir = RIGHT;
    this->jump = 0;
    player_bitmap = al_load_bitmap(filename.c_str());
    if (!player_bitmap) {
        std::cerr << "Failed to load player_bitmap" << std::endl;
    }
}

void Player::Update() {
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
    al_destroy_bitmap(player_bitmap);
}

void Player::move(int keyCode) {
    PlayScene *scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"));
    int dx = x;
    int dy = y;
    if (keyCode == ALLEGRO_KEY_A) {
        dx -= this->speed;
    } else if (keyCode == ALLEGRO_KEY_S) {
        dy += this->speed;
    } else if (keyCode == ALLEGRO_KEY_D) {
        dx += this->speed;
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
    al_draw_tinted_scaled_bitmap(player_bitmap, al_map_rgb(255, 255, 255),
        0, 0, 32, 32,
        dx, dy, PLAYER_SIZE, PLAYER_SIZE, 0);
}