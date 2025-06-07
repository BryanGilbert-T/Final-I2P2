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
#include "friendlist.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"

void FriendListScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().getVirtW();
    int h = Engine::GameEngine::GetInstance().getVirtH();
    int halfW = w / 2;
    int halfH = h / 2;
    Engine::ImageButton *btn;

    PlayFont = al_load_font("Resource/fonts/imfell.ttf", 48, ALLEGRO_ALIGN_CENTER);
    Logo = al_load_bitmap("Resource/images/stage-select/sunwukuo-logo.png");

    // Not safe if release resource while playing, however we only free while change scene, so it's fine.
    bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);

    // Not safe if release resource while playing, however we only free while change scene, so it's fine.
    bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
}
void FriendListScene::Logout(int stage) {
    std::ofstream ofs("Resource/account.txt", std::ofstream::trunc);
    Engine::GameEngine::GetInstance().ChangeScene("login");
}
void FriendListScene::Terminate() {
    AudioHelper::StopSample(bgmInstance);
    bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
    if (Logo) al_destroy_bitmap(Logo);
    if (PlayFont) al_destroy_font(PlayFont);
}
void FriendListScene::Draw() const {
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
    ALLEGRO_COLOR logoutcolor = (logoutHover) ? al_map_rgb(255, 0, 0) : al_map_rgb(0, 0, 0);
    ALLEGRO_COLOR friendlistcolor = (friendlistHover) ? al_map_rgb(255, 0, 0) : al_map_rgb(0, 0, 0);

    al_draw_text(PlayFont, playcolor, w * 0.2 + sw / 2, h * 0.575, ALLEGRO_ALIGN_CENTER, "PLAY");
    al_draw_text(PlayFont, settingcolor, w * 0.2 + sw / 2, h * 0.675, ALLEGRO_ALIGN_CENTER, "SETTINGS");
    al_draw_text(PlayFont, backcolor, w * 0.2 + sw / 2, h * 0.775, ALLEGRO_ALIGN_CENTER, "BACK");
    al_draw_text(PlayFont, logoutcolor, w * 0.8, h * 0.8, ALLEGRO_ALIGN_CENTER, "LOGOUT");
    al_draw_text(PlayFont, friendlistcolor, w * 0.8, h * 0.175, ALLEGRO_ALIGN_CENTER, "FRIENDLIST");


    al_draw_tinted_scaled_bitmap(Logo, al_map_rgb_f(1, 1, 1),
        0, 0, sw, sh,
        w * 0.2, h * 0.2, sw, sh, 0);
}
static bool mouseIn(int mx, int my, int x, int y, int w, int h)  {
    if (mx >= x && mx <= x + w && my >= y && my <= y + h) {
        return true;
    }
    return false;
}
void FriendListScene::Update(float deltatime) {
    IScene::Update(deltatime);

    Engine::Point mouse = Engine::GameEngine::GetInstance().GetMousePosition();

    const int offset = 10;

    int w = Engine::GameEngine::GetInstance().getVirtW();
    int h = Engine::GameEngine::GetInstance().getVirtH();
    int startx = w * 0.2;

    int sw = al_get_bitmap_width(Logo);
    int sh = al_get_font_line_height(PlayFont) * 2 + offset;

    int dxlogout = w * 0.8 - sw / 2;
    int dylogout = h * 0.8 - offset;


    if(mouseIn(mouse.x, mouse.y, startx, h * 0.575 - offset, sw, sh)) {
        playHover = true;
        settingHover = false;
        backHover = false;
        logoutHover = false;
        friendlistHover = false;
    }
    else if(mouseIn(mouse.x, mouse.y, startx, h * 0.675 - offset, sw, sh)) {
        playHover = false;
        settingHover = true;
        backHover = false;
        logoutHover = false;
        friendlistHover = false;
    }
    else if(mouseIn(mouse.x, mouse.y, startx, h * 0.775 - offset, sw, sh)) {
        playHover = false;
        settingHover = false;
        backHover = true;
        logoutHover = false;
        friendlistHover = false;
    }
    else if (mouseIn(mouse.x, mouse.y, dxlogout, dylogout, sw, sh)) {
        playHover = false;
        settingHover = false;
        backHover = false;
        logoutHover = true;
        friendlistHover = false;
    }
    else if (mouseIn(mouse.x, mouse.y, dxlogout - 20, h * 0.175 - offset, sw + 20, sh)) {
        playHover = false;
        settingHover = false;
        backHover = false;
        logoutHover = false;
        friendlistHover = true;
    }
    else {
        playHover = false;
        settingHover = false;
        backHover = false;
        logoutHover = false;
        friendlistHover = false;
    }
}
void FriendListScene::OnMouseDown(int button, int mx, int my) {
    if (button & 1) {
        const int offset = 10;

        int w = Engine::GameEngine::GetInstance().getVirtW();
        int h = Engine::GameEngine::GetInstance().getVirtH();
        int startx = w * 0.2;

        int sw = al_get_bitmap_width(Logo);
        int sh = al_get_font_line_height(PlayFont) * 2 + offset;

        int logoutdx = w * 0.8 - sw / 2;
        int logoutdy = h * 0.8 - offset;

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
        else if (mouseIn(mx, my, logoutdx, logoutdy, sw, sh)) {
            std::ofstream ofs("Resource/account.txt", std::ofstream::trunc);
            Engine::GameEngine::GetInstance().ChangeScene("login");
        }
        else if (mouseIn(mx, my, logoutdx - 20, h * 0.175 - offset, sw + 20, sh)) {
            Engine::GameEngine::GetInstance().ChangeScene("settings");
        }
    }
}
void FriendListScene::BackOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("start");
}
void FriendListScene::PlayOnClick(int stage) {
    PlayScene *scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"));
    scene->MapId = stage;
    Engine::GameEngine::GetInstance().ChangeScene("play");
}
void FriendListScene::ScoreboardOnClick() {
    Engine::GameEngine::GetInstance().ChangeScene("scoreboard-scene");
}
void FriendListScene::BGMSlideOnValueChanged(float value) {
    AudioHelper::ChangeSampleVolume(bgmInstance, value);
    AudioHelper::BGMVolume = value;
}
void FriendListScene::SFXSlideOnValueChanged(float value) {
    AudioHelper::SFXVolume = value;
}
