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

#include <iostream>
#include <ostream>

const std::string filename = "Resource/images/stage-select/player.png";
const int SPEED = 4;
const int PLAYER_SIZE = 64;

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

Player::Player(){
    hp = 100;
    x = 0;
    y = 0;
    speed = SPEED;
    dir = RIGHT;
    jump = 0;
}

Player::~Player() {
    al_destroy_bitmap(player_bitmap);
}

void Player::move(int keyCode) {
    if (keyCode == ALLEGRO_KEY_W) {
        this->y -= this->speed;
    } else if (keyCode == ALLEGRO_KEY_A) {
        this->x -= this->speed;
    } else if (keyCode == ALLEGRO_KEY_S) {
        this->y += this->speed;
    } else if (keyCode == ALLEGRO_KEY_D) {
        this->x += this->speed;
    }
}

void Player::Draw(Camera cam){
    int dx = x - cam.x;
    int dy = y - cam.y;
    al_draw_tinted_scaled_bitmap(player_bitmap, al_map_rgb(255, 255, 255),
        0, 0, 32, 32,
        x, y, PLAYER_SIZE, PLAYER_SIZE, 0);
}