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
#include <filesystem>
#include <iterator>

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
#include "Enemy/Boss.hpp"
#include "Engine/Location.hpp"

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
Engine::Location PlayScene::location;
int PlayScene::MapWidth = 64;
int PlayScene::MapHeight = 64;
Camera PlayScene::cam;
Engine::ParallaxBackground PlayScene::MountainSceneBg;
Engine::ParallaxCloud PlayScene::CloudBg;

const std::vector<int> PlayScene::code = {
    ALLEGRO_KEY_UP, ALLEGRO_KEY_UP, ALLEGRO_KEY_DOWN, ALLEGRO_KEY_DOWN,
    ALLEGRO_KEY_LEFT, ALLEGRO_KEY_RIGHT, ALLEGRO_KEY_LEFT, ALLEGRO_KEY_RIGHT,
    ALLEGRO_KEY_B, ALLEGRO_KEY_A, ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_ENTER
};
Engine::Point PlayScene::GetClientSize() {
    return Engine::Point(MapWidth * BlockSize, MapHeight * BlockSize);
}
void PlayScene::Initialize() {
    chatting = false;
    chatBox = ChatBox();
    background = al_load_bitmap("Resource/images/friendlist-scene/friendlist-bg.png");
    loadingBg = al_load_bitmap("Resource/images/friendlist-scene/loading-bg.png");
    Engine::Point SpawnGridPoint = Engine::Point(-1, 0);
    Engine::Point EndGridPoint = Engine::Point(-1, 0);
    mapState.clear();
    keyStrokes.clear();
    enemyGroup.clear();
    ticks = 0;
    deathCountDown = -1;
    lives = 10;
    SpeedMult = 1;
    cam.Update(0, 0);
    location.Initialize();
    shop = nullptr;

    int w = Engine::GameEngine::GetInstance().getVirtW();
    int h = Engine::GameEngine::GetInstance().getVirtH();
    int halfW = w / 2;
    int halfH = h / 2;

    //SHADER
    lightShader = al_create_shader(ALLEGRO_SHADER_AUTO);
    std::cerr << "CWD: " << std::filesystem::current_path() << "\n";
    std::ifstream v("Resource/light.vert"), f("Resource/light.frag");
    if (!v.is_open()) std::cerr << "→ Cannot open Resource/light.vert\n";
    if (!f.is_open()) std::cerr << "→ Cannot open Resource/light.frag\n";

    // (2) Attach once, check those bools:
    bool okV = al_attach_shader_source_file(lightShader,
                    ALLEGRO_VERTEX_SHADER,
                    "Resource/light.vert");
    bool okF = al_attach_shader_source_file(lightShader,
                    ALLEGRO_PIXEL_SHADER,
                    "Resource/light.frag");
    std::cerr << "attach vertex: " << okV
              << ", attach fragment: " << okF << "\n";

    if (!okV || !okF) {
        std::cerr << "→ Make sure Resource/light.vert & .frag live under your CWD/Resource/\n";
    }
    bool built = al_build_shader(lightShader);
    const char* log = al_get_shader_log(lightShader);
    std::cerr
      << (built ? "[OK]  " : "[FAIL] ")
      << "Shader build result:\n"
      << log << "\n";
    // (3) Only now build the shader:
    if (!al_build_shader(lightShader)) {
        std::cerr << "Shader build error:\n"
                  << al_get_shader_log(lightShader) << "\n";
    }

    coinIcon = al_load_bitmap("Resource/images/play-scene/shop/AppleIcon.png");
    if (!coinIcon) {
        std::cerr << "<UNK> Cannot load coinIcon.png\n";
    }
    coinFont = al_load_ttf_font("Resource/fonts/imfell.ttf", 64, 0);

    items.clear();
    coins.clear();

    //UI
    elapsedTime = 0.0f;
    Engine::ImageButton *btn;
    btn = new Engine::ImageButton("play-scene/ui/pause-btn.png", "play-scene/ui/pause-btn-hov.png", w * 0.9, h * 0.1, 64, 64);
    btn->SetOnClickCallback(std::bind(&PlayScene::Pause, this, 1));
    AddNewControlObject(btn);
    HealthUIBg = al_load_bitmap("Resource/images/play-scene/ui/life-ui-bg.png");
    HealthUIValue = al_load_bitmap("Resource/images/play-scene/ui/life-ui-value.png");
    vignette = al_load_bitmap("Resource/images/play-scene/vignete.png");

    std::vector<std::string> cloudLayers = {
        "Resource/images/play-scene/mountains/sky.png",
        "Resource/images/play-scene/mountains/clouds-01.png",
        "Resource/images/play-scene/mountains/clouds-02.png",
    };
    std::vector<float> cloudFactor = {0.0, 0.05f, 0.1f};
    CloudBg.Initialize(cloudLayers, cloudFactor);
    CloudBg.SetLayerOffset(0, 0, -700);
    CloudBg.SetLayerOffset(1, 0, -200);
    CloudBg.SetLayerOffset(2, 0, -100);

    goBackPoints.clear();

    std::vector<std::string> layers = {
        "Resource/images/play-scene/mountains/mountains.png",
        "Resource/images/play-scene/mountains/tree.png"
    };
    std::vector<float> factors = {0.1f, 0.25f};

    MountainSceneBg.Initialize(layers, factors);
    MountainSceneBg.SetLayerOffset(0, 0, 0);   // mountains shifted right/down
    MountainSceneBg.SetLayerOffset(1, 0, 0 );   // trees shifted left/up

    teleportLeft.clear();
    teleportRight.clear();

    changeScene = false;
    pause = false;
    DrawLoading(1);

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
    this->money = score;
    this->MoneyBefore = money;
    DrawLoading(2);

    rng.seed(std::random_device{}());

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
    // Start BGM.

    PauseFont = al_load_ttf_font("Resource/fonts/imfell.ttf", 16, 0);
    if (!PauseFont) {
        std::cerr << "Failed to load pause menu font\n";
    }
    for (int i = 3; i <= 9; i++) {
        DrawLoading(i);
        al_rest(0.1);
    }

    if (MapId == 9) {
        std::ofstream filez("Resource/account.txt"); // truncate mode by default
        if (!filez) {
            std::cerr << "Failed to open file for writing.\n";
        }

        int dx = 0;
        int dy = 0;
        if (MapBefore == 1) {
            dx = 12 * BlockSize - (100 - BlockSize);
            dy = 22 * BlockSize - (100 - BlockSize);
        }

        // Write new values into the file
        filez << player.username << " " << MapBefore << " " << dx << " " << dy
        << " " << money << " " << player.hp;

        filez.close();

        updateUser(player.username, dx, dy, money, player.hp, MapBefore);
    }
    DrawLoading(10);
}
void PlayScene::Pause(int stage) {
    pause = !pause;
}
void PlayScene::Terminate() {
    MountainSceneBg.Terminate();
    IScene::Terminate();

    if (coinIcon) {
        al_destroy_bitmap(coinIcon);
    }

    if (changeScene == false && MapId != 9) {
        DrawLoading(1);
        std::ofstream file("Resource/account.txt"); // truncate mode by default
        if (!file) {
            std::cerr << "Failed to open file for writing.\n";
        }

        // Write new values into the file
        file << player.username << " " << MapId << " " << player.x << " " << player.y
        << " " << money << " " << player.hp;
        DrawLoading(2);

        file.close();

        updateUser(player.username, player.x, player.y, money, player.hp, MapId);
        for (int i = 3; i <= 10; i++) {
            DrawLoading(i);
            al_rest(0.1);
        }
    }

    if (lightShader) al_destroy_shader(lightShader);


    if (PauseFont) {
        al_destroy_font(PauseFont);
        PauseFont = nullptr;
    }
}
void PlayScene::DrawLoading(int step) {
    ALLEGRO_DISPLAY* d = al_get_current_display();
    int W = al_get_display_width(d);
    int H = al_get_display_height(d);

    ALLEGRO_TRANSFORM old;
    al_copy_transform(&old, al_get_current_transform());
    ALLEGRO_TRANSFORM identity;
    al_identity_transform(&identity);
    al_use_transform(&identity);

    // 3) Clear to white
    al_draw_scaled_bitmap(background, 0, 0,
                           al_get_bitmap_width(background), al_get_bitmap_height(background),
                           0, 0,
                           W, H, 0);

    // 4) Outline rectangle parameters
    const int totalSteps = 10;
    int barW =  int(W * 0.35f);
    int barH =  int(H * 0.65f);
    int x0   = (W - barW) / 2;
    int y0   = (H - barH) / 2;
    int x1   = x0 + barW;
    int y1   = y0 + barH;

    // 6) Compute segment widths & spacing
    //    inset from the border so we don’t overwrite the border
    const int inset = 4;
    float innerW    = float(barW - 2*inset);
    float innerH    = float(barH - 2*inset);

    // spacing between segments
    float spacing   = 4.0f;
    // solve: stepW * totalSteps + spacing*(totalSteps-1) = innerW
    float stepW     = (innerW - spacing*(totalSteps-1)) / totalSteps;

    step = std::clamp(step, 0, totalSteps);
    float progress = float(step) / float(totalSteps);   // 0.0 → 1.0
    float fillH    = innerH * progress;                 // how much of the bar to fill

    float xL = x0 + inset;
    float xR = x1 - inset;
    float yB = y1 - inset;
    float yT = yB - fillH;

    // 3) DRAW THE FILL **FIRST** (behind the PNG):
    al_draw_filled_rectangle(
      xL, yT,
      xR, yB,
      al_map_rgb(0, 0, 0)
    );

    // 4) NOW DRAW YOUR PNG (with the hollow monkey) on top:
    al_draw_scaled_bitmap(
      loadingBg,
      0,0,
      al_get_bitmap_width(loadingBg),
      al_get_bitmap_height(loadingBg),
      0, 0,
      W, H,
      0
    );

    // 5) flip & restore transforms…
    al_flip_display();
    al_use_transform(&old);
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
                << " " << money << " " << player.hp;

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
                << " " << money << " " << player.hp;

                file.close();
            }
            if(MapId + 1 == 3) {
                int nextx = 2 * BlockSize - (100 - BlockSize);
                int nexty = 19 * BlockSize - (100 - BlockSize);

                std::ofstream file("Resource/account.txt"); // truncate mode by default
                if (!file) {
                    std::cerr << "Failed to open file for writing.\n";
                }

                // Write new values into the file
                file << player.username << " " << 3 << " " << nextx << " " << nexty
                << " " << money << " " << player.hp;

                file.close();
            }
            Engine::GameEngine::GetInstance().ChangeScene("play");
            return;
        }
    }
    for (Engine::Point p : goBackPoints) {
        if (player.x / BlockSize == p.x &&
            player.y / BlockSize == p.y) {
            Engine::GameEngine::GetInstance().ChangeScene("play");
        }
    }
}

