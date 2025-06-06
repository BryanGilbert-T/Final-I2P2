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
const float JUMP_ACCELERATION = 0.5;
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
    PlayScene *scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"));
    if (jump > 0) {
        int dy = this->y - jumpSpeed;
        int dx = x;

        jumpSpeed -= JUMP_ACCELERATION;

        if (dx >= 0 && dy >= 0 &&
            dx + PLAYER_SIZE - 1 < scene->MapWidth * scene->BlockSize && dy + PLAYER_SIZE - 1 < scene->MapHeight * scene->BlockSize &&
            !scene->map.IsCollision(dx, dy) && !scene->map.IsCollision(dx + PLAYER_SIZE - 1, dy + PLAYER_SIZE - 1) &&
            !scene->map.IsCollision(dx, dy + PLAYER_SIZE - 1) && !scene->map.IsCollision(dx + PLAYER_SIZE - 1, dy)) {
            x = dx;
            y = dy;
            } else {
                dy = this->y + 1;
                if (dx >= 0 && dy >= 0 &&
                dx + PLAYER_SIZE - 1 < scene->MapWidth * scene->BlockSize && dy + PLAYER_SIZE - 1 < scene->MapHeight * scene->BlockSize &&
                !scene->map.IsCollision(dx, dy) && !scene->map.IsCollision(dx + PLAYER_SIZE - 1, dy + PLAYER_SIZE - 1) &&
                !scene->map.IsCollision(dx, dy + PLAYER_SIZE - 1) && !scene->map.IsCollision(dx + PLAYER_SIZE - 1, dy)) {
                    x = dx;
                    y = dy;
                }
            }
    } else {
        int dy = this->y + GRAVITY;
        int dx = x;

        if (dx >= 0 && dy >= 0 &&
            dx + PLAYER_SIZE - 1 < scene->MapWidth * scene->BlockSize && dy + PLAYER_SIZE - 1 < scene->MapHeight * scene->BlockSize &&
            !scene->map.IsCollision(dx, dy) && !scene->map.IsCollision(dx + PLAYER_SIZE - 1, dy + PLAYER_SIZE - 1) &&
            !scene->map.IsCollision(dx, dy + PLAYER_SIZE - 1) && !scene->map.IsCollision(dx + PLAYER_SIZE - 1, dy)) {
            x = dx;
            y = dy;
            } else {
                dy = this->y + 1;
                if (dx >= 0 && dy >= 0 &&
                dx + PLAYER_SIZE - 1 < scene->MapWidth * scene->BlockSize && dy + PLAYER_SIZE - 1 < scene->MapHeight * scene->BlockSize &&
                !scene->map.IsCollision(dx, dy) && !scene->map.IsCollision(dx + PLAYER_SIZE - 1, dy + PLAYER_SIZE - 1) &&
                !scene->map.IsCollision(dx, dy + PLAYER_SIZE - 1) && !scene->map.IsCollision(dx + PLAYER_SIZE - 1, dy)) {
                    x = dx;
                    y = dy;
                }
            }
    }
}

Player::Player(){
    hp = 100;
    x = 0;
    y = 0;
    speed = SPEED;
    dir = RIGHT;
    jump = 0;
    jumpSpeed = 0;
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
    if (jump < 2 && jump != -1) {
        jump++;
        jumpSpeed = INITIAL_JUMP_SPEED;
    }
}

void Player::Draw(Camera cam){
    int dx = x - cam.x;
    int dy = y - cam.y;
    al_draw_tinted_scaled_bitmap(player_bitmap, al_map_rgb(255, 255, 255),
        0, 0, 32, 32,
        dx, dy, PLAYER_SIZE, PLAYER_SIZE, 0);
}