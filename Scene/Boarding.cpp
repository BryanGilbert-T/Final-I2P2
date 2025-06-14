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

void BoardingScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().getVirtW();
    int h = Engine::GameEngine::GetInstance().getVirtH();
    int halfW = w / 2;
    int halfH = h / 2;
    Engine::ImageButton *btn;

    PlayFont = al_load_font("Resource/fonts/imfell.ttf", 48, ALLEGRO_ALIGN_CENTER);
    Logo = al_load_bitmap("Resource/images/stage-select/sunwukuo-logo.png");
    smallFont = al_load_font("Resource/fonts/imfell.ttf", 24, ALLEGRO_ALIGN_CENTER);

    // Not safe if release resource while playing, however we only free while change scene, so it's fine.
    // bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
    std::ifstream in("Resource/account.txt");
    in >> curUser;
    // Not safe if release resource while playing, however we only free while change scene, so it's fine.
    // bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
}
void BoardingScene::Logout(int stage) {

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
    ALLEGRO_COLOR leaderboardcolor = (leaderboardHover) ? al_map_rgb(255, 0, 0) : al_map_rgb(0, 0, 0);

    al_draw_text(PlayFont, playcolor, w * 0.2 + sw / 2, h * 0.575, ALLEGRO_ALIGN_CENTER, "PLAY");
    al_draw_text(PlayFont, settingcolor, w * 0.2 + sw / 2, h * 0.675, ALLEGRO_ALIGN_CENTER, "SETTINGS");
    al_draw_text(PlayFont, backcolor, w * 0.2 + sw / 2, h * 0.775, ALLEGRO_ALIGN_CENTER, "BACK");
    al_draw_text(PlayFont, logoutcolor, w * 0.8, h * 0.8, ALLEGRO_ALIGN_CENTER, "LOGOUT");
    al_draw_text(PlayFont, friendlistcolor, w * 0.8, h * 0.175, ALLEGRO_ALIGN_CENTER, "FRIENDLIST");
    al_draw_text(PlayFont, leaderboardcolor, w * 0.8, h * 0.275, ALLEGRO_ALIGN_CENTER, "LEADERBOARD");

    al_draw_tinted_scaled_bitmap(Logo, al_map_rgb_f(1, 1, 1),
        0, 0, sw, sh,
        w * 0.2, h * 0.2, sw, sh, 0);

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
        leaderboardHover = false;
    }
    else if(mouseIn(mouse.x, mouse.y, startx, h * 0.675 - offset, sw, sh)) {
        playHover = false;
        settingHover = true;
        backHover = false;
        logoutHover = false;
        friendlistHover = false;
        leaderboardHover = false;
    }
    else if(mouseIn(mouse.x, mouse.y, startx, h * 0.775 - offset, sw, sh)) {
        playHover = false;
        settingHover = false;
        backHover = true;
        logoutHover = false;
        friendlistHover = false;
        leaderboardHover = false;
    }
    else if (mouseIn(mouse.x, mouse.y, dxlogout, dylogout, sw, sh)) {
        playHover = false;
        settingHover = false;
        backHover = false;
        logoutHover = true;
        friendlistHover = false;
        leaderboardHover = false;
    }
    else if (mouseIn(mouse.x, mouse.y, dxlogout - 20, h * 0.175 - offset, sw + 20, sh)) {
        playHover = false;
        settingHover = false;
        backHover = false;
        logoutHover = false;
        friendlistHover = true;
        leaderboardHover = false;
    }
    else if (mouseIn(mouse.x, mouse.y, dxlogout - 20, h * 0.275 - offset, sw + 20, sh)) {
        playHover = false;
        settingHover = false;
        backHover = false;
        logoutHover = false;
        friendlistHover = false;
        leaderboardHover = true;
    }
    else {
        playHover = false;
        settingHover = false;
        backHover = false;
        logoutHover = false;
        friendlistHover = false;
        leaderboardHover = false;
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

        int logoutdx = w * 0.8 - sw / 2;
        int logoutdy = h * 0.8 - offset;

        if(mouseIn(mx, my, startx, h * 0.575 - offset, sw, sh)) {
            AudioHelper::PlayAudio("sfx/dungtak.mp3");
            PlayScene *scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"));
            Engine::GameEngine::GetInstance().ChangeScene("play");
        }
        else if(mouseIn(mx, my, startx, h * 0.675 - offset, sw, sh)) {
            Engine::GameEngine::GetInstance().ChangeScene("settings");
        }
        else if(mouseIn(mx, my, startx, h * 0.775 - offset, sw, sh)) {
            Engine::GameEngine::GetInstance().ChangeScene("start");
        }
        else if (mouseIn(mx, my, logoutdx, logoutdy, sw, sh)) {
            std::string curUser;
            std::ifstream in("Resource/account.txt");
            if (in.peek() != EOF) {
                in >> curUser;
                set_online(curUser, false);
            }
            std::ofstream ofs("Resource/account.txt", std::ofstream::trunc);
            Engine::GameEngine::GetInstance().ChangeScene("login");
        }
        else if (mouseIn(mx, my, logoutdx - 20, h * 0.175 - offset, sw + 20, sh)) {
            Engine::GameEngine::GetInstance().ChangeScene("friendlist");
        }
        else if (mouseIn(mx, my, logoutdx - 20, h * 0.275 - offset, sw + 20, sh)) {
            Engine::GameEngine::GetInstance().ChangeScene("leaderboard");
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