bool PlayScene::PlayerIsInside(int x, int y) {
    return (player.x >= x && player.x <= x + BlockSize &&
        player.y >= y);
}

void PlayScene::CheckChatTrigger() {
    int w = Engine::GameEngine::GetInstance().getVirtW();
    int h = Engine::GameEngine::GetInstance().getVirtH();
    int halfW = w / 2;
    int halfH = h / 2;

    if (MapId == 1) {
        if (PlayerIsInside(1396, player.y - 1)) {
            if (chatDone[0] == false) {
                auto it = std::next(enemyGroup.begin(), 4);
                Enemy* e = *it;
                std::vector<DialogueEntry> convo = {
                    DialogueEntry{ "Hey, who's there?",
                        float(e->x - halfW - BlockSize / 2),
                        float(e->y - halfH - BlockSize / 2),
                    e->ENEMY_WIDTH, e->ENEMY_HEIGHT},
                    DialogueEntry{ "Me! The almighty Wu Kuo!",
                    float(player.x - halfW - BlockSize / 2),
                    float(player.y - halfH - BlockSize / 2),
                            157, 100},
                    DialogueEntry{ "Move! Or you will regret it!",
                    float(player.x - halfW - BlockSize / 2),
                    float(player.y - halfH - BlockSize / 2),
                            157, 100},
                    DialogueEntry{ "Try me monke",
                        float(e->x - halfW - BlockSize / 2),
                        float(e->y - halfH - BlockSize / 2),
                    e->ENEMY_WIDTH, e->ENEMY_HEIGHT},
                    // … more lines …
                };
                chatBox.start(convo);

                chatDone[0] = true;
            }
        } else if (PlayerIsInside(5882, 1400)) {
            if (chatDone[1] == false) {

                std::vector<DialogueEntry> convo = {
                    DialogueEntry{ "The scroll is up ahead!",
                    float(player.x - halfW - BlockSize / 2),
                    float(player.y - halfH - BlockSize / 2),
                            157, 100},
                    DialogueEntry{ "Let's get moving!",
                    float(player.x - halfW - BlockSize / 2),
                    float(player.y - halfH - BlockSize / 2),
                            157, 100},
                    // … more lines …
                };
                chatBox.start(convo);

                chatDone[1] = true;
            }
        }
    } else if (MapId == 2) {

    } else if (MapId == 3) {
        if (PlayerIsInside(330, player.y - 1)) {
            if (chatDone[2] == false) {
                std::vector<DialogueEntry> convo = {
                    DialogueEntry{ "Be Careful!",
                    float(player.x - halfW - BlockSize / 2),
                    float(player.y - halfH - BlockSize / 2),
                            157, 100},
                    DialogueEntry{ "I sense something evil near!",
                    float(player.x - halfW - BlockSize / 2),
                    float(player.y - halfH - BlockSize / 2),
                            157, 100},
                };
                chatBox.start(convo);

                chatDone[2] = true;
            }
        }
    }
}


