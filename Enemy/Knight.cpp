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
#include "Knight.hpp"
#include "Engine/map.hpp"

#include <iostream>
#include <ostream>

const int HP = 100;
const int SPEED = 5;
const int DAMAGE = 12;

const int IDLE_FRAME_COUNT = 10;
const double IDLE_FRAME_RATE = 0.1;

const int JUMP_FRAME_COUNT = 2;
const double JUMP_FRAME_RATE = 0.3;

const int WIDTH = 120*2.5;
const int HEIGHT = 80*2.5;

const std::string filename = "Resource/images/character/idle-sheet.png";
const std::string idlefile = "Resource/images/character/knight/_Idle.png";

KnightEnemy::KnightEnemy(int x, int y) : Enemy(HP, x, y, SPEED, DAMAGE, WIDTH, HEIGHT){
    flag = 0;
    idle_sheet = al_load_bitmap(idlefile.c_str());
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
}

void KnightEnemy::Draw(Camera cam) {
    Enemy::Draw(cam);
}

void KnightEnemy::Update(float deltaTime) {
    Enemy::Update(deltaTime);
}

void KnightEnemy::move(int keyCode) {

}

KnightEnemy::~KnightEnemy() {

}

