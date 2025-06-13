#ifndef PLAYSCENE_HPP
#define PLAYSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_font.h>
#include <list>
#include <memory>
#include <utility>
#include <vector>
#include <set>
#include <random>

#include "Engine/IScene.hpp"
#include "Shop/Shop.hpp"
#include "Engine/Chat.hpp"
#include "Engine/Point.hpp"
#include "Engine/map.hpp"
#include "Engine/Location.hpp"
#include "Engine/ParallaxBackground.hpp"
#include "Engine/utility.hpp"
#include "Player/Player.hpp"
#include "Engine/ParallaxCloud.hpp"

class Turret;
namespace Engine {
    class Group;
    class Image;
    class Label;
    class Sprite;
    class Map;
    class Location;
}   // namespace Engine


class PlayScene final : public Engine::IScene {
private:
    ALLEGRO_SAMPLE_ID bgmId;
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> deathBGMInstance;
    ChatBox chatBox;
    bool chatting = false;

protected:
    int lives;
    int money;
    int SpeedMult;
    ALLEGRO_BITMAP* background;
    ALLEGRO_BITMAP* loadingBg;

public:
    bool inShop = false;
    std::vector<bool> chatDone;
    void CheckChatTrigger();
    bool PlayerIsInside(int x, int y);
    Shop* shop;
    float ambientTimer    = 0.0f;                         // seconds elapsed
    static constexpr float AmbientCycle = 4.0f * 60.0f;  // 720 seconds
    bool    isRaining     = false;
    int     currentPhase  = -1;       // last‐seen index 0…5
    std::mt19937 rng;                 // random engine
    std::uniform_int_distribution<int> rainRoll{0,1}; // 0…4

    struct Raindrop {
        float x, y, speed;
    };
    std::vector<Raindrop> drops;

    void DrawLoading(int step);
    ALLEGRO_FONT* PauseFont;
    static Engine::ParallaxBackground MountainSceneBg; //BACKGROUND
    static Engine::ParallaxCloud CloudBg;
    static Player player;
    std::set<int> keyHeld;
    static Camera cam;
    static Engine::Map map;
    static Engine::Location location;
    static bool DebugMode;
    static const std::vector<Engine::Point> directions;
    static int MapWidth, MapHeight;
    static const int BlockSize;
    static const float DangerTime;
    Engine::Point SpawnGridPoint;
    Engine::Point EndGridPoint;
    static const std::vector<int> code;
    int MapId;
    float ticks;
    float deathCountDown;
    // Map tiles.
    Group *TileMapGroup;
    Group *GroundEffectGroup;
    Group *DebugIndicatorGroup;
    Group *BulletGroup;
    Group *TowerGroup;
    Group *EffectGroup;
    Group *UIGroup;
    std::list<Enemy*> enemyGroup;
    Engine::Label *UIMoney;
    Engine::Label *UILives;
    Engine::Image *imgTarget;
    Engine::Sprite *dangerIndicator;
    Turret *preview;
    std::vector<std::vector<TileType>> mapState;
    std::vector<std::vector<int>> mapDistance;
    std::list<std::pair<int, float>> enemyWaveData;
    std::list<int> keyStrokes;
    ALLEGRO_BITMAP* HealthUIBg;
    ALLEGRO_BITMAP* HealthUIValue;
    ALLEGRO_SHADER* lightShader;
    ALLEGRO_BITMAP* vignette;
    float elapsedTime;
    float hpDraw;
    std::vector<ALLEGRO_BITMAP*> Locations;
    static Engine::Point GetClientSize();
    explicit PlayScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void Draw() const override;
    void OnMouseDown(int button, int mx, int my) override;
    void OnMouseMove(int mx, int my) override;
    void OnMouseUp(int button, int mx, int my) override;
    void OnKeyDown(int keyCode) override;
    void OnKeyUp(int keyCode) override;
    void OnKeyHold();
    void Hit();
    int GetMoney() const;
    void EarnMoney(int money);
    void ReadMap();
    void ReadEnemyWave();
    void ConstructUI();
    void UIBtnClicked(int id);
    bool CheckSpaceValid(int x, int y);
    std::vector<std::vector<int>> CalculateBFSDistance();
    // void ModifyReadMapTiles();
    std::vector<Engine::Point> teleportLeft;
    std::vector<Engine::Point> teleportRight;

    bool changeScene = false;
    void Pause(int stage);
    bool pause = false;

    void findTeleport();
};
#endif   // PLAYSCENE_HPP
