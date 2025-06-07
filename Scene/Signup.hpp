#ifndef SIGNUPSCENE_HPP
#define SIGNUPSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <memory>

#include "Engine/IScene.hpp"

class SignupScene final : public Engine::IScene {
private:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
    bool BoxOneClicked;
    bool BoxTwoClicked;
    bool LoginBtnHov;
    std::string name;
    std::string pass;

    ALLEGRO_FONT* font;
    float elapsed;
    float userExistTimeStamp;
    ALLEGRO_BITMAP* logo;
    ALLEGRO_BITMAP* usernameText;
    ALLEGRO_BITMAP* passwordText;
    ALLEGRO_BITMAP* loginMsg;
    ALLEGRO_BITMAP* loginText;
    ALLEGRO_BITMAP* logintextHov;
    ALLEGRO_BITMAP* background;
    ALLEGRO_BITMAP* BG;

    Engine::Label* UserExistLabel;


public:
    explicit SignupScene() = default;
    void Initialize() override;
    void Terminate() override;
    void BackOnClick(int stage);
    void Draw() const override;
    void Update(float deltaTime) override;

    void Signup(int stage);
    void OnMouseDown(int button, int mx, int my) override;
    void OnKeyDown(int keyCode) override;

    void RaiseExist();
};

#endif   // STAGESELECTSCENE_HPP
