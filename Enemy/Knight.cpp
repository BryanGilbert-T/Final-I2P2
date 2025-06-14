#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include <cmath>
#include <utility>

#include "Enemy/Enemy.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/AudioHelper.hpp"
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
const int SPEED = 2;
const int DAMAGE = 12;

const int IDLE_FRAME_COUNT = 10;
const double IDLE_FRAME_RATE = 0.1;

const int WALK_FRAME_COUNT = 10;
const double WALK_FRAME_RATE = 0.1;

const int ATTACK_FRAME_COUNT = 4;
const double ATTACK_FRAME_RATE = 0.275;

const int JUMP_FRAME_COUNT = 2;
const double JUMP_FRAME_RATE = 0.3;

const int DEAD_FRAME_COUNT = 10;
const double DEAD_FRAME_RATE = 0.1;

const int WIDTH = 120*2.5;
const int HEIGHT = 80*2.5;

const float attackCooldownMax = 2.0f;

const int xkurang = 90;
const int ykurang = 80;

const std::string filename = "Resource/images/character/idle-sheet.png";
const std::string idlefile = "Resource/images/character/knight/_Idle.png";
const std::string walkfile = "Resource/images/character/knight/_Run.png";
const std::string attackfile = "Resource/images/character/knight/_Attack.png";
const std::string deadfile = "Resource/images/character/knight/_DeathNoMovement.png";

KnightEnemy::KnightEnemy(int x, int y):
    Enemy(HP, x, y, SPEED, DAMAGE, WIDTH, HEIGHT),
    aiState(AIState::PATROL),
    patrolOriginX(static_cast<float>(x)),
    patrolDir(1),
    patrolRange(200.0f),    // e.g. ±200px from spawn
    chaseRadius(400.0f),    // e.g. start chasing if closer than 300px
    attackRadius(135.0f)   // optional melee range
{
    rng.seed(std::random_device{}());
    attackCooldown = 0.0f;
    hitPlayer = false;
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

    idle_sheet = al_load_bitmap(walkfile.c_str());
    if (!idle_sheet) {
        std::cerr << "Failed to load player_bitmap" << std::endl;
    }
    frameW = al_get_bitmap_width(idle_sheet) / WALK_FRAME_COUNT;
    frameH = al_get_bitmap_height(idle_sheet);
    Animation walkAnim(WALK_FRAME_RATE);
    for (int i = 0; i < WALK_FRAME_COUNT; ++i) {
        ALLEGRO_BITMAP* f = al_create_sub_bitmap(
            idle_sheet, i * frameW, 0, frameW, frameH
        );
        walkAnim.frames.push_back(f);
    }
    animations[WALK] = std::move(walkAnim);

    idle_sheet = al_load_bitmap(attackfile.c_str());
    if (!idle_sheet) {
        std::cerr << "Failed to load player_bitmap" << std::endl;
    }
    frameW = al_get_bitmap_width(idle_sheet) / ATTACK_FRAME_COUNT;
    frameH = al_get_bitmap_height(idle_sheet);
    Animation attackAnim(ATTACK_FRAME_RATE);
    for (int i = 0; i < ATTACK_FRAME_COUNT; ++i) {
        ALLEGRO_BITMAP* f = al_create_sub_bitmap(
            idle_sheet, i * frameW, 0, frameW, frameH
        );
        attackAnim.frames.push_back(f);
    }
    animations[ATTACK] = std::move(attackAnim);

    idle_sheet = al_load_bitmap(deadfile.c_str());
    if (!idle_sheet) {
        std::cerr << "Failed to load player_bitmap" << std::endl;
    }
    frameW = al_get_bitmap_width(idle_sheet) / DEAD_FRAME_COUNT;
    frameH = al_get_bitmap_height(idle_sheet);
    Animation deadAnim(DEAD_FRAME_RATE);
    for (int i = 0; i < DEAD_FRAME_COUNT; ++i) {
        ALLEGRO_BITMAP* f = al_create_sub_bitmap(
            idle_sheet, i * frameW, 0, frameW, frameH
        );
        deadAnim.frames.push_back(f);
    }
    animations[DEAD] = std::move(deadAnim);

    animations[JUMP] = animations[IDLE];

    PlaySound = true;
}

