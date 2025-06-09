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
    int x, y;
    Enemy(int hp, int x, int y, int speed, int damage);
    ~Enemy();
    virtual void Update(float deltaTime);
    virtual void move(int keyCode);
    virtual void Draw(Camera cam);
    void Jump();
    int flag;

private:
    void setState(State s);
};
#endif   // PLAYER_HPP
