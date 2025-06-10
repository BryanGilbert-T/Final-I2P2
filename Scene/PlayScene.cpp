#include <algorithm>
#include <allegro5/allegro.h>
#include <cmath>
#include <fstream>
#include <functional>
#include <memory>
#include <queue>
#include <string>
#include <vector>

#include "Enemy/Enemy.hpp"
#include "Enemy/Knight.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/LOG.hpp"
#include "Engine/map.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "UI/Component/Label.hpp"

// TODO HACKATHON-4 (1/3): Trace how the game handles keyboard input.
// TODO HACKATHON-4 (2/3): Find the cheat code sequence in this file.
// TODO HACKATHON-4 (3/3): When the cheat code is entered, a plane should be spawned and added to the scene.
// TODO HACKATHON-5 (1/4): There's a bug in this file, which crashes the game when you win. Try to find it.
// TODO HACKATHON-5 (2/4): The "LIFE" label are not updated when you lose a life. Try to fix it.

bool PlayScene::DebugMode = false;
const std::vector<Engine::Point> PlayScene::directions = { Engine::Point(-1, 0), Engine::Point(0, -1), Engine::Point(1, 0), Engine::Point(0, 1) };
const int PlayScene::BlockSize = 64;
const float PlayScene::DangerTime = 7.61;
Engine::Map PlayScene::map;
Player PlayScene::player;
int PlayScene::MapWidth = 64;
int PlayScene::MapHeight = 64;
Camera PlayScene::cam;
Engine::ParallaxBackground PlayScene::MountainSceneBg;
const std::vector<int> PlayScene::code = {
    ALLEGRO_KEY_UP, ALLEGRO_KEY_UP, ALLEGRO_KEY_DOWN, ALLEGRO_KEY_DOWN,
    ALLEGRO_KEY_LEFT, ALLEGRO_KEY_RIGHT, ALLEGRO_KEY_LEFT, ALLEGRO_KEY_RIGHT,
    ALLEGRO_KEY_B, ALLEGRO_KEY_A, ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_ENTER
};
Engine::Point PlayScene::GetClientSize() {
    return Engine::Point(MapWidth * BlockSize, MapHeight * BlockSize);
}
void PlayScene::Initialize() {
    Engine::Point SpawnGridPoint = Engine::Point(-1, 0);
    Engine::Point EndGridPoint = Engine::Point(-1, 0);
    mapState.clear();
    keyStrokes.clear();
    ticks = 0;
    deathCountDown = -1;
    lives = 10;
    money = 150;
    SpeedMult = 1;
    cam.Update(0, 0);

    std::vector<std::string> layers = {
        "Resource/images/play-scene/mountains/clouds-01.png",
        "Resource/images/play-scene/mountains/clouds-02.png",
        "Resource/images/play-scene/mountains/mountains.png",
        "Resource/images/play-scene/mountains/tree.png"
    };
    std::vector<float> factors = {0.2f, 0.5f, 0.8f, 1.0f};
    MountainSceneBg.Initialize(layers, factors);

    // Add groups from bottom to top.
    AddNewObject(TileMapGroup = new Group());
    AddNewObject(GroundEffectGroup = new Group());
    AddNewObject(DebugIndicatorGroup = new Group());
    AddNewObject(TowerGroup = new Group());
    AddNewObject(BulletGroup = new Group());
    AddNewObject(EffectGroup = new Group());
    // Should support buttons.
    AddNewControlObject(UIGroup = new Group());
    ReadMap();
    mapDistance = CalculateBFSDistance();
    // Preload Lose Scene
    deathBGMInstance = Engine::Resources::GetInstance().GetSampleInstance("astronomia.ogg");
    Engine::Resources::GetInstance().GetBitmap("lose/benjamin-happy.png");
    // Start BGM.
    bgmId = AudioHelper::PlayBGM("play.ogg");
}
void PlayScene::Terminate() {
    MountainSceneBg.Terminate();
    AudioHelper::StopBGM(bgmId);
    AudioHelper::StopSample(deathBGMInstance);
    deathBGMInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}
