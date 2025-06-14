#include <allegro5/allegro_audio.h>
#include <functional>
#include <memory>
#include <string>
#include <fstream>
#include <iostream>

#include "Engine/AudioHelper.hpp"
#include "Engine/Point.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Collider.hpp"
#include "Engine/Sheets.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "Boarding.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"

const int ANIM_FRAME_COUNT = 2;
const float ANIM_FRAME_RATE = 1.0f;

const int START_FRAME_COUNT = 6;
const float START_FRAME_RATE = 0.35f;

void BoardingScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().getVirtW();
    int h = Engine::GameEngine::GetInstance().getVirtH();
    int halfW = w / 2;
    int halfH = h / 2;
    Engine::ImageButton *btn;
    playing = false;
    delayingSceneChange = false;

    PlayFont = al_load_font("Resource/fonts/imfell.ttf", 45, ALLEGRO_ALIGN_CENTER);

    Logo = al_load_bitmap("Resource/images/stage-select/logo-white.png");
    smallFont = al_load_font("Resource/fonts/imfell.ttf", 24, ALLEGRO_ALIGN_CENTER);

    start_sheet = al_load_bitmap("Resource/images/stage-select/start-sheet.png");
    if (!start_sheet) {
        std::cerr << "Failed to load start bitmap" << std::endl;
    }
    int frameW = al_get_bitmap_width(start_sheet)/ANIM_FRAME_COUNT;
    int frameH = al_get_bitmap_height(start_sheet);
    Animation Anim(ANIM_FRAME_RATE);
    for (int i = 0; i < ANIM_FRAME_COUNT; ++i) {
        ALLEGRO_BITMAP* f = al_create_sub_bitmap(
            start_sheet, i * frameW, 0, frameW, frameH
            );
        Anim.frames.push_back(f);
    }
    startAnimation = std::move(Anim);

    start_sheet = al_load_bitmap("Resource/images/stage-select/play-sheet.png");
    frameW = al_get_bitmap_width(start_sheet)/START_FRAME_COUNT;
    frameH = al_get_bitmap_height(start_sheet);
    Animation playAnim(START_FRAME_RATE);
    for (int i = 0; i < START_FRAME_COUNT; ++i) {
        ALLEGRO_BITMAP* f = al_create_sub_bitmap(
            start_sheet, i * frameW, 0, frameW, frameH
            );
        playAnim.frames.push_back(f);
    }
    playAnimation = std::move(playAnim);

    btn = new Engine::ImageButton("stage-select/back-btn.png", "stage-select/back-btn-hov.png", 274, 777, 286, 105);
    btn->SetOnClickCallback(std::bind(&BoardingScene::BackOnClick, this, 1));
    AddNewControlObject(btn);

    btn = new Engine::ImageButton("stage-select/settings-btn.png", "stage-select/settings-btn-hov.png", 210, 669, 412, 105);
    btn->SetOnClickCallback(std::bind(&BoardingScene::SettingsOnClick, this, 1));
    AddNewControlObject(btn);

    btn = new Engine::ImageButton("stage-select/start-btn.png", "stage-select/start-btn-hov.png", 267, 570, 286, 105);
    btn->SetOnClickCallback(std::bind(&BoardingScene::PlayOnClick, this, 1));
    AddNewControlObject(btn);

    btn = new Engine::ImageButton("stage-select/friendlist-btn.png", "stage-select/friendlist-btn-hov.png", w * 0.93, 63, 102, 102);
    btn->SetOnClickCallback(std::bind(&BoardingScene::FriendlistOnClick, this, 1));
    AddNewControlObject(btn);

    btn = new Engine::ImageButton("stage-select/ldb-btn.png", "stage-select/ldb-btn-hov.png", w * 0.93, 185, 102, 102);
    btn->SetOnClickCallback(std::bind(&BoardingScene::ScoreboardOnClick, this, 1));
    AddNewControlObject(btn);

    btn = new Engine::ImageButton("stage-select/logout-btn.png", "stage-select/logout-btn-hov.png", w * 0.93, 955, 102, 102);
    btn->SetOnClickCallback(std::bind(&BoardingScene::Logout, this, 1));
    AddNewControlObject(btn);


    // Not safe if release resource while playing, however we only free while change scene, so it's fine.
    // bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
    std::ifstream in("Resource/account.txt");
    in >> curUser;
    // Not safe if release resource while playing, however we only free while change scene, so it's fine.
    // bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
}

