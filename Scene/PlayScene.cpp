#include <algorithm>
#include <allegro5/allegro.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
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
#include "UI/Component/ImageButton.hpp"
#include "Engine/Sheets.hpp"
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
    enemyGroup.clear();
    ticks = 0;
    deathCountDown = -1;
    lives = 10;
    money = 150;
    SpeedMult = 1;
    cam.Update(0, 0);

    int w = Engine::GameEngine::GetInstance().getVirtW();
    int h = Engine::GameEngine::GetInstance().getVirtH();
    int halfW = w / 2;
    int halfH = h / 2;

    Engine::ImageButton *btn;
    btn = new Engine::ImageButton("play-scene/pause-btn.png", "play-scene/pause-btn-hov.png", w * 0.9, h * 0.1, 64, 64);
    btn->SetOnClickCallback(std::bind(&PlayScene::Pause, this, 1));
    AddNewControlObject(btn);

    std::vector<std::string> layers = {
        "Resource/images/play-scene/mountains/mountains.png",
        "Resource/images/play-scene/mountains/tree.png"
    };
    std::vector<float> factors = {0.1f, 0.25f};
    MountainSceneBg.Initialize(layers, factors);
    MountainSceneBg.SetLayerOffset(0, 0, 30);   // mountains shifted right/down
    MountainSceneBg.SetLayerOffset(1, 0, 50 );   // trees shifted left/up
    finishBmp = al_load_bitmap("Resource/images/play-scene/mountains/finish.png");

    teleportLeft.clear();
    teleportRight.clear();

    changeScene = false;
    pause = false;

    std::ifstream file("Resource/account.txt");
    if (!file) {
        std::cerr << "Failed to open file.\n";
    }
    std::string username;
    int level, x, y, score, hp;
    // Read values from the file
    file >> username >> level >> x >> y >> score >> hp;
    file.close();
    this->MapId = level;
    this->player.Create(hp, x, y, username);

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
    std::cout << player.x << " " << player.y << std::endl;
    mapDistance = CalculateBFSDistance();
    // Preload Lose Scene
    deathBGMInstance = Engine::Resources::GetInstance().GetSampleInstance("astronomia.ogg");
    Engine::Resources::GetInstance().GetBitmap("lose/benjamin-happy.png");
    // Start BGM.
    bgmId = AudioHelper::PlayBGM("play.ogg");

    PauseFont = al_load_ttf_font("Resource/fonts/imfell.ttf", 16, 0);
    if (!PauseFont) {
        std::cerr << "Failed to load pause menu font\n";
    }
}
void PlayScene::Pause(int stage) {
    pause = !pause;
}
void PlayScene::Terminate() {
    MountainSceneBg.Terminate();
    AudioHelper::StopBGM(bgmId);
    AudioHelper::StopSample(deathBGMInstance);
    deathBGMInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();

    if (changeScene == false) {
        std::ofstream file("Resource/account.txt"); // truncate mode by default
        if (!file) {
            std::cerr << "Failed to open file for writing.\n";
        }

        // Write new values into the file
        file << player.username << " " << MapId << " " << player.x << " " << player.y
        << " " << 0 << " " << player.hp;

        file.close();

        updateUser(player.username, player.x, player.y, 0, player.hp, MapId);
    }

    if (PauseFont) {
        al_destroy_font(PauseFont);
        PauseFont = nullptr;
    }
}
void PlayScene::findTeleport() {
    for (Engine::Point p : teleportLeft) {
        if (player.x >= p.x && player.x <= p.x + BlockSize &&
            player.y >= p.y && player.y <= p.y + BlockSize) {
            changeScene = true;
            if(MapId - 1 == 1) {
                int nextx = 99 * BlockSize - (100 - BlockSize);
                int nexty = 24 * BlockSize - (100 - BlockSize);

                std::ofstream file("Resource/account.txt"); // truncate mode by default
                if (!file) {
                    std::cerr << "Failed to open file for writing.\n";
                }

                // Write new values into the file
                file << player.username << " " << 1 << " " << nextx << " " << nexty
                << " " << 0 << " " << player.hp;

                file.close();
            }
            Engine::GameEngine::GetInstance().ChangeScene("play");
            return;
        }
    }
    for (Engine::Point p : teleportRight) {
        if (player.x >= p.x && player.x <= p.x + BlockSize &&
            player.y >= p.y && player.y <= p.y + BlockSize) {
            changeScene = true;
            if(MapId + 1 == 2) {
                int nextx = 3 * BlockSize - (100 - BlockSize);
                int nexty = 21 * BlockSize - (100 - BlockSize);

                std::ofstream file("Resource/account.txt"); // truncate mode by default
                if (!file) {
                    std::cerr << "Failed to open file for writing.\n";
                }

                // Write new values into the file
                file << player.username << " " << 2 << " " << nextx << " " << nexty
                << " " << 0 << " " << player.hp;

                file.close();
            }
            if(MapId + 1 == 3) {
                int nextx = 10 * BlockSize - (100 - BlockSize);
                int nexty = 3 * BlockSize - (100 - BlockSize);

                std::ofstream file("Resource/account.txt"); // truncate mode by default
                if (!file) {
                    std::cerr << "Failed to open file for writing.\n";
                }

                // Write new values into the file
                file << player.username << " " << 3 << " " << nextx << " " << nexty
                << " " << 0 << " " << player.hp;

                file.close();
            }
            Engine::GameEngine::GetInstance().ChangeScene("play");
            return;
        }
    }
}