std::pair<float,float> KnightEnemy::getPlayerPos() const {
    auto scene = dynamic_cast<PlayScene*>(
        Engine::GameEngine::GetInstance().GetScene("play")
    );
    if (scene) {
        return { scene->player.x, scene->player.y };
    }
    return { 0, 0 };
}

void KnightEnemy::Draw(Camera cam) {
    Enemy::Draw(cam);
}

void KnightEnemy::performPatrol(float dt) {
    // Move horizontally
    auto scene = dynamic_cast<PlayScene*>(
        Engine::GameEngine::GetInstance().GetScene("play")
    );
    int dx = x + (patrolDir * speed);
    int dy = y;

    if (patrolDir == -1) {
        flag = 1;
    }
    if (patrolDir == 1) {
        flag = 0;
    }

    // Flip direction at range limits
    if (dx > patrolOriginX + patrolRange || dx < patrolOriginX - patrolRange) {
        patrolDir = -patrolDir;
    }

    const int x2 = dx + xkurang;
    const int y2 = dy + ykurang;
    const int w2 = ENEMY_WIDTH - xkurang - xkurang;
    const int h2 = ENEMY_HEIGHT - ykurang;

    if (x2 >= 0 && y2 >= 0 &&
        x2 + w2 - 1 < scene->MapWidth * scene->BlockSize && y2 + h2 - 1 < scene->MapHeight * scene->BlockSize &&
        !scene->map.IsCollision(x2, y2) && !scene->map.IsCollision(x2 + w2 - 1, y2 + h2 - 1) &&
        !scene->map.IsCollision(x2, y2 + h2 - 1) && !scene->map.IsCollision(x2 + w2 - 1, y2)) {
            x = dx;
        }
    else {
        patrolDir = -patrolDir;
    }

    // Choose idle or walk animation based on patrolDir
    setState(WALK);  // or a WALK state if you have one
}

void KnightEnemy::performChase(float dt, float dx, float dy, float dist) {
    // Normalize direction
    auto scene = dynamic_cast<PlayScene*>(
        Engine::GameEngine::GetInstance().GetScene("play")
    );
    if (flag == 1) {
        if (dist > 100) {
            float nx = dx / dist;
            float ny = dy / dist;

            if (dx < 0) flag = 1;
            if (dx > 0) flag = 0;

            int dx = x + (nx * speed * 2);
            int dy = y;

            const int x2 = dx + xkurang;
            const int y2 = dy + ykurang;
            const int w2 = ENEMY_WIDTH - xkurang - xkurang;
            const int h2 = ENEMY_HEIGHT - ykurang;

            if (x2 >= 0 && y2 >= 0 &&
                x2 + w2 - 1 < scene->MapWidth * scene->BlockSize && y2 + h2 - 1 < scene->MapHeight * scene->BlockSize &&
                !scene->map.IsCollision(x2, y2) && !scene->map.IsCollision(x2 + w2 - 1, y2 + h2 - 1) &&
                !scene->map.IsCollision(x2, y2 + h2 - 1) && !scene->map.IsCollision(x2 + w2 - 1, y2)) {
                x = dx;
            }
        }
    } else if (flag == 0) {
        if (dist > 210) {
            float nx = dx / dist;
            float ny = dy / dist;

            if (dx < 0) flag = 1;
            if (dx > 0) flag = 0;

            int dx = x + (nx * speed * 2);
            int dy = y;

            const int x2 = dx + xkurang;
            const int y2 = dy + ykurang;
            const int w2 = ENEMY_WIDTH - xkurang - xkurang;
            const int h2 = ENEMY_HEIGHT - ykurang;

            if (x2 >= 0 && y2 >= 0 &&
                x2 + w2 - 1 < scene->MapWidth * scene->BlockSize && y2 + h2 - 1 < scene->MapHeight * scene->BlockSize &&
                !scene->map.IsCollision(x2, y2) && !scene->map.IsCollision(x2 + w2 - 1, y2 + h2 - 1) &&
                !scene->map.IsCollision(x2, y2 + h2 - 1) && !scene->map.IsCollision(x2 + w2 - 1, y2)) {
                x = dx;
            }
        }
    }


    // You can trigger an attack when very close:
    if (flag == 1) {
        if (dist < attackRadius && attackCooldown <= 0.0f) {
            // e.g. launch projectile or deal melee damage
            // attackPlayer();
            setState(ATTACK); // placeholder for attack animation
            performAttack(dt, dist);

            attackCooldown = attackCooldownMax;
            animations[ATTACK].current = 0;
            animations[ATTACK].timer   = 0;
        } else {
            setState(WALK); // or chase‐walk animation
        }
    } else if (flag == 0) {
        if (dist < attackRadius + ENEMY_WIDTH && attackCooldown <= 0.0f) {
            // e.g. launch projectile or deal melee damage
            // attackPlayer();
            setState(ATTACK); // placeholder for attack animation
            performAttack(dt, dist);

            attackCooldown = attackCooldownMax;
            animations[ATTACK].current = 0;
            animations[ATTACK].timer   = 0;
        } else {
            setState(WALK); // or chase‐walk animation
        }
    }

}

