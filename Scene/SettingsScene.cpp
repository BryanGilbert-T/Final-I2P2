#include <allegro5/allegro_audio.h>
#include <functional>
#include <memory>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "Scene/SettingsScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"

void SettingsScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().getVirtW();
    int h = Engine::GameEngine::GetInstance().getVirtH();
    int halfW = w / 2;
    int halfH = h / 2;

    background = al_load_bitmap("Resource/images/friendlist-scene/friendlist-bg.png");
    loadingBg = al_load_bitmap("Resource/images/friendlist-scene/loading-bg.png");
    PlayFont = al_load_font("Resource/fonts/imfell.ttf", 45, ALLEGRO_ALIGN_CENTER);
    decor = al_load_bitmap("Resource/images/friendlist-scene/decor-line.png");

    barBg = al_load_bitmap("Resource/images/stage-select/bar-bg.png");
    bgmIcon = al_load_bitmap("Resource/images/stage-select/bgm.png");
    sfxIcon = al_load_bitmap("Resource/images/stage-select/sfx.png");

    Engine::ImageButton *btn;
    btn = new Engine::ImageButton("friendlist-scene/back-btn.png", "friendlist-scene/back-btn-hov.png", halfW - 496/2, h * 0.9 - 30, 496, 112);
    btn->SetOnClickCallback(std::bind(&SettingsScene::BackOnClick, this, 1));
    AddNewControlObject(btn);

    Slider *sliderBGM, *sliderSFX;
    sliderBGM = new Slider(752 , 447, 530, 15);
    sliderBGM->SetOnValueChangedCallback(std::bind(&SettingsScene::BGMSlideOnValueChanged, this, std::placeholders::_1));
    AddNewControlObject(sliderBGM);
    sliderSFX = new Slider(752, 564, 530, 15);
    sliderSFX->SetOnValueChangedCallback(std::bind(&SettingsScene::SFXSlideOnValueChanged, this, std::placeholders::_1));
    AddNewControlObject(sliderSFX);

    sliderBGM->SetValue(AudioHelper::BGMVolume);
    sliderSFX->SetValue(AudioHelper::SFXVolume);
}
void SettingsScene::Terminate() {

    bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();

}

void SettingsScene::Draw() const {
    int w = Engine::GameEngine::GetInstance().getVirtW();
    int h = Engine::GameEngine::GetInstance().getVirtH();
    int halfW = w / 2;
    int halfH = h / 2;

    //background and decor
    al_draw_scaled_bitmap(background, 0, 0,
                            al_get_bitmap_width(background), al_get_bitmap_height(background),
                            0, 0,
                            w, h, 0);

    al_draw_scaled_bitmap(decor, 0, 0,
                        al_get_bitmap_width(decor), al_get_bitmap_height(decor),
                        117, 140, al_get_bitmap_width(decor), al_get_bitmap_height(decor), 0);

    al_draw_scaled_bitmap(decor, 0, 0,
                        al_get_bitmap_width(decor), al_get_bitmap_height(decor),
                        117, 900, al_get_bitmap_width(decor), al_get_bitmap_height(decor), 0);

    al_draw_text(PlayFont, al_map_rgb(0, 0, 0), halfW, 70, ALLEGRO_ALIGN_CENTER, "Settings");

    al_draw_scaled_bitmap(barBg, 0, 0, al_get_bitmap_width(barBg), al_get_bitmap_height(barBg), 745, 441,
        al_get_bitmap_width(barBg), al_get_bitmap_height(barBg), 0);
    al_draw_scaled_bitmap(barBg, 0, 0, al_get_bitmap_width(barBg), al_get_bitmap_height(barBg), 745, 558,
        al_get_bitmap_width(barBg), al_get_bitmap_height(barBg), 0);

    al_draw_scaled_bitmap(bgmIcon, 0, 0, al_get_bitmap_width(bgmIcon), al_get_bitmap_height(bgmIcon), 636, 427,
        al_get_bitmap_width(bgmIcon), al_get_bitmap_height(bgmIcon), 0);
    al_draw_scaled_bitmap(sfxIcon, 0, 0, al_get_bitmap_width(sfxIcon), al_get_bitmap_height(sfxIcon), 636, 544,
        al_get_bitmap_width(sfxIcon), al_get_bitmap_height(sfxIcon), 0);

    Group::Draw();
}

void SettingsScene::BackOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("boarding");
}
void SettingsScene::BGMSlideOnValueChanged(float value) {
    AudioHelper::ChangeSampleVolume(AudioHelper::currentBgm, value);
    AudioHelper::BGMVolume = value;
}
void SettingsScene::SFXSlideOnValueChanged(float value) {
    AudioHelper::SFXVolume = value;
}
