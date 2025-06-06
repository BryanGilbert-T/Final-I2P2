#ifndef PLAYER_HPP
#define PLAYER_HPP
#include <allegro5/base.h>
#include <list>
#include <string>

#include "Engine/Sprite.hpp"

class Enemy;
class PlayScene;

typedef enum {
  IDLE,
  WALK,
  RUN,
  JUMP,
  ATTACK,
} State;

typedef enum {
  LEFT,
  RIGHT,
  UP,
  DOWN,
} Direction;

class Player{
protected:
  int hp;
  int x, y;
  int speed;
  int jump;
  Direction dir;

public:
  void Create(int hp, int x, int y);
  Player();
  void move(int keyCode);
  void Draw();
};
#endif   // PLAYER_HPP
