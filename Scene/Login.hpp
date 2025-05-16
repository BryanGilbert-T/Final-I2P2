#ifndef LOGINSCENE_HPP
#define LOGINSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <memory>

#include "Engine/IScene.hpp"

class LoginScene final : public Engine::IScene {
private:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;

public:
    explicit LoginScene() = default;
    void Initialize() override;
    void Terminate() override;
    void PlayOnClick(int stage);
    void ScoreboardOnClick();
    void BackOnClick(int stage);

    void BGMSlideOnValueChanged(float value);
    void SFXSlideOnValueChanged(float value);
};

#endif   // STAGESELECTSCENE_HPP
