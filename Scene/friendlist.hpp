#ifndef FRIENDLIST_HPP
#define FRIENDLIST_HPP
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_font.h>
#include <memory>

#include "Engine/IScene.hpp"
#include "UI/Component/ImageButton.hpp"

class FriendListScene final : public Engine::IScene {
private:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;

public:
    explicit FriendListScene() = default;
    void Initialize() override;
    void Terminate() override;
    void PlayOnClick(int stage);
    void ScoreboardOnClick();
    void BackOnClick(int stage);
    void Logout(int stage);
    void FriendsOnClick(int stage);

    void Draw() const override;
    void Update(float deltaTime) override;
    void OnMouseDown(int button, int mx, int my) override;

    void BGMSlideOnValueChanged(float value);
    void SFXSlideOnValueChanged(float value);

    ALLEGRO_FONT* PlayFont;
    ALLEGRO_BITMAP* Logo;

    ALLEGRO_BITMAP* background;

    std::string curUser;
    std::vector<std::string> friends;

    Engine::ImageButton* friendsIcon;
    Engine::ImageButton* requestsIcon;
    Engine::ImageButton* searchIcon;
};

#endif   // STAGESELECTSCENE_HPP