void BoardingScene::SettingsOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("settings");
}

void BoardingScene::FriendlistOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("friendlist");
}

void BoardingScene::Logout(int stage) {
    std::string curUser;
    std::ifstream in("Resource/account.txt");
    if (in.peek() != EOF) {
        in >> curUser;
        set_online(curUser, false);
    }
    std::ofstream ofs("Resource/account.txt", std::ofstream::trunc);
    Engine::GameEngine::GetInstance().ChangeScene("login");
}
void BoardingScene::Terminate() {
    IScene::Terminate();
    if (Logo) al_destroy_bitmap(Logo);
    if (PlayFont) al_destroy_font(PlayFont);
}
void BoardingScene::Draw() const {
    al_clear_to_color(al_map_rgb(255, 255, 255));

    int w = Engine::GameEngine::GetInstance().getVirtW();
    int h = Engine::GameEngine::GetInstance().getVirtH();
    int halfW = w / 2;
    int halfH = h / 2;

    if (playing) {
        // Draw play animation when game is playing
        ALLEGRO_BITMAP* bmp = playAnimation.frames[playAnimation.current];
        al_draw_scaled_bitmap(bmp, 0, 0, w, h, 0, 0, w, h, 0);
    } else {
        // Draw start animation when in menu
        ALLEGRO_BITMAP* bmp = startAnimation.frames[startAnimation.current];
        al_draw_scaled_bitmap(bmp, 0, 0, w, h, 0, 0, w, h, 0);
        Group::Draw();
        int sw = al_get_bitmap_width(Logo);
        int sh = al_get_bitmap_height(Logo);


        al_draw_tinted_scaled_bitmap(Logo, al_map_rgb_f(1, 1, 1),
            0, 0, sw, sh,
            w * 0.145, h * 0.2, sw, sh, 0);

        al_draw_text(smallFont, al_map_rgb(255, 255, 255),
            w * 0.9, h * 0.91, ALLEGRO_ALIGN_RIGHT, ("user: " + curUser).c_str());
    }


}
static bool mouseIn(int mx, int my, int x, int y, int w, int h)  {
    if (mx >= x && mx <= x + w && my >= y && my <= y + h) {
        return true;
    }
    return false;
}
void BoardingScene::Update(float deltatime) {
    IScene::Update(deltatime);

    Engine::Point mouse = Engine::GameEngine::GetInstance().GetMousePosition();

    Group::Update(deltatime);

    const int offset = 10;

    int w = Engine::GameEngine::GetInstance().getVirtW();
    int h = Engine::GameEngine::GetInstance().getVirtH();
    int startx = w * 0.2;

    int sw = al_get_bitmap_width(Logo);
    int sh = al_get_font_line_height(PlayFont) * 2 + offset;
\
    if (playing) {
        auto &A = playAnimation;
        A.timer += deltatime;
        if (A.timer >= A.frame_time) {
            A.timer -= A.frame_time;
            A.current = (A.current + 1);
            if (A.current == A.frames.size() - 1) {
                Engine::GameEngine::GetInstance().ChangeScene("play");
                return;
            }
        }
    }else {
        auto &A = startAnimation;
        A.timer += deltatime;
        if (A.timer >= A.frame_time) {
            A.timer -= A.frame_time;
            A.current = (A.current + 1) % A.frames.size();
        }
    }

    if (delayingSceneChange) {
        sceneChangeDelay -= deltatime;
        if (sceneChangeDelay <= 0) {
            Engine::GameEngine::GetInstance().ChangeScene("play");
            delayingSceneChange = false;
        }
    }
}

void BoardingScene::BackOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("start");
}
void BoardingScene::PlayOnClick(int stage) {
    AudioHelper::PlayAudio("sfx/dungtak.mp3");
    playing = true;
    playAnimation.current = 0;
    playAnimation.timer = 0;
    sceneChangeDelay = 5.0f;
    delayingSceneChange = true;
}
void BoardingScene::ScoreboardOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("leaderboard");
}
void BoardingScene::BGMSlideOnValueChanged(float value) {
    AudioHelper::ChangeSampleVolume(bgmInstance, value);
    AudioHelper::BGMVolume = value;
}
void BoardingScene::SFXSlideOnValueChanged(float value) {
    AudioHelper::SFXVolume = value;
}