void KnightEnemy::Update(float deltaTime) {
    if (state == DEAD) {
        Enemy::Update(deltaTime);
        return;
    }
    auto [px, py] = getPlayerPos();
    float dx   = px - x;
    float dy   = py - y;
    float dist = std::sqrt(dx*dx + dy*dy);


    if (dx <= 0) {
        flag = 1;
    } else {
        flag = 0;
    }
    attackCooldown = std::max(0.0f, attackCooldown - deltaTime);

    // Choose state
    if (state == ATTACK) {
        performAttack(deltaTime, dist);
    } else if (dist < chaseRadius || (dx > 0 && dist < chaseRadius + ENEMY_WIDTH)) {
        aiState = AIState::CHASE;
    } else {
        if (aiState == AIState::CHASE) {
            patrolOriginX = x;
        }
        aiState = AIState::PATROL;
    }

    // Act
    if (state == ATTACK) {

    } else if (aiState == AIState::PATROL) {
        performPatrol(deltaTime);
    } else {
        performChase(deltaTime, dx, dy, dist);
    }

    Enemy::Update(deltaTime);
}

void KnightEnemy::move(int keyCode) {
    // find the nearest path to player
}

KnightEnemy::~KnightEnemy() {
}

void KnightEnemy::performAttack(float dt, float dist) {
    Animation &anim = animations[ATTACK];
    if (PlaySound && anim.current == 1) {
        int id = whichSound(rng);
        AudioHelper::PlaySample("play/swing-" + std::to_string(id) + ".mp3", false, AudioHelper::SFXVolume, 0);
        PlaySound = false;
    }
    if (anim.current == 1 || anim.current == 2) {
        if (flag == 0 && hitPlayer == false) {
            auto scene = dynamic_cast<PlayScene*>(
                Engine::GameEngine::GetInstance().GetScene("play")
            );
            if (scene) {
                int dx = this->x + ENEMY_WIDTH;
                int dy = this->y + ENEMY_HEIGHT / 4 * 3;
                if (scene->player.CollideWith(dx, dy)) {
                    scene->player.Hit(damage, (flag == 1) ? -1 : 1);
                    hitPlayer = true;
                }
            }
        } else if (flag == 1 && hitPlayer == false) {
            auto scene = dynamic_cast<PlayScene*>(
                            Engine::GameEngine::GetInstance().GetScene("play")
                        );
            if (scene) {
                int dx = this->x + 20;
                int dy = this->y + ENEMY_HEIGHT / 4 * 3;
                if (scene->player.CollideWith(dx, dy)) {
                    scene->player.Hit(damage, (flag == 1) ? -1 : 1);
                    hitPlayer = true;
                }
            }
        }
    }
    if (anim.current >= anim.frames.size() - 1) {
        // reset to WALK once animation is done
        setState(WALK);
        hitPlayer = false;
        PlaySound = true;
    }
}

bool KnightEnemy::CollideWith(int x, int y) {
    const int x2 = this->x + xkurang;
    const int y2 = this->y + ykurang;
    const int w2 = ENEMY_WIDTH - xkurang - xkurang;
    const int h2 = ENEMY_HEIGHT - ykurang;
    if (x >= x2 && x <= x2 + w2 && y >= y2 && y <= y2 + h2) {
        return true;
    }
    return false;
}

