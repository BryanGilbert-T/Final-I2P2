#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include <cmath>
#include <utility>
#include <algorithm>

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

const int PLAYER_WIDTH  = 157;
const int PLAYER_HEIGHT = 100;
const int SPEED = PLAYER_HEIGHT / 16;

const int ATTACK_RADIUS = 100;

const int GRAVITY = 8;
const float JUMP_ACCELERATION = 1;
const int INITIAL_JUMP_SPEED = 16;

const int IDLE_FRAME_COUNT = 2;
const double IDLE_FRAME_RATE = 0.3;

const int ATTACK_FRAME_COUNT = 4;
const double ATTACK_FRAME_RATE = 0.15;

const int JUMP_FRAME_COUNT = 6;
const double JUMP_FRAME_RATE = 0.089;  // ~11 FPS

const int WALK_FRAME_COUNT = 6;
const double WALK_FRAME_RATE = 0.175;

const int RUN_FRAME_COUNT = 6;
const double RUN_FRAME_RATE = 0.1;

const int DEAD_FRAME_COUNT = 6;
const double DEAD_FRAME_RATE = 0.1;

const float ATTACK_COOLDOWN_MAX = 1.0f;

const int xkurang = 35;
const int ykurang = 15;


void Player::Create(int hp, int x, int y, std::string name){
    username = name;
    flag = 0;
    isMoving = false;
    isRunning = false;
    isDying = false;
    died = false;
    idle_sheet = al_load_bitmap("Resource/images/character/idle-sheet.png");
    if (!idle_sheet) {
        std::cerr << "Failed to load player_bitmap(idle-sheet.png)" << std::endl;
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

    idle_sheet = al_load_bitmap("Resource/images/character/attack-sheet.png");
    if (!idle_sheet) {
        std::cerr << "Failed to load player_bitmap(atk)" << std::endl;
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

    idle_sheet = al_load_bitmap("Resource/images/character/walk-sheet.png");
    if (!idle_sheet) {
        std::cerr << "Failed to load player_bitmap(walk)" << std::endl;
    }
    frameW = al_get_bitmap_width(idle_sheet)/WALK_FRAME_COUNT;
    frameH = al_get_bitmap_height(idle_sheet);
    Animation walkAnim(WALK_FRAME_RATE);
    for (int i = 0; i < WALK_FRAME_COUNT; ++i) {
        ALLEGRO_BITMAP* f = al_create_sub_bitmap(
            idle_sheet, i * frameW, 0, frameW, frameH
            );
        walkAnim.frames.push_back(f);
    }
    animations[WALK] = std::move(walkAnim);

    idle_sheet = al_load_bitmap("Resource/images/character/walk-sheet.png");
    if (!idle_sheet) {
        std::cerr << "Failed to load player_bitmap(walk)" << std::endl;
    }
    frameW = al_get_bitmap_width(idle_sheet)/WALK_FRAME_COUNT;
    frameH = al_get_bitmap_height(idle_sheet);
    Animation runAnim(RUN_FRAME_RATE);
    for (int i = 0; i < RUN_FRAME_COUNT; ++i) {
        ALLEGRO_BITMAP* f = al_create_sub_bitmap(
            idle_sheet, i * frameW, 0, frameW, frameH
            );
        runAnim.frames.push_back(f);
    }
    animations[RUN] = std::move(runAnim);

    idle_sheet = al_load_bitmap("Resource/images/character/jump-sheet.png");
    if (!idle_sheet) {
        std::cerr << "Failed to load player_bitmap(jump)" << std::endl;
    }
    frameW = al_get_bitmap_width(idle_sheet)/4;
    frameH = al_get_bitmap_height(idle_sheet);
    std::vector<ALLEGRO_BITMAP*> base;
    for (int i = 0; i < 4; i++) {
        base.push_back(al_create_sub_bitmap(
            idle_sheet, i * frameW, 0, frameW, frameH
        ));
    }
    Animation jumpAnim(JUMP_FRAME_RATE);
    int sequence[6] = { 0, 1, 1, 2, 2, 3 };
    for (int k = 0; k < 6; k++) {
        jumpAnim.frames.push_back(base[ sequence[k] ]);
    }
    animations[JUMP] = std::move(jumpAnim);

    idle_sheet = al_load_bitmap("Resource/images/character/dying-sheet.png");
    if (!idle_sheet) {
        std::cerr << "Failed to load player_bitmap(dying)" << std::endl;
    }
    frameW = al_get_bitmap_width(idle_sheet)/DEAD_FRAME_COUNT;
    frameH = al_get_bitmap_height(idle_sheet);
    Animation dyingAnim(DEAD_FRAME_RATE);
    for (int i = 0; i < DEAD_FRAME_COUNT; ++i) {
        ALLEGRO_BITMAP* f = al_create_sub_bitmap(
            idle_sheet, i * frameW, 0, frameW, frameH);
        dyingAnim.frames.push_back(f);
    }
    animations[DEAD] = std::move(dyingAnim);

    //STAMINA
    staminaBg = al_load_bitmap("Resource/images/play-scene/ui/stamina-bg.png");
    staminaValue = al_load_bitmap("Resource/images/play-scene/ui/stamina-value.png");

    this->hp = hp;
    this->x = x;
    this->y = y;
    this->speed = SPEED;
    this->dir = RIGHT;
    this->jump = 0;
    state = IDLE;
    attacked = false;
}

void Player::Hit(int damage, int flag) {
    hp -= damage;
    if (hp <= 0) {
        hp = 0;
        setState(DEAD);
        isDying    = true;
        died   = false;      // we’ll flip this when the animation completes
        animations[DEAD].current = 0;
        animations[DEAD].timer   = 0;
        return;
    }
    // start the red flash + knockback
    isHit = true;
    hitTimer = 0.2f;  // flash for 0.2 seconds
    knockbackRemaining = KNOCKBACK_DISTANCE;
    // push away from the facing direction
    knockbackDir = flag;
    std::cout << "Player hp left: " << hp << std::endl;
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

    if (isDying && state == DEAD) {
        auto &A = animations[DEAD];
        A.timer += deltaTime;
        if (A.timer >= A.frame_time) {
            A.timer -= A.frame_time;
            A.current++;
            // once we hit the last frame, mark done:
            if (A.current >= A.frames.size()) {
                A.current = A.frames.size() - 1;  // stay on final pose
                died = true;
                isDying = false;
            }
        }
        return;  // don’t do any other movement/physics
    }

    if (hp == 0 && died) {
        return;
    }

    attackCooldown = std::max(0.0f, attackCooldown - deltaTime);
    sprintCooldownTimer = std::max(0.0f, sprintCooldownTimer - deltaTime);

    bool wantSprint = rightHeld && stamina>0 && sprintCooldownTimer==0.0f;

    bool wantsVisible = (stamina < maxStamina);
    // fade alpha up or down toward 0/1
    if (wantsVisible) {
        staminaBarAlpha = std::min(1.0f, staminaBarAlpha + STAMINA_BAR_FADE_SPEED * deltaTime);
    } else {
        staminaBarAlpha = std::max(0.0f, staminaBarAlpha - STAMINA_BAR_FADE_SPEED * deltaTime);
    }

    if (wantSprint && state != RUN) {
        isRunning = true;
    }
    if (dashTimer > 0.0f) {
        dashTimer -= deltaTime;
    }
    else if (wantSprint) {
        isRunning = true;
        stamina = std::max(0.0f, stamina - sprintDrain * deltaTime);
        if (stamina == 0.0f) sprintCooldownTimer = sprintCooldownDuration;
    }
    else {
        if (isRunning) {
            isRunning = false;
            // pick new state once, too:
            if (isMoving) setState(WALK);
            else          setState(IDLE);
        }
        stamina = std::min(maxStamina, stamina + regenRate * deltaTime);
    }
    if (state == RUN)       speed = SPEED * 1.5;
    else if (state == WALK) speed = SPEED;

    if (state == ATTACK) {
        Animation &anim = animations[ATTACK];
        if (anim.current == 1 || anim.current == 2) {
            for (Enemy* e : scene->enemyGroup) {
                if (attacked == false) {
                    if (dir == RIGHT) {
                        int dx = this->x + PLAYER_WIDTH;
                        int dy = this->y + PLAYER_HEIGHT / 2;
                        if (e->CollideWith(dx, dy)) {
                            e->Hit(10, (flag == 1) ? -1 : 1);
                            attacked = true;
                            return;
                        }
                    } else if (dir == LEFT) {
                        int dx = this->x;
                        int dy = this->y + PLAYER_HEIGHT / 2;
                        if (e->CollideWith(dx, dy)) {
                            e->Hit(10, (flag == 1) ? -1 : 1);
                            attacked = true;
                            return;
                        }
                    }

                }
            }
        }
        if (anim.current >= anim.frames.size() - 1) {
            // reset to WALK once animation is done
            attacked = false;
            setState(IDLE);
        }
    }
    const int w2 = PLAYER_WIDTH - xkurang - xkurang;
    const int h2 = PLAYER_HEIGHT - ykurang;
    if (knockbackRemaining > 0) {
        int step = std::min(knockbackRemaining, speed);
        int dx = x + knockbackDir * step;
        int dy = y;
        knockbackRemaining -= step;
        // (optional) you can check collisions here if you don’t want them shoved into walls
        const int x2 = dx + xkurang;
        const int y2 = dy + ykurang;

        if (x2 >= 0 && y2 >= 0 &&
            x2 + w2 - 1 < scene->MapWidth * scene->BlockSize && y2 + h2 - 1 < scene->MapHeight * scene->BlockSize &&
            !scene->map.IsCollision(x2, y2) && !scene->map.IsCollision(x2 + w2 - 1, y2 + h2 - 1) &&
            !scene->map.IsCollision(x2, y2 + h2 - 1) && !scene->map.IsCollision(x2 + w2 - 1, y2)) {
            x = dx;
            y = dy;
        }
    }
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
        int leftX = x + xkurang;
        int rightX = leftX + w2 - 1;
        bool collided = false;

        if (signDY > 0) {
            // moving down: check the two bottom corners
            // world-coords = (leftX, testY+PLAYER_SIZE-1) and (rightX, testY+PLAYER_SIZE-1)
            if (scene->map.IsCollision(leftX,           testY + PLAYER_HEIGHT - 1) ||
                scene->map.IsCollision(rightX,          testY + PLAYER_HEIGHT - 1) ||
                scene->map.IsCollision(leftX + PLAYER_WIDTH / 2, testY + PLAYER_HEIGHT - 1))
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
                scene->map.IsCollision(rightX,          testY) ||
                scene->map.IsCollision(leftX + PLAYER_WIDTH / 2, testY))
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
    if (state != ATTACK) {
        if (jump > 0 && vy < 0)        setState(JUMP);
        else if (jump > 0 && vy > 0)   setState(JUMP);  // falling could be separate
        else if (isRunning && isMoving) {setState(RUN);}
        else if (isMoving) setState(WALK);
        else  setState(IDLE);
    }
    auto &A = animations[state];
    A.timer += deltaTime;
    if (A.timer >= A.frame_time) {
        A.timer -= A.frame_time;
        A.current = (A.current + 1) % A.frames.size();
    }

    // 4) We do not change x here—that only happens when move(keyCode) is called.
    if (isHit) {
        hitTimer -= deltaTime;
        if (hitTimer <= 0.0f) {
            isHit = false;
            hitTimer = 0.0f;
        }
    }

    // 5) Clamp inside world bounds (so you can’t fall off the map):
    int mapPixelW = scene->MapWidth * scene->BlockSize;
    int mapPixelH = scene->MapHeight * scene->BlockSize;
    const int x2 = x + xkurang;
    const int y2 = y + ykurang;
    if (x2 < 0)                           x = 0;
    if (x2 + w2 > mapPixelW)     x = mapPixelW - w2;
    if (y < 0)                           y = 0;
    if (y + PLAYER_HEIGHT > mapPixelH)     y = mapPixelH - PLAYER_HEIGHT;
}

void Player::DrawStamina() {
    if (staminaBarAlpha <= 0.01f) return;

    bool inCooldown = sprintCooldownTimer > 0.0f;
    //if (!isRunning && !inCooldown) return;

    int w = Engine::GameEngine::GetInstance().getVirtW();
    int h = Engine::GameEngine::GetInstance().getVirtH();
    int halfH = h / 2;

    int barW = al_get_bitmap_width(staminaBg);
    int barH = al_get_bitmap_height(staminaBg);
    int barX = w * 0.95f;
    int barY = halfH - barH / 2;

    int fillH = static_cast<int>(barH * (stamina / maxStamina));
    fillH = std::clamp(fillH, 0, barH);
    ALLEGRO_COLOR tint = al_map_rgba_f(1, 1, 1, staminaBarAlpha);

    if (fillH > 0) {
        int srcY = barH - fillH;
        int dstY = barY + (barH - fillH);

        al_draw_tinted_scaled_bitmap(
            staminaValue, tint,
            0, srcY,           // only the bottom `fillH` pixels of the source
            barW, fillH,
            barX + 7,         // +15 to inset it a bit inside the BG
            dstY + 3,
            barW, fillH,
            0
        );
    }
    al_draw_tinted_scaled_bitmap(
        staminaBg, tint,
        0, 0,
        barW, barH,
        barX, barY,
        barW, barH,
        0
    );

}


Player::Player(){
    hp = 100;
    x = 0;
    y = 0;
    speed = SPEED;
    dir = RIGHT;
    jump = 0;
    vy = 0;
    isHit = false;
    hitTimer = 0.0f;
    knockbackDir = 0;
    knockbackRemaining = 0;
    attackRadius = ATTACK_RADIUS;
    attackCooldown = 0;
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
        dir = LEFT;
        isMoving = true;
        flag = 1;
    } else if (keyCode == ALLEGRO_KEY_S) {
        dy += this->speed;
        isMoving = false;
    } else if (keyCode == ALLEGRO_KEY_D) {
        dx += this->speed;
        dir = RIGHT;
        isMoving = true;
        flag = 0;
    }
    const int x2 = dx + xkurang;
    const int y2 = dy + ykurang;
    const int w2 = PLAYER_WIDTH - xkurang - xkurang;
    const int h2 = PLAYER_HEIGHT - ykurang;
    if (x2 >= 0 && y2 >= 0 &&
        x2 + w2 - 1 < scene->MapWidth * scene->BlockSize && y2 + h2 - 1 < scene->MapHeight * scene->BlockSize &&
        !scene->map.IsCollision(x2, y2) && !scene->map.IsCollision(x2 + w2 - 1, y2 + h2 - 1) &&
        !scene->map.IsCollision(x2, y2 + h2 - 1) && !scene->map.IsCollision(x2 + w2 - 1, y2)) {
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

    if (isHit) {
        // red tint with some alpha
        ALLEGRO_COLOR redTint = al_map_rgba(255, 0, 0, 200);
        al_draw_tinted_scaled_bitmap(
          bmp, redTint,
          0, 0,
          al_get_bitmap_width(bmp), al_get_bitmap_height(bmp),
          dx, dy,
          PLAYER_WIDTH, PLAYER_HEIGHT,
          flag
        );
    } else {
        al_draw_scaled_bitmap(
          bmp,
          0, 0,
          al_get_bitmap_width(bmp), al_get_bitmap_height(bmp),
          dx, dy,
          PLAYER_WIDTH, PLAYER_HEIGHT,
          flag
        );
    }
}

bool Player::CollideWith(int x, int y) {
    const int x2 = this->x + xkurang;
    const int y2 = this->y + ykurang;
    const int w2 = PLAYER_WIDTH - xkurang - xkurang;
    const int h2 = PLAYER_HEIGHT - ykurang;
    if (x >= x2 && x <= x2 + w2 && y >= y2 && y <= y2 + h2) {
        return true;
    }
    return false;
}


bool Player::enemyInRange(int x, int y) {
    if (dir == RIGHT) {
        if (x > this->x &&
            x < this->x + PLAYER_WIDTH + attackRadius) {
            return true;
        }
    } else if (dir == LEFT) {
        if (x < this->x &&
            x > this->x - attackRadius / 2) {
            return true;
        }
    }
    return false;
}

void Player::Attack() {
    if (attackCooldown > 0.0f) {
        return;
    }
    std::cout << "Attack" << std::endl;
    setState(ATTACK);
    attackCooldown = ATTACK_COOLDOWN_MAX;
    animations[ATTACK].current = 0;
    animations[ATTACK].timer   = 0;

}
