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

class Player : public Engine::Sprite {
protected:
  int hp;
  int x, y;
  int speed;
  int jump;
  Direction dir;

public:
  Player(int hp, int x, int y, int speed);
  void move(int keyCode);
};
#endif   // PLAYER_HPP