void PlayScene::Update(float deltaTime) {
    IScene::Update(deltaTime);
    OnKeyHold();
    player.Update(deltaTime);

    int w = Engine::GameEngine::GetInstance().getVirtW();
    int h = Engine::GameEngine::GetInstance().getVirtH();
    int halfW = w / 2;
    int halfH = h / 2;

    cam.x = player.x - halfW - BlockSize / 2;
    cam.y = player.y - halfH - BlockSize / 2;

    if (cam.x < 0) cam.x = 0;
    if (cam.y < 0) cam.y = 0;

    if (cam.x > MapWidth * BlockSize - w) cam.x = MapWidth * BlockSize - w;
    if (cam.y > MapHeight * BlockSize - h) cam.y = MapHeight * BlockSize - h;

    for (Enemy* e : enemyGroup) {
        e->Update(deltaTime);
    }
}
void PlayScene::Draw() const {
    IScene::Draw();
    int w = Engine::GameEngine::GetInstance().getVirtW();
    int h = Engine::GameEngine::GetInstance().getVirtH();

    // draw parallax behind everything
    MountainSceneBg.Draw(cam);
    map.DrawMap(cam);
    player.Draw(cam);
    for (Enemy* e : enemyGroup) {
        e->Draw(cam);
    }
    if (DebugMode) {
        // Draw reverse BFS distance on all reachable blocks.
        for (int i = 0; i < MapHeight; i++) {
            for (int j = 0; j < MapWidth; j++) {
                if (mapDistance[i][j] != -1) {
                    // Not elegant nor efficient, but it's quite enough for debugging.
                    Engine::Label label(std::to_string(mapDistance[i][j]), "pirulen.ttf", 32, (j + 0.5) * BlockSize, (i + 0.5) * BlockSize);
                    label.Anchor = Engine::Point(0.5, 0.5);
                    label.Draw();
                }
            }
        }
    }
}
void PlayScene::OnMouseDown(int button, int mx, int my) {
    IScene::OnMouseDown(button, mx, my);
}
void PlayScene::OnMouseMove(int mx, int my) {
    IScene::OnMouseMove(mx, my);
}
void PlayScene::OnMouseUp(int button, int mx, int my) {
    IScene::OnMouseUp(button, mx, my);
}
void PlayScene::OnKeyDown(int keyCode) {
    IScene::OnKeyDown(keyCode);
    keyHeld.insert(keyCode);

    if (keyCode == ALLEGRO_KEY_W || keyCode == ALLEGRO_KEY_SPACE) {
        player.Jump();
    }
}
void PlayScene::OnKeyUp(int keyCode) {
    IScene::OnKeyUp(keyCode);
    keyHeld.erase(keyCode);
}
void PlayScene::OnKeyHold() {
    for (int key : keyHeld) {
        switch (key){
            case ALLEGRO_KEY_W:
            case ALLEGRO_KEY_A:
            case ALLEGRO_KEY_S:
            case ALLEGRO_KEY_D:
                player.move(key);
                break;
            default:
                break;
        }
    }
}

void PlayScene::Hit() {

}
int PlayScene::GetMoney() const {
    return money;
}
void PlayScene::EarnMoney(int money) {
    this->money += money;
}
void PlayScene::ReadMap() {
    std::string filename = std::string("Resource/map") + std::to_string(MapId) + ".txt";
    // Read map file.
    char c;
    std::vector<int> mapData;
    std::ifstream fin(filename);
    fin >> MapHeight >> MapWidth;
    while (fin >> c) {
        switch (c) {
            case '0': mapData.push_back(0); break;
            case '1': mapData.push_back(1); break;
            case 'S': mapData.push_back(2); break;
            case 'E': mapData.push_back(3); break;
            case '\n':
            case '\r':
                if (static_cast<int>(mapData.size()) / MapWidth != 0)
                    throw std::ios_base::failure("Map data is corrupted.");
                break;
            default: throw std::ios_base::failure("Map data is corrupted.");
        }
    }
    fin.close();
    // Validate map data.
    if (static_cast<int>(mapData.size()) != MapWidth * MapHeight)
        throw std::ios_base::failure("Map data is corrupted.");
    // Store map in 2d array.
    mapState = std::vector<std::vector<TileType>>(MapHeight, std::vector<TileType>(MapWidth));
    for (int i = 0; i < MapHeight; i++) {
        for (int j = 0; j < MapWidth; j++) {
            const int num = mapData[i * MapWidth + j];
            if (num == 0) {
                mapState[i][j] = TILE_SKY;
            } else if (num == 1) {
                mapState[i][j] = TILE_DIRT;
            } else if (num == 2) {
                mapState[i][j] = TILE_SKY;
                player.Create(100, j * BlockSize - (100 - BlockSize), i * BlockSize - (100 - BlockSize));
            } else if (num == 3) {
                mapState[i][j] = TILE_SKY;
                enemyGroup.push_back(new KnightEnemy(j * BlockSize - (120*2.5 - BlockSize), i * BlockSize - (80*2.5 - BlockSize)));
            }
            std::cout << num;
        }
        std::cout << std::endl;
    }
    map.Init(MapWidth, MapHeight, mapState);
}
void PlayScene::ReadEnemyWave() {

}
void PlayScene::ConstructUI() {

}

void PlayScene::UIBtnClicked(int id) {
   }

bool PlayScene::CheckSpaceValid(int x, int y) {
    return true;
}
std::vector<std::vector<int>> PlayScene::CalculateBFSDistance() {
    // Reverse BFS to find path.
    std::vector<std::vector<int>> map(MapHeight, std::vector<int>(std::vector<int>(MapWidth, -1)));
    std::queue<Engine::Point> que;
    // Push end point.
    // BFS from end point.
    if (mapState[MapHeight - 1][MapWidth - 1] != TILE_DIRT)
        return map;
    que.push(Engine::Point(MapWidth - 1, MapHeight - 1));
    map[MapHeight - 1][MapWidth - 1] = 0;
    while (!que.empty()) {
        Engine::Point p = que.front();
        que.pop();
        // TODO PROJECT-1 (1/1): Implement a BFS starting from the most right-bottom block in the map.
        //               For each step you should assign the corresponding distance to the most right-bottom block.
        //               mapState[y][x] is TILE_DIRT if it is empty.
    }
    return map;
}