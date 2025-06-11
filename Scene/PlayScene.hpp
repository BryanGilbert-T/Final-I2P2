#ifndef PLAYSCENE_HPP
#define PLAYSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <list>
#include <memory>
#include <utility>
#include <vector>
#include <set>

#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"
#include "Engine/map.hpp"
#include "Engine/ParallaxBackground.hpp"
#include "Engine/utility.hpp"
#include "Player/Player.hpp"

class Turret;
namespace Engine {
    class Group;
    class Image;
    class Label;
    class Sprite;
    class Map;
}   // namespace Engine


class PlayScene final : public Engine::IScene {
private:
    ALLEGRO_SAMPLE_ID bgmId;
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> deathBGMInstance;

protected:
    int lives;
    int money;
    int SpeedMult;

public:
    static Engine::ParallaxBackground MountainSceneBg; //BACKGROUND
    static Player player;
    std::set<int> keyHeld;
    static Camera cam;
    static Engine::Map map;
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
    ALLEGRO_BITMAP* finishBmp;
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

    void findTeleport();
};
#endif   // PLAYSCENE_HPP