void PlayScene::Update(float deltaTime) {
    IScene::Update(deltaTime);

    if (pause) {
        return;
    }

    std::cout << player.x << " " << player.y << std::endl;

    int w = Engine::GameEngine::GetInstance().getVirtW();
    int h = Engine::GameEngine::GetInstance().getVirtH();
    int halfW = w / 2;
    int halfH = h / 2;

    if (shop) {
        shop->Update(deltaTime, player);
    }

    for (auto it = coins.begin(); it != coins.end(); /*no ++it here*/) {
        Coin* c = *it;
        c->Update(deltaTime, player);

        if (c->playerIsNear) {
            // 1) Award the player
            money += 1;                // or call EarnMoney(1) if you have such a method

            // 3) Erase from the vector and advance the iterator
            it = coins.erase(it);
        }
        else {
            ++it;
        }
    }

    for (Item* i : items) {
        i->Update(deltaTime, player);
    }

    if (!chatBox.isActive()) {
        CheckChatTrigger();
    }

    ambientTimer += deltaTime;
    CloudBg.Update(deltaTime);
    if (ambientTimer >= AmbientCycle)
        ambientTimer -= AmbientCycle;

    int newPhase = int((ambientTimer / AmbientCycle) * 6.0f) % 6;

    // if we just entered a new phase:
    if (newPhase != currentPhase) {
        currentPhase = newPhase;
        // roll 1-in-5 chance:
        isRaining = (rainRoll(rng) == 0);
        // clear any leftover drops
        drops.clear();
    }

    // if raining, spawn & update raindrops
    if (isRaining) {
        // spawn e.g. 50 drops per second:
        static const float spawnRate = 50.0f;
        static float    spawnAcc  = 0.0f;
        spawnAcc += spawnRate * deltaTime;
        while (spawnAcc >= 1.0f) {
            spawnAcc -= 1.0f;
            Raindrop d;
            d.x     = float(rng() % Engine::GameEngine::GetInstance().getVirtW());
            d.y     = -10.0f;                     // start just above screen
            d.speed = 400.0f + float(rng()%200);  // px/sec
            drops.push_back(d);
        }

        // move drops
        for (auto &d : drops) {
            d.y += d.speed * deltaTime;
        }
        // remove off‐screen
        drops.erase(std::remove_if(drops.begin(), drops.end(),
            [&](auto &d){ return d.y > Engine::GameEngine::GetInstance().getVirtH(); }),
            drops.end());
    }
    if (MapId != 9) location.Update(deltaTime);

    if (chatBox.isActive()) {
        player.Update(deltaTime);
        chatBox.update(deltaTime);

        // smooth‐lerp camera toward current speaker
        auto [tx, ty] = chatBox.getCameraTarget();
        float lerp = 3.0f * deltaTime;
        cam.x += (tx - cam.x) * lerp;
        cam.y += (ty - cam.y) * lerp;

        if (cam.x < 0) cam.x = 0;
        if (cam.y < 0) cam.y = 0;

        if (cam.x > MapWidth * BlockSize - w) cam.x = MapWidth * BlockSize - w;
        if (cam.y > MapHeight * BlockSize - h) cam.y = MapHeight * BlockSize - h;

        return;   // skip all the rest of your normal update!
    }

    if (player.hp == 0) {
        if (!player.isDying && !player.died) {
            // Start death animation if not already started
            player.isDying = true;
        }
        else if (player.died) {
            // Animation is complete, show death screen
            return; // Skip other updates
        }
        // Let the death animation continue updating
        player.Update(deltaTime);
        return;
    }

    OnKeyHold();
    player.Update(deltaTime);
    findTeleport();


    hpDraw = std::clamp( float(player.hp) / 100.0f, 0.0f, 1.0f );

    // cam.x = player.x - halfW - BlockSize / 2;
    // cam.y = player.y - halfH - BlockSize / 2;
    float lerp = 2.0f * deltaTime;
    cam.x += ((player.x - halfW - BlockSize / 2) - cam.x) * lerp;
    cam.y += ((player.y - halfH - BlockSize / 2) - cam.y) * lerp;

    if (cam.x < 0) cam.x = 0;
    if (cam.y < 0) cam.y = 0;

    if (cam.x > MapWidth * BlockSize - w) cam.x = MapWidth * BlockSize - w;
    if (cam.y > MapHeight * BlockSize - h) cam.y = MapHeight * BlockSize - h;

    auto it = enemyGroup.begin();
    while (it != enemyGroup.end()) {
        Enemy* e = *it;
        e->Update(deltaTime);

        if (e->timetoDie) {
            // unlink from list and advance iterator in one call:
            it = enemyGroup.erase(it);
        }
        else {
            ++it;
        }
    }
}
void PlayScene::Draw() const {
    //al_clear_to_color(al_map_rgb(0, 0, 0));

    int w = Engine::GameEngine::GetInstance().getVirtW();
    int h = Engine::GameEngine::GetInstance().getVirtH();
    int halfW = w / 2;

    // draw parallax behind everything
    float phase = ambientTimer / AmbientCycle;

    // Option A: smooth cosine-based brightness in [0,1]:
    //    at phase=0 → brightness=1 (full day)
    //    at phase=0.5 → brightness=0 (midnight)
    float brightness = 0.5f * (cosf(phase * 2.0f * M_PI) + 1.0f);

    // Define your day‐ and night‐colours:
    static constexpr float MorningAmb[3]   = { 0.5f, 0.35f, 0.4f };   // cool dawn
    static constexpr float NoonAmb[3]      = { 0.7f, 0.7f, 0.55f };   // bright day
    static constexpr float NormalAmb[3]    = { 1.0f, 1.0f, 1.0f };
    static constexpr float AfternoonAmb[3] = { 0.8f, 0.6f, 0.4f };    // warm late day
    static constexpr float DuskNightAmb[3]    = { 0.5f, 0.3f, 0.5f };
    static constexpr float EveningAmb[3]   = { 0.2f, 0.15f, 0.3f };   // purple dusk

    // Interpolate each channel:
    float scaled = phase * 6.0f;
    int   segment = static_cast<int>(scaled) % 6;      // 0,1,2,3
    float localT = scaled - float(segment);            // 0→1 within the segment

    // pick endpoints
    const float* A;
    const float* B;
    switch (segment) {
        case 1: A = NormalAmb;   B = AfternoonAmb;    break;
        case 2: A = AfternoonAmb; B = DuskNightAmb; break;
        case 3: A = DuskNightAmb; B = EveningAmb;  break;
        case 4: A = EveningAmb; B = MorningAmb;  break;
        case 5: A = MorningAmb; B = NoonAmb;  break;
        default: A = NoonAmb; B = NormalAmb;  break;
    }

    // interpolate
    float amb[3];
    for (int i = 0; i < 3; i++) {
        amb[i] = A[i] + localT * (B[i] - A[i]);
    }

    // draw parallax behind everything
    if (MapId != 9) {
        al_use_shader(lightShader);
        al_set_shader_float_vector("ambient", 3, amb, 1);
        al_set_shader_int("numLights", 1); // No dynamic lights for static geometry
    }
    CloudBg.Draw(cam);
    MountainSceneBg.Draw(cam);
    map.DrawMap(cam);

    al_use_shader(nullptr);
    if (shop) {
        shop->Draw(cam);
    }

    for (Coin* c : coins) {
        c->Draw(cam);
    }

    const int iconx = w * 0.05;
    const int icony = h * 0.1;
    const int iconw = 64;
    const int iconh = 64;
    al_draw_scaled_bitmap(coinIcon,
        0, 0, 32, 32,
        iconx, icony, iconw, iconh,
        0);

    const int distance = BlockSize * 1.5;
    const int fontHeight = al_get_font_line_height(coinFont);
    al_draw_text(coinFont, al_map_rgb(255, 255, 255),
        iconx + distance, icony - (fontHeight/2), ALLEGRO_ALIGN_LEFT,
        std::to_string(money).c_str());

    for (Item* i : items) {
        i->Draw(cam);
    }

    player.Draw(cam);
    for (Enemy* e : enemyGroup) {
        e->Draw(cam);
    }
    if (isRaining) {
        for (auto &d : drops) {
            float lx = d.x;
            float ly = d.y;
            float ly2 = ly + 15.0f; // length of the streak

            // draw a thin semi-transparent line:
            al_draw_line(lx,  ly,
                         lx,  ly2,
                         al_map_rgba(200,200,255,120),
                         3.0f);
        }
    }
    Group::Draw();
    if (MapId != 9) location.Draw(MapId);
    player.DrawStamina();

    al_draw_tinted_scaled_bitmap(vignette, al_map_rgba(255, 255, 255, 50), 0, 0, al_get_bitmap_width(vignette), al_get_bitmap_height(vignette), 0, 0, w, h, 0);
    //HEALTH UI
    al_draw_scaled_bitmap(HealthUIValue, 0, 0, al_get_bitmap_width(HealthUIValue), al_get_bitmap_height(HealthUIValue),
                        halfW - 224, h * 0.9 + 10 + 15, al_get_bitmap_width(HealthUIValue) * hpDraw, al_get_bitmap_height(HealthUIValue),0);
    al_draw_scaled_bitmap(HealthUIBg, 0, 0, al_get_bitmap_width(HealthUIBg), al_get_bitmap_height(HealthUIBg),
                    halfW - al_get_bitmap_width(HealthUIBg)/2, h * 0.9 + 10,
                    al_get_bitmap_width(HealthUIBg), al_get_bitmap_height(HealthUIBg), 0);

    if (player.isDying) {
        player.Draw(cam);   // this will draw the current DYING frame
        return;
    }

    if (player.hp == 0 && player.died) {

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
                     ALLEGRO_ALIGN_CENTER, "Try Again");
        al_draw_text(PauseFont, fg,
                     bx + ButtonW/2, byExit + ButtonH/2 - 8,
                     ALLEGRO_ALIGN_CENTER, "Exit");
        return;
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
    chatBox.draw(cam);
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

    if ((button & 1) && chatBox.isActive()) {
        chatBox.OnMouseClick();
        return;
    }

    if (button & 2) {
        player.lastRightDown = al_get_time();
        player.rightHeld    = true;
        return;
    }

    if ((player.hp == 0) && (button & 1)) {
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
            std::ofstream file("Resource/account.txt"); // truncate mode by default
            if (!file) {
                std::cerr << "Failed to open file for writing.\n";
            }

            // Write new values into the file
            file << player.username << " " << 1 << " " << 820 << " " << 1372
            << " " << MoneyBefore << " " << 100;

            file.close();

            changeScene = true;
            player.isDying = false;
            player.died = false;
            player.hp = 100;

            Engine::GameEngine::GetInstance().ChangeScene("play");
            return;   // don’t fall through to normal click logic
        }
        // Exit?
        if (mx >= bx && mx <= bx + ButtonW &&
            my >= byE && my <= byE + ButtonH)
        {
            MapId = 1;
            player.hp = 100;
            player.x = 820;
            player.y = 1372;
            money = MoneyBefore;

            Engine::GameEngine::GetInstance().ChangeScene("boarding");
        }
        return;
    }

    IScene::OnMouseDown(button, mx, my);

    if (button & 1) {
        player.Attack();
    }
}
void PlayScene::OnMouseMove(int mx, int my) {
    IScene::OnMouseMove(mx, my);
}
void PlayScene::OnMouseUp(int button, int mx, int my) {
    IScene::OnMouseUp(button, mx, my);

    if (button & 2) {
        double held = al_get_time() - player.lastRightDown;
        player.rightHeld = false;
        if (held < player.clickThreshold) {
            // tap → start dash
            player.dashTimer = player.dashDuration;
            player.isRunning = true;
        }
        return;
    }
}
void PlayScene::OnKeyDown(int keyCode) {
    IScene::OnKeyDown(keyCode);
    if (chatBox.isActive()) {
        chatBox.OnKeyDown(keyCode);
        return;
    }
    for (auto it = items.begin(); it != items.end(); ++it) {
        Item* i = *it;
        if (keyCode == ALLEGRO_KEY_F && i->playerIsNear)
        {
            if (i->type == ITEM_APPLE) {
                std::cout << "lai";
                if (money >= i->price) {
                    player.hp = std::min(player.hp + 10, 100);
                    money -= i->price;
                    it = items.erase(it);

                    std::ofstream filez("Resource/account.txt"); // truncate mode by default
                    if (!filez) {
                        std::cerr << "Failed to open file for writing.\n";
                    }
                    int dx = 0;
                    int dy = 0;
                    if (MapBefore == 1) {
                        dx = 12 * BlockSize - (100 - BlockSize);
                        dy = 22 * BlockSize - (100 - BlockSize);
                    }
                    // Write new values into the file
                    filez << player.username << " " << MapBefore << " " << dx << " " << dy
                    << " " << money << " " << player.hp;
                    filez.close();

                    break;
                }
            } else if (i->type == ITEM_PEACH) {

            }

            // updateUser(player.username, dx, dy, money, player.hp, MapBefore);
        }
    }
    if (shop) {
        if (shop->playerIsNear && keyCode == ALLEGRO_KEY_F) {
            changeScene = true;
            int nextx = 5 * BlockSize - (100 - BlockSize);
            int nexty = 23 * BlockSize - (100 - BlockSize);

            std::ofstream file("Resource/account.txt"); // truncate mode by default
            if (!file) {
                std::cerr << "Failed to open file for writing.\n";
            }

            // Write new values into the file
            file << player.username << " " << 9 << " " << nextx << " " << nexty
            << " " << money << " " << player.hp;

            file.close();

            MapBefore = MapId;

            Engine::GameEngine::GetInstance().ChangeScene("play");
            return;
        }
    }
    keyHeld.insert(keyCode);
    if (keyCode == ALLEGRO_KEY_W || keyCode == ALLEGRO_KEY_SPACE) {
        player.Jump();
    }
}
void PlayScene::OnKeyUp(int keyCode) {
    IScene::OnKeyUp(keyCode);
    keyHeld.erase(keyCode);
    if (keyCode == ALLEGRO_KEY_A || keyCode == ALLEGRO_KEY_D) {
        player.isMoving = false;
    }
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
            case 'B': mapData.push_back(6); break;
            case '8': mapData.push_back(7); break;
            case '9': mapData.push_back(8); break;
            case 'T': mapData.push_back(9); break;
            case 'J': mapData.push_back(10); break;
            case 'K': mapData.push_back(11); break;
            case 'C': mapData.push_back(12); break;
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
                shop = new Shop(j * BlockSize - (260 - BlockSize), i * BlockSize - (200 - BlockSize));
            } else if (num == 3) {
                mapState[i][j] = TILE_SKY;
                enemyGroup.push_back(new KnightEnemy(j * BlockSize - (120*2.5 - BlockSize), i * BlockSize - (80*2.5 - BlockSize)));
            } else if (num == 4) {
                mapState[i][j] = TILE_SKY;
                teleportRight.emplace_back(Engine::Point(j * BlockSize, i * BlockSize));
            } else if (num == 5) {
                mapState[i][j] = TILE_SKY;
                teleportLeft.emplace_back(Engine::Point(j * BlockSize, i * BlockSize));
            } else if (num == 6) {
                mapState[i][j] = TILE_SKY;
                enemyGroup.push_back(new BossEnemy(j * BlockSize - (120*2.5 - BlockSize), i * BlockSize - (80*2.5 - BlockSize)));
            } else if (num == 7) {
                mapState[i][j] = SHOP_SKY;
            } else if (num == 8) {
                mapState[i][j] = TILE_DIRT;
            } else if (num == 9) {
                mapState[i][j] = SHOP_SKY;
                goBackPoints.emplace_back(Engine::Point{float(j), float(i)});
            } else if (num == 10) {
                mapState[i][j] = SHOP_SKY;
                items.emplace_back(new Item(j * BlockSize, i * BlockSize, 32, 32,
                    64, 64,
                    2, "Resource/images/play-scene/shop/Apple.png", 8, 0.25f,
                    ITEM_APPLE));
            } else if (num == 11) {
                mapState[i][j] = SHOP_SKY;
                // items.emplace_back();
            } else if (num == 12) {
                mapState[i][j] = TILE_SKY;
                coins.emplace_back(new Coin(j * BlockSize - (64 - BlockSize), i * BlockSize - (64 - BlockSize)));
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