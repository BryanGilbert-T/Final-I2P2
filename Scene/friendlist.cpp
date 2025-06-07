#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_primitives.h>
#include <functional>
#include <memory>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include "Engine/AudioHelper.hpp"
#include "Engine/Point.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Collider.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "Engine/Sheets.hpp"
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

    const int iconW = 64;
    const int iconH = 64;

    friendsIcon = al_load_bitmap("Resource/images/friendlist-scene/friendsicon.png");
    requestsIcon = al_load_bitmap("Resource/images/friendlist-scene/requestsicon.png");
    searchIcon = al_load_bitmap("Resource/images/friendlist-scene/searchicon.png");

    friendsIconHover = al_load_bitmap("Resource/images/friendlist-scene/friendsicon.png");
    requestsIconHover = al_load_bitmap("Resource/images/friendlist-scene/requestsicon.png");
    searchIconHover = al_load_bitmap("Resource/images/friendlist-scene/searchicon.png");

    std::ifstream in("Resource/account.txt");
    in >> curUser;
    friends = getFriends(curUser);
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

    al_destroy_bitmap(friendsIcon);
    al_destroy_bitmap(requestsIcon);
    al_destroy_bitmap(searchIcon);
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

    int iconw = 64;
    int iconh = 64;

    for (int i = 0; i < 5; i++) {
        if (i >= friends.size()) break;
        al_draw_text(PlayFont, al_map_rgb(0, 0, 0),
            w * 0.2, h * 0.2 + i * 100, ALLEGRO_ALIGN_LEFT,
            friends[i].c_str());
    }

    if (friendsHover) { // hover nya belum ada
        al_draw_tinted_scaled_bitmap(friendsIconHover, al_map_rgb(255, 255, 255),
                    0, 0, al_get_bitmap_width(friendsIconHover), al_get_bitmap_height(friendsIconHover),
                    w * 0.25, h * 0.1, iconw, iconh,
                    0);
    } else {
        al_draw_tinted_scaled_bitmap(friendsIcon, al_map_rgb(255, 255, 255),
            0, 0, al_get_bitmap_width(friendsIcon), al_get_bitmap_height(friendsIcon),
            w * 0.25, h * 0.1, iconw, iconh,
            0);
    }

    if (requestHover) { // hover nya belum ada
        std::cout << "here";
        al_draw_tinted_scaled_bitmap(requestsIconHover, al_map_rgb(255, 255, 255),
                0, 0, al_get_bitmap_width(requestsIconHover), al_get_bitmap_height(requestsIconHover),
                w * 0.5, h * 0.1, iconw, iconh,
                0);
    } else {
        al_draw_tinted_scaled_bitmap(requestsIcon, al_map_rgb(255, 255, 255),
        0, 0, al_get_bitmap_width(requestsIcon), al_get_bitmap_height(requestsIcon),
        w * 0.5, h * 0.1, iconw, iconh,
        0);
    }

    if (searchHover) { // hover nya belum ada
        al_draw_tinted_scaled_bitmap(searchIconHover, al_map_rgb(255, 255, 255),
              0, 0, al_get_bitmap_width(searchIconHover), al_get_bitmap_height(searchIconHover),
              w * 0.75, h * 0.1, iconw, iconh,
              0);
    } else {
        al_draw_tinted_scaled_bitmap(searchIcon, al_map_rgb(255, 255, 255),
      0, 0, al_get_bitmap_width(searchIcon), al_get_bitmap_height(searchIcon),
      w * 0.75, h * 0.1, iconw, iconh,
      0);
    }

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

    const int iconw = 64;
    const int iconh = 64;

    if (mouseIn(mouse.x, mouse.y, w * 0.25, h * 0.1, iconw, iconh)) {
        friendsHover = true;
        requestHover = false;
        searchHover = false;
    }
    else if (mouseIn(mouse.x, mouse.y, w * 0.5, h * 0.1, iconw, iconh)) {
        friendsHover = false;
        requestHover = true;
        searchHover = false;
    }
    else if (mouseIn(mouse.x, mouse.y, w * 0.75, h * 0.1, iconw, iconh)) {
        friendsHover = false;
        requestHover = false;
        searchHover = true;
    }
    else {
        friendsHover = false;
        requestHover = false;
        searchHover = false;
    }
}
void FriendListScene::OnMouseDown(int button, int mx, int my) {
    IScene::OnMouseDown(button, mx, my);
    if (button & 1) {
        const int offset = 10;

        int w = Engine::GameEngine::GetInstance().getVirtW();
        int h = Engine::GameEngine::GetInstance().getVirtH();
        int startx = w * 0.2;

        int sw = al_get_bitmap_width(Logo);
        int sh = al_get_font_line_height(PlayFont) * 2 + offset;

        int logoutdx = w * 0.8 - sw / 2;
        int logoutdy = h * 0.8 - offset;

        if (friendsHover) {

        } else if (requestHover) {
            Engine::GameEngine::GetInstance().ChangeScene("requests");
        } else if (searchHover) {
            Engine::GameEngine::GetInstance().ChangeScene("search");
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
