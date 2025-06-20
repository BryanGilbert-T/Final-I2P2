#ifndef FRIENDLIST_HPP
#define FRIENDLIST_HPP
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_font.h>
#include <memory>
#include <map>

#include "Engine/IScene.hpp"
#include "UI/Component/ImageButton.hpp"

class FriendListScene final : public Engine::IScene {
private:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;

public:
    const int MaxVisible = 5;
    int scrollOffset = 0;
    explicit FriendListScene() = default;
    void Initialize() override;
    void Terminate() override;
    void PlayOnClick(int stage);
    void ScoreboardOnClick();
    void BackOnClick(int stage);
    void Logout(int stage);

    void Draw() const override;
    void Update(float deltaTime) override;
    void OnMouseDown(int button, int mx, int my) override;
    void OnMouseScroll(int mx, int my, int delta) override;

    void BGMSlideOnValueChanged(float value);
    void SFXSlideOnValueChanged(float value);

    void DrawLoading(int step);

    ALLEGRO_FONT* PlayFont;
    ALLEGRO_BITMAP* Logo;

    ALLEGRO_BITMAP* background;

    std::string curUser;
    std::vector<std::string> friends;
    std::map<std::string, bool> online;

    std::vector<std::string> requests;

    bool friendsHover = false;
    bool requestHover = false;
    bool searchHover = false;

    ALLEGRO_BITMAP* decor;
    ALLEGRO_BITMAP* profile;

    ALLEGRO_BITMAP* loadingBg;

    ALLEGRO_BITMAP* friendsIcon;
    ALLEGRO_BITMAP* requestsIcon;
    ALLEGRO_BITMAP* searchIcon;

    ALLEGRO_BITMAP* friendsIconHover;
    ALLEGRO_BITMAP* requestsIconHover;
    ALLEGRO_BITMAP* searchIconHover;
};

#endif   // STAGESELECTSCENE_HPP
