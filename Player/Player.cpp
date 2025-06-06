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

const std::string filename = "stage-select/player.png";
const int SPEED = 4;

void Player::Create(int hp, int x, int y){
    this->hp = hp;
    this->x = x;
    this->y = y;
    this->speed = SPEED;
    this->dir = RIGHT;
    this->jump = 0;
}

Player::Player(){
    hp = 100;
    x = 0;
    y = 0;
    speed = SPEED;
    dir = RIGHT;
    jump = 0;
}


void Player::move(int keyCode) {
    if (keyCode == ALLEGRO_KEY_W) {

    } else if (keyCode == ALLEGRO_KEY_A) {

    } else if (keyCode == ALLEGRO_KEY_S) {

    } else if (keyCode == ALLEGRO_KEY_D) {

    }
}

void Player::Draw(){
}


