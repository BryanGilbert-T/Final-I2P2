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
    void ScoreboardOnClick();
    void BackOnClick(int stage);
    void Logout(int stage);

    void Draw() const override;
    void Update(float deltaTime) override;
    void OnMouseDown(int button, int mx, int my) override;

    void BGMSlideOnValueChanged(float value);
    void SFXSlideOnValueChanged(float value);

    bool playHover = false;
    bool settingHover = false;
    bool backHover = false;
    bool logoutHover = false;
    bool friendlistHover = false;
    bool leaderboardHover = false;


    ALLEGRO_FONT* PlayFont;
    ALLEGRO_BITMAP* Logo;
};

#endif   // STAGESELECTSCENE_HPP
