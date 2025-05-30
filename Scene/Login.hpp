#ifndef LOGINSCENE_HPP
#define LOGINSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <memory>

#include "Engine/IScene.hpp"

class LoginScene final : public Engine::IScene {
private:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
    bool BoxOneClicked;
    bool BoxTwoClicked;
    std::string name;
    std::string pass;

    ALLEGRO_FONT* font;
    float elapsed;
    float NotFoundTimeStamp;
    float WrongPasswordTimeStamp;

    Engine::Label* NotFoundLabel;
    Engine::Label* WrongPasswordLabel;

public:
    explicit LoginScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Draw() const override;
    void Update(float deltaTime) override;
    void BackOnClick(int stage);
    void Login(int stage);
    void OnMouseDown(int button, int mx, int my) override;
    void OnKeyDown(int keyCode) override;
    void RaiseNotFound();
    void RaiseWrongPassword();
};

#endif   // STAGESELECTSCENE_HPP
