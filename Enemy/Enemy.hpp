#ifndef ENEMY_HPP
#define ENEMY_HPP
#include <allegro5/base.h>
#include <list>
#include <string>
#include <map>
#include "Scene/PlayScene.hpp"

#include "Engine/Sprite.hpp"

class Enemy;
class PlayScene;

typedef enum {
    E_IDLE,
    E_WALK,
    E_RUN,
    E_JUMP,
    E_ATTACK,
} EnemyState;


class Enemy{
protected:
    int hp;
    int speed;
    int jump;
    int damage;
    float vy;
    Direction dir;
    ALLEGRO_BITMAP* idle_sheet;
    std::map<State, Animation> animations;
    State                      state;


public:
    bool  isHit        = false;    // true while we’re being knocked back
    float hitTimer     = 0.0f;     // how long to flash red
    int   knockbackDir = 0;        // +1 or -1
    int   knockbackRem = 0;        // pixels left to push
    static constexpr int KNOCKBACK_DISTANCE = 64;

    void Hit(int dmg, int dir);

    int x, y;
    int ENEMY_WIDTH;
    int ENEMY_HEIGHT;
    Enemy(int hp, int x, int y, int speed, int damage, int w, int h);
    ~Enemy();
    virtual void Update(float deltaTime);
    virtual void move(int keyCode);
    virtual void Draw(Camera cam);
    void Jump();
    void Hit(int dmg);
    int flag;
    void setState(State s);
};
#endif   // PLAYER_HPP
