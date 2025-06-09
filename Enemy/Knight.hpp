#ifndef KNIGHTENEMY_HPP
#define KNIGHTENEMY_HPP
#include <allegro5/base.h>
#include <list>
#include <string>
#include <map>
#include "Scene/PlayScene.hpp"
#include "Enemy/Enemy.hpp"

#include "Engine/Sprite.hpp"

class Enemy;
class PlayScene;

class KnightEnemy: public Enemy{
public:
    KnightEnemy(int x, int y);
    ~KnightEnemy();
    void Update(float deltaTime) override;
    void move(int keyCode) override;
    void Draw(Camera cam) override;
};
#endif   // PLAYER_HPP
