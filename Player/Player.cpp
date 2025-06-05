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

const std::string filename = "Resource/images/player.png";

Player::Player(int hp, int x, int y, int speed) : Engine::Sprite(filename, x, y),
hp(hp), x(x), y(y), speed(speed), dir(RIGHT) {
    jump = 0;
}

void Player::move(int keyCode) {
    if (keyCode == ALLEGRO_KEY_W) {

    } else if (keyCode == ALLEGRO_KEY_A) {

    } else if (keyCode == ALLEGRO_KEY_S) {

    } else if (keyCode == ALLEGRO_KEY_D) {

    }
}

void Player::Draw() const {
    Sprite::Draw();
}


