#ifndef BOARDINGSCENE_HPP
#define BOARDINGSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <memory>

#include "Engine/IScene.hpp"

class BoardingScene final : public Engine::IScene {
private:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
    std::string curUser;
    ALLEGRO_FONT* smallFont;

public:
    explicit BoardingScene() = default;
    void Initialize() override;
    void Terminate() override;
    void PlayOnClick(int stage);
    void SettingsOnClick(int stage);
    void ScoreboardOnClick(int stage);
    void FriendlistOnClick(int stage);
    void BackOnClick(int stage);
    void Logout(int stage);

    void Draw() const override;
    void Update(float deltaTime) override;

    void BGMSlideOnValueChanged(float value);
    void SFXSlideOnValueChanged(float value);

    bool playing;

    ALLEGRO_FONT* PlayFont;
    ALLEGRO_BITMAP* Logo;
    ALLEGRO_BITMAP* start_sheet;
    Animation startAnimation;
    Animation playAnimation;

    float sceneChangeDelay;
    bool delayingSceneChange;
};

#endif   // STAGESELECTSCENE_HPP
