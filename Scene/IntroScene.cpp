//
// Created by Hsuan on 2024/4/10.
//

#include "IntroScene.h"
#include <allegro5/allegro_audio.h>
#include <functional>
#include <iostream>
#include <memory>
#include <ostream>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "Scene/StartScene.h"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"

// TODO HACKATHON-2 (1/3): You can imitate the 2 files: 'StartScene.hpp', 'StartScene.cpp' to implement your SettingsScene.
void IntroScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().getVirtW();
    int h = Engine::GameEngine::GetInstance().getVirtH();
    int halfW = w / 2;
    int halfH = h / 2;
    Engine::ImageButton *btn;

    cur = al_load_bitmap("Resource/images/stage-select/sunwukuo-logo.png");
    elapsedTime = 0;
}
void IntroScene::Terminate() {
    if (cur) al_destroy_bitmap(cur);
    IScene::Terminate();
}
void IntroScene::Update(float deltaTime) {
    IScene::Update(deltaTime);
    elapsedTime += deltaTime;

    if (elapsedTime > 7.0) {
        Engine::GameEngine::GetInstance().ChangeScene("start");
    }
}
void IntroScene::Draw() const {
    IScene::Draw();

    al_clear_to_color(al_map_rgb(255, 255, 255));

    const float delay = 1.0f;
    const float fade = 2.5f;
    const float fadeOut = 4.0f;
    float alpha = 0.0f;

    if      (elapsedTime < delay)           alpha = 0.0f;
    else if (elapsedTime < delay + fade)    alpha = (elapsedTime - delay) / fade;
    else if (elapsedTime < fadeOut)         alpha = 1.0f;
    else if (elapsedTime < fadeOut + fade)  alpha = 1.0f - ((elapsedTime - fadeOut) / fade);
    else                                    alpha = 0.0f;

    float scale = 1.0;

    int benW = al_get_bitmap_width(cur);
    int benH = al_get_bitmap_height(cur);
    float dstW  = benW * scale;
    float dstH  = benH * scale;

    // center on screen
    auto& eng = Engine::GameEngine::GetInstance();
    int  vw  = eng.getVirtW();
    int  vh  = eng.getVirtH();
    float x  = (vw - dstW) * 0.5f;
    float y  = (vh - dstH) * 0.5f;


    // draw tinted (white tint + alpha)
    ALLEGRO_COLOR tint = al_map_rgba_f(1, 1, 1, alpha);
    al_draw_tinted_scaled_bitmap(cur, tint,
        0, 0, benW, benH,
        x, y, dstW, dstH, 0);

    // now draw any UI you added (label, buttons…)
}
void IntroScene::PlayOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("stage-select");
}
void IntroScene::SettingsOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("settings");
}