void PlayScene::Update(float deltaTime) {
    IScene::Update(deltaTime);
    if (pause) {
        return;
    }
    OnKeyHold();
    player.Update(deltaTime);
    findTeleport();

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

    int w = Engine::GameEngine::GetInstance().getVirtW();
    int h = Engine::GameEngine::GetInstance().getVirtH();

    // draw parallax behind everything
    MountainSceneBg.Draw(cam);

    map.DrawMap(cam);
    player.Draw(cam);
    for (Enemy* e : enemyGroup) {
        e->Draw(cam);
    }
    Group::Draw();
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
    if (pause) {
        const int recw = 600;
        const int rech = 400;
        int left = w/2 - recw/2;
        int top  = h/2 - rech/2;

        const int ButtonW = 200;
        const int ButtonH = 50;

        // Compute center‐aligned button positions
        int bx = w/2 - ButtonW/2;
        int byContinue = top + 100;
        int byExit     = byContinue + ButtonH + 20;

        // draw button backgrounds
        ALLEGRO_COLOR bg = al_map_rgb(60,60,60);
        ALLEGRO_COLOR fg = al_map_rgb(255,255,255);
        al_draw_filled_rectangle(bx,              byContinue,
                                 bx + ButtonW,   byContinue + ButtonH,
                                 bg);
        al_draw_filled_rectangle(bx,              byExit,
                                 bx + ButtonW,   byExit     + ButtonH,
                                 bg);

        // draw labels
        al_draw_text(PauseFont, fg,
                     bx + ButtonW/2, byContinue + ButtonH/2 - 8,
                     ALLEGRO_ALIGN_CENTER, "Continue");
        al_draw_text(PauseFont, fg,
                     bx + ButtonW/2, byExit + ButtonH/2 - 8,
                     ALLEGRO_ALIGN_CENTER, "Exit");

    }
}
void PlayScene::OnMouseDown(int button, int mx, int my) {
    if (pause && (button & 1)) {
        const int w = Engine::GameEngine::GetInstance().getVirtW();
        const int h = Engine::GameEngine::GetInstance().getVirtH();

        const int recw = 600;
        const int rech = 400;
        int left = w/2 - recw/2;
        int top  = h/2 - rech/2;

        const int ButtonW = 200;
        const int ButtonH = 50;

        int bx   = w/2 - ButtonW/2;
        int byC  = top + 100;
        int byE  = byC + ButtonH + 20;

        // Continue?
        if (mx >= bx && mx <= bx + ButtonW &&
            my >= byC && my <= byC + ButtonH)
        {
            pause = false;
            return;   // don’t fall through to normal click logic
        }
        // Exit?
        if (mx >= bx && mx <= bx + ButtonW &&
            my >= byE && my <= byE + ButtonH)
        {
            Engine::GameEngine::GetInstance().ChangeScene("boarding");
        }
        return;
    }

    IScene::OnMouseDown(button, mx, my);

    if (button & 1) {
        player.Attack(enemyGroup);
    }
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
            case 'W': mapData.push_back(4); break;
            case 'L': mapData.push_back(5); break;
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
            } else if (num == 3) {
                mapState[i][j] = TILE_SKY;
                enemyGroup.push_back(new KnightEnemy(j * BlockSize - (120*2.5 - BlockSize), i * BlockSize - (80*2.5 - BlockSize)));
            } else if (num == 4) {
                mapState[i][j] = TILE_SKY;
                teleportRight.emplace_back(Engine::Point(j * BlockSize, i * BlockSize));
            } else if (num == 5) {
                mapState[i][j] = TILE_SKY;
                teleportLeft.emplace_back(Engine::Point(j * BlockSize, i * BlockSize));
            }
        }
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