#ifndef BOSSENEMY_HPP
#define BOSSENEMY_HPP
#include <allegro5/base.h>
#include <list>
#include <string>
#include <map>
#include "Scene/PlayScene.hpp"
#include "Enemy/Enemy.hpp"

#include "Engine/Sprite.hpp"


class Enemy;
class PlayScene;

class BossEnemy: public Enemy{
public:
    BossEnemy(int x, int y);
    ~BossEnemy();
    void Update(float deltaTime) override;
    void move(int keyCode) override;
    void Draw(Camera cam) override;

    AIState   aiState;
    float     patrolOriginX;
    int       patrolDir;         // +1 = right, -1 = left
    float     patrolRange;       // how far from origin to patrol
    float     chaseRadius;       // start chasing when player closer than this
    float     attackRadius;      // optional: trigger attack when even closer

    bool hitPlayer = false;

    void performAttack(float dt, float dist);
    float attackCooldown;

    // helper to fetch player position:
    std::pair<float,float> getPlayerPos() const;
    void performPatrol(float dt);
    void performChase(float dt, float dx, float dy, float dist);
};
#endif   // PLAYER_HPP
