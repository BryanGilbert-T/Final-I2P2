//
// Created by Hsuan on 2024/4/10.
//

#ifndef INC_2025_I2P2_TOWERDEFENSE_WITH_ANSWER_INTROSCENE_H
#define INC_2025_I2P2_TOWERDEFENSE_WITH_ANSWER_INTROSCENE_H

#include <allegro5/allegro_audio.h>
#include <memory>

#include "Engine/IScene.hpp"
class IntroScene final : public Engine::IScene {
public:
    explicit IntroScene() = default;
    void Initialize() override;
    void Terminate() override;
    void PlayOnClick(int stage);
    void SettingsOnClick(int stage);
};
#endif   // INC_2025_I2P2_TOWERDEFENSE_WITH_ANSWER_INTROSCENE_H
