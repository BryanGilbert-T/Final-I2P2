#include <allegro5/allegro_audio.h>
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
#include "search.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"

void SearchScene::Initialize() {
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
    btn = new Engine::ImageButton("friendlist-scene/friendsicon.png", "friendlist-scene/friendsicon.png", w * 0.25, h * 0.1, iconW, iconH);
    btn->SetOnClickCallback(std::bind(&SearchScene::FriendsOnClick, this, 1));
    AddNewControlObject(btn);
    btn = new Engine::ImageButton("friendlist-scene/requestsicon.png", "friendlist-scene/requestsicon.png", w * 0.5, h * 0.1, iconW, iconH);
    btn->SetOnClickCallback(std::bind(&SearchScene::FriendsOnClick, this, 2));
    AddNewControlObject(btn);
    btn = new Engine::ImageButton("friendlist-scene/searchicon.png", "friendlist-scene/searchicon.png", w * 0.75, h * 0.1, iconW, iconH);
    btn->SetOnClickCallback(std::bind(&SearchScene::FriendsOnClick, this, 3));
    AddNewControlObject(btn);

    std::ifstream in("Resource/account.txt");
    in >> curUser;
    friends = getFriends(curUser);
}
void SearchScene::FriendsOnClick(int stage) {
    if (stage == 1) {
        Engine::GameEngine::GetInstance().ChangeScene("friendlist");
    } else if (stage == 2) {
        Engine::GameEngine::GetInstance().ChangeScene("requests");
    } else if (stage == 3) {

    }
}

void SearchScene::Logout(int stage) {
    std::ofstream ofs("Resource/account.txt", std::ofstream::trunc);
    Engine::GameEngine::GetInstance().ChangeScene("login");
}
void SearchScene::Terminate() {
    AudioHelper::StopSample(bgmInstance);
    bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
    if (Logo) al_destroy_bitmap(Logo);
    if (PlayFont) al_destroy_font(PlayFont);
}
void SearchScene::Draw() const {
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

}
static bool mouseIn(int mx, int my, int x, int y, int w, int h)  {
    if (mx >= x && mx <= x + w && my >= y && my <= y + h) {
        return true;
    }
    return false;
}
void SearchScene::Update(float deltatime) {
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


}
void SearchScene::OnMouseDown(int button, int mx, int my) {
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


    }
}
void SearchScene::BackOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("start");
}
void SearchScene::PlayOnClick(int stage) {
    PlayScene *scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"));
    scene->MapId = stage;
    Engine::GameEngine::GetInstance().ChangeScene("play");
}
void SearchScene::ScoreboardOnClick() {
    Engine::GameEngine::GetInstance().ChangeScene("scoreboard-scene");
}
void SearchScene::BGMSlideOnValueChanged(float value) {
    AudioHelper::ChangeSampleVolume(bgmInstance, value);
    AudioHelper::BGMVolume = value;
}
void SearchScene::SFXSlideOnValueChanged(float value) {
    AudioHelper::SFXVolume = value;
}
