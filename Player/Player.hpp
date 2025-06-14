#ifndef PLAYER_HPP
#define PLAYER_HPP
#include <allegro5/base.h>
#include <list>
#include <string>
#include <vector>
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

  bool CollideWith(int x, int y);
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
  void DrawStamina();
  ALLEGRO_BITMAP* staminaBg;
  ALLEGRO_BITMAP* staminaValue;
  float staminaBarAlpha = 0.0f;
  static constexpr float STAMINA_BAR_FADE_SPEED = 0.8f;
  int flag;
  bool isMoving;
  bool isRunning;
  bool isDying;
  bool died;
  void Attack();


  bool attacked = false;

  float attackCooldown;

  float stamina        = 5.0f;
  const float maxStamina   = 5.0f;
  const float sprintDrain  = 1.0f;    // stamina per second when holding
  const float regenRate    = maxStamina / 5.0f;

  // tap-to-dash:
  const float dashDuration = 0.5f;     // half‐second dash
  float dashTimer         = 0.0f;

  float   sprintCooldownTimer = 0.0f;
  const float sprintCooldownDuration = 5.0f;

  // input timing:
  double lastRightDown    = 0.0;       // timestamp of last right-mouse down
  const double clickThreshold = 0.2;   // max seconds between down/up to count as a “click”
  bool rightHeld          = false;


private:
  void setState(State s);
};
#endif   // PLAYER_HPP
