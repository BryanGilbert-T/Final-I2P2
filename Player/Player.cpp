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
hp(hp), x(x), y(y), speed(speed) {
    jump = 0;
}

void Player::move(Direction dir) {
    if (dir == LEFT) {

    } else if (dir == RIGHT) {

    } else if (dir == UP) {

    } else if (dir == DOWN) {

    }
}


