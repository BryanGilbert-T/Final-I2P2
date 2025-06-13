#ifndef SettingsScene_HPP
#define SettingsScene_HPP
#include <memory>

#include "Engine/IScene.hpp"
#include <allegro5/allegro_audio.h>

class SettingsScene final : public Engine::IScene {
private:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;

public:
    explicit SettingsScene() = default;
    void Initialize() override;
    void Terminate() override;
    void BackOnClick(int stage);
    void Draw() const override;
    void BGMSlideOnValueChanged(float value);
    void SFXSlideOnValueChanged(float value);
    ALLEGRO_FONT* PlayFont;

    ALLEGRO_BITMAP* background;
    ALLEGRO_BITMAP* loadingBg;
    ALLEGRO_BITMAP* decor;

    ALLEGRO_BITMAP* bgmIcon;
    ALLEGRO_BITMAP* sfxIcon;
    ALLEGRO_BITMAP* barBg;
};

#endif   // SettingsScene_HPP
