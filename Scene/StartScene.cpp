//
// Created by Hsuan on 2024/4/10.
//

#include "StartScene.h"
#include <allegro5/allegro_audio.h>
#include <functional>
#include <memory>
#include <cmath>
#include <string>
#include <iostream>
#include <fstream>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "Scene/StartScene.h"

#include <allegro5/allegro_ttf.h>

#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"

// TODO Arwen : Leaderboard and Setting Scene
void StartScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().getVirtW();
    int h = Engine::GameEngine::GetInstance().getVirtH();
    int halfW = w / 2;
    int halfH = h / 2;
    Engine::ImageButton *btn;
    elapsed = 0.0;

    font = al_load_ttf_font("Resource/fonts/imfell.ttf", 24, 0);
    if (!font) {
        std::cout<<"ERROR: failed to load imfell.ttf\n";
        std::exit(1);
    }

    cur = al_load_bitmap("Resource/images/stage-select/boarding-bg.png");
    //AddNewObject(new Engine::Label("Sun Wu Kuo", "pirulen.ttf", 120, halfW, halfH / 3 + 50, 10, 255, 255, 255, 0.5, 0.5));
    clicktxt = al_load_bitmap("Resource/images/stage-select/click-text.png");
    clickbg = al_load_bitmap("Resource/images/stage-select/text-background.png");
}
void StartScene::Update(float deltaTime) {
    //if (cur) al_destroy_bitmap(cur);
    IScene::Update(deltaTime);
    elapsed += deltaTime;
}
void StartScene::Draw() const {
    IScene::Draw();
    al_clear_to_color(al_map_rgb(255, 244, 226));
    auto& eng = Engine::GameEngine::GetInstance();
    int w = eng.getVirtW();
    int h = eng.getVirtH();
    int halfW = w / 2;
    int halfH = h / 2;
    float freq = 0.5f;


    int benW = al_get_bitmap_width(cur);
    int benH = al_get_bitmap_height(cur);
    int txtW = al_get_bitmap_width(clicktxt);
    int txtH = al_get_bitmap_height(clicktxt);
    int bgW = al_get_bitmap_width(clickbg);
    int bgH = al_get_bitmap_height(clickbg);

    int  vw  = eng.getVirtW();
    int  vh  = eng.getVirtH();

    float scaleX = float(vw) / benW;
    float scaleY = float(vh) / benH;
    float scale = std::min(scaleX, scaleY);

    float dstW  = benW * scale;
    float dstH  = benH * scale;

    //auto& eng = Engine::GameEngine::GetInstance();

    float x  = (vw - dstW) * 0.5f;
    float y  = (vh - dstH) * 0.5f;

    float base = (std::sin(elapsed * 2 * M_PI * freq) + 1) * 0.5f;
    float alpha = 0.4f + 0.6f * base;

    al_draw_tinted_scaled_bitmap(cur, al_map_rgb(255,255,255),
        0, 0, benW, benH,
        x, y, dstW, dstH, 0);


    ALLEGRO_COLOR tint = al_map_rgba_f(1,1,1,alpha);
    // al_draw_text(font, tint, w/2, h*0.9, ALLEGRO_ALIGN_CENTER,
    //     "CLICK TO BEGIN");

    al_draw_tinted_scaled_bitmap(clicktxt, tint, 0, 0,
                                txtW, txtH, halfW-txtW/2, h*0.9 - 20,
                                txtW, txtH, 0);



}
bool is_empty(const std::string& path) {
    std::ifstream f(path);
    if (!f) throw std::runtime_error("Cannot open file");
    return f.peek() == std::ifstream::traits_type::eof();
}

void StartScene::OnKeyDown(int keyCode) {
    IScene::OnKeyDown(keyCode);
    // if (keyCode == ALLEGRO_KEY_ENTER) return;
    // if (is_empty("Resource/account.txt")) {
    //     Engine::GameEngine::GetInstance().ChangeScene("login");
    // } else {
    //     Engine::GameEngine::GetInstance().ChangeScene("boarding");
    // }
}

void StartScene::Terminate() {
    if (cur) {
        al_destroy_bitmap(cur);
        cur = nullptr;
    }
    IScene::Terminate();
}
void StartScene::OnMouseDown(int button, int mx, int my) {
    IScene::OnMouseDown(button, mx, my);
    if (is_empty("Resource/account.txt")) {
        Engine::GameEngine::GetInstance().ChangeScene("login");
    } else {
        Engine::GameEngine::GetInstance().ChangeScene("boarding");
    }
}
void StartScene::PlayOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("stage-select");
}
void StartScene::SettingsOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("settings");
}