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
const std::string filename = "Resource/images/stage-select/start-sheet.png";

const int ANIM_FRAME_COUNT = 2;
const float ANIM_FRAME_RATE = 1.0f;

void BoardingScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().getVirtW();
    int h = Engine::GameEngine::GetInstance().getVirtH();
    int halfW = w / 2;
    int halfH = h / 2;
    Engine::ImageButton *btn;

    PlayFont = al_load_font("Resource/fonts/imfell.ttf", 45, ALLEGRO_ALIGN_CENTER);

    Logo = al_load_bitmap("Resource/images/stage-select/logo-white.png");
    smallFont = al_load_font("Resource/fonts/imfell.ttf", 24, ALLEGRO_ALIGN_CENTER);

    start_sheet = al_load_bitmap(filename.c_str());
    if (!start_sheet) {
        std::cerr << "Failed to load start bitmap" << std::endl;
    }
    int frameW = al_get_bitmap_width(start_sheet)/ANIM_FRAME_COUNT;
    int frameH = al_get_bitmap_height(start_sheet)/ANIM_FRAME_COUNT;
    Animation Anim(ANIM_FRAME_RATE);
    for (int i = 0; i < ANIM_FRAME_COUNT; ++i) {
        ALLEGRO_BITMAP* f = al_create_sub_bitmap(
            start_sheet, i * frameW, 0, frameW, frameH
            );
        Anim.frames.push_back(f);
    }
    animation = std::move(Anim);

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

    ALLEGRO_BITMAP* bmp = animation.frames[animation.current];

    al_draw_scaled_bitmap(bmp,
        0, 0, w,h,
        0, 0, w, h,
        0);


    Group::Draw();

    int sw = al_get_bitmap_width(Logo);
    int sh = al_get_bitmap_height(Logo);


    al_draw_tinted_scaled_bitmap(Logo, al_map_rgb_f(1, 1, 1),
        0, 0, sw, sh,
        w * 0.145, h * 0.2, sw, sh, 0);

    al_draw_text(smallFont, al_map_rgb(0, 0, 0),
        w * 0.98, h * 0.95, ALLEGRO_ALIGN_RIGHT, ("user: " + curUser).c_str());

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

    auto &A = animation;
    A.timer += deltatime;
    if (A.timer >= A.frame_time) {
        A.timer -= A.frame_time;
        A.current = (A.current + 1) % A.frames.size();
    }
}

void BoardingScene::BackOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("start");
}
void BoardingScene::PlayOnClick(int stage) {
    AudioHelper::PlayAudio("sfx/dungtak.mp3");
    PlayScene *scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"));
    Engine::GameEngine::GetInstance().ChangeScene("play");
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
