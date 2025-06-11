#ifndef PLAYER_HPP
#define PLAYER_HPP
#include <allegro5/base.h>
#include <list>
#include <string>
#include <map>

#include "Engine/Sprite.hpp"

class Enemy;
class PlayScene;

typedef enum {
  IDLE,
  WALK,
  RUN,
  JUMP,
  ATTACK,
  DEAD
} State;

typedef enum {
  LEFT,
  RIGHT,
  UP,
  DOWN,
} Direction;

struct Animation {
  std::vector<ALLEGRO_BITMAP*> frames;  // individual frames
  double                  frame_time;   // seconds per frame
  int                     current;      // current frame index
  double                  timer;        // time accumulator

  Animation(double ft = 0.1)
    : frame_time(ft), current(0), timer(0.0) {}
};

class Player{
protected:
  int speed;
  int jump;
  float vy;
  Direction dir;
  ALLEGRO_BITMAP* idle_sheet;
  std::map<State, Animation> animations;
  State                      state;


public:
  bool isHit;               // true while flashing red
  float hitTimer;           // how long to keep flashing
  int  knockbackDir;        // +1 or -1
  int  knockbackRemaining;  // pixels left to be knocked back
  static constexpr int KNOCKBACK_DISTANCE = 100; // total pixels to push back
  int attackRadius;

  int x, y;
  int hp;
  std::string username;
  void Create(int hp, int x, int y, std::string name);
  Player();
  ~Player();
  void Update(float deltaTime);
  void move(int keyCode);
  void Draw(Camera cam);
  void Jump();
  void Hit(int damage, int flag);
  bool enemyInRange(int x, int y);
  int flag;

  void Attack();

  bool attacked = false;

  float attackCooldown;

private:
  void setState(State s);
};
#endif   // PLAYER_HPP
