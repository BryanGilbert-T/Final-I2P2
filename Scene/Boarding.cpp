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
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "Boarding.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"

void BoardingScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().getVirtW();
    int h = Engine::GameEngine::GetInstance().getVirtH();
    int halfW = w / 2;
    int halfH = h / 2;
    Engine::ImageButton *btn;

    PlayFont = al_load_font("Resource/fonts/imfell.ttf", 48, ALLEGRO_ALIGN_CENTER);
    Logo = al_load_bitmap("Resource/images/stage-select/sunwukuo-logo.png");

    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", w - 400, h * 0.9, 400, 100);
    btn->SetOnClickCallback(std::bind(&BoardingScene::Logout, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Logout", "imfell.ttf", 48, w - 200, h * 0.9 + 50, 0, 0, 0, 255, 0.5, 0.5));

    // Not safe if release resource while playing, however we only free while change scene, so it's fine.
    bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);

    // Not safe if release resource while playing, however we only free while change scene, so it's fine.
    bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
}
void BoardingScene::Logout(int stage) {
    std::ofstream ofs("Resource/account.txt", std::ofstream::trunc);
    Engine::GameEngine::GetInstance().ChangeScene("login");
}
void BoardingScene::Terminate() {
    AudioHelper::StopSample(bgmInstance);
    bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
    if (Logo) al_destroy_bitmap(Logo);
    if (PlayFont) al_destroy_font(PlayFont);
}
void BoardingScene::Draw() const {
    al_clear_to_color(al_map_rgb(255, 255, 255));
    Group::Draw();

    int w = Engine::GameEngine::GetInstance().getVirtW();
    int h = Engine::GameEngine::GetInstance().getVirtH();
    int halfW = w / 2;
    int halfH = h / 2;

    int sw = al_get_bitmap_width(Logo);
    int sh = al_get_bitmap_height(Logo);

    ALLEGRO_COLOR playcolor = (playHover) ? al_map_rgb(255, 0, 0) : al_map_rgb(0, 0, 0);
    ALLEGRO_COLOR settingcolor = (settingHover) ? al_map_rgb(255, 0, 0) : al_map_rgb(0, 0, 0);
    ALLEGRO_COLOR backcolor = (backHover) ? al_map_rgb(255, 0, 0) : al_map_rgb(0, 0, 0);

    al_draw_text(PlayFont, playcolor, w * 0.2 + sw / 2, h * 0.575, ALLEGRO_ALIGN_CENTER, "PLAY");
    al_draw_text(PlayFont, settingcolor, w * 0.2 + sw / 2, h * 0.675, ALLEGRO_ALIGN_CENTER, "SETTINGS");
    al_draw_text(PlayFont, backcolor, w * 0.2 + sw / 2, h * 0.775, ALLEGRO_ALIGN_CENTER, "BACK");


    al_draw_tinted_scaled_bitmap(Logo, al_map_rgb_f(1, 1, 1),
        0, 0, sw, sh,
        w * 0.2, h * 0.2, sw, sh, 0);
}
bool mouseIn(int mx, int my, int x, int y, int w, int h) {
    if (mx >= x && mx <= x + w && my >= y && my <= y + h) {
        return true;
    }
    return false;
}
void BoardingScene::Update(float deltatime) {
    IScene::Update(deltatime);

    Engine::Point mouse = Engine::GameEngine::GetInstance().GetMousePosition();

    const int offset = 10;

    int w = Engine::GameEngine::GetInstance().getVirtW();
    int h = Engine::GameEngine::GetInstance().getVirtH();
    int startx = w * 0.2;

    int sw = al_get_bitmap_width(Logo);
    int sh = al_get_font_line_height(PlayFont) * 2 + offset;

    if(mouseIn(mouse.x, mouse.y, startx, h * 0.575 - offset, sw, sh)) {
        playHover = true;
        settingHover = false;
        backHover = false;
    }
    else if(mouseIn(mouse.x, mouse.y, startx, h * 0.675 - offset, sw, sh)) {
        playHover = false;
        settingHover = true;
        backHover = false;
    }
    else if(mouseIn(mouse.x, mouse.y, startx, h * 0.775 - offset, sw, sh)) {
        playHover = false;
        settingHover = false;
        backHover = true;
    } else {
        playHover = false;
        settingHover = false;
        backHover = false;
    }
}
void BoardingScene::OnMouseDown(int button, int mx, int my) {
    if (button & 1) {
        const int offset = 10;

        int w = Engine::GameEngine::GetInstance().getVirtW();
        int h = Engine::GameEngine::GetInstance().getVirtH();
        int startx = w * 0.2;

        int sw = al_get_bitmap_width(Logo);
        int sh = al_get_font_line_height(PlayFont) * 2 + offset;

        if(mouseIn(mx, my, startx, h * 0.575 - offset, sw, sh)) {
            PlayScene *scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"));
            scene->MapId = 1;
            Engine::GameEngine::GetInstance().ChangeScene("play");
        }
        else if(mouseIn(mx, my, startx, h * 0.675 - offset, sw, sh)) {
            Engine::GameEngine::GetInstance().ChangeScene("settings");
        }
        else if(mouseIn(mx, my, startx, h * 0.775 - offset, sw, sh)) {
            Engine::GameEngine::GetInstance().ChangeScene("start");
        }
    }
}
void BoardingScene::BackOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("start");
}
void BoardingScene::PlayOnClick(int stage) {
    PlayScene *scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"));
    scene->MapId = stage;
    Engine::GameEngine::GetInstance().ChangeScene("play");
}
void BoardingScene::ScoreboardOnClick() {
    Engine::GameEngine::GetInstance().ChangeScene("scoreboard-scene");
}
void BoardingScene::BGMSlideOnValueChanged(float value) {
    AudioHelper::ChangeSampleVolume(bgmInstance, value);
    AudioHelper::BGMVolume = value;
}
void BoardingScene::SFXSlideOnValueChanged(float value) {
    AudioHelper::SFXVolume = value;
}
