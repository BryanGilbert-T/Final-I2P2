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

    font = al_load_font("Resource/fonts/pirulen.ttf", 24, 0);
    if (!font) {
        std::cout<<"ERROR: failed to load pirulen.ttf\n";
        std::exit(1);
    }


    AddNewObject(new Engine::Label("Sun Wu Kuo", "pirulen.ttf", 120, halfW, halfH / 3 + 50, 10, 255, 255, 255, 0.5, 0.5));

}
void StartScene::Update(float deltaTime) {
    IScene::Update(deltaTime);
    elapsed += deltaTime;
}
void StartScene::Draw() const {
    IScene::Draw();
    auto& eng = Engine::GameEngine::GetInstance();
    int w = eng.getVirtW();
    int h = eng.getVirtH();
    float freq = 0.5f;

    float base = (std::sin(elapsed * 2 * M_PI * freq) + 1) * 0.5f;
    float alpha = 0.4f + 0.6f * base;
    ALLEGRO_COLOR tint = al_map_rgba_f(1,1,1,alpha);
    al_draw_text(font, tint, w/2, h*0.9, ALLEGRO_ALIGN_CENTER,
        "Press any key");
}
bool is_empty(const std::string& path) {
    std::ifstream f(path);
    if (!f) throw std::runtime_error("Cannot open file");
    return f.peek() == std::ifstream::traits_type::eof();
}

void StartScene::OnKeyDown(int keyCode) {
    IScene::OnKeyDown(keyCode);
    if (keyCode == ALLEGRO_KEY_ENTER) return;
    if (is_empty("Resource/account.txt")) {
        Engine::GameEngine::GetInstance().ChangeScene("login");
    } else {
        Engine::GameEngine::GetInstance().ChangeScene("boarding");
    }
}

void StartScene::Terminate() {
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