#ifndef SEARCH_HPP
#define SEARCH_HPP
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_font.h>
#include <memory>
#include <map>

#include "Engine/IScene.hpp"
#include "UI/Component/ImageButton.hpp"

class SearchScene final : public Engine::IScene {
private:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;

public:
    const int MaxVisible = 5;
    int scrollOffset = 0;
    explicit SearchScene() = default;
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

    ALLEGRO_FONT* PlayFont;
    ALLEGRO_BITMAP* Logo;

    ALLEGRO_BITMAP* background;

    std::string curUser;
    std::vector<std::string> friends;
    std::vector<std::string> allPlayer;
    std::map<std::string, bool> online;
    std::vector<std::string> requests;
    std::vector<std::string> pending;

    std::vector<bool> requestsHover;

    void addReq(int idx);

    bool friendsHover = false;
    bool requestHover = false;
    bool searchHover = false;

    ALLEGRO_BITMAP* friendsIcon;
    ALLEGRO_BITMAP* requestsIcon;
    ALLEGRO_BITMAP* searchIcon;

    ALLEGRO_BITMAP* friendsIconHover;
    ALLEGRO_BITMAP* requestsIconHover;
    ALLEGRO_BITMAP* searchIconHover;
};

#endif   // STAGESELECTSCENE_HPP
