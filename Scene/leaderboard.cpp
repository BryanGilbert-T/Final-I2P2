#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_primitives.h>
#include <functional>
#include <memory>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>

#include "Engine/AudioHelper.hpp"
#include "Engine/Point.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Collider.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "Engine/Sheets.hpp"
#include "PlayScene.hpp"
#include "leaderboard.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"

void LeaderboardScene::Initialize() {
    background = al_load_bitmap("Resource/images/friendlist-scene/friendlist-bg.png");
    loadingBg = al_load_bitmap("Resource/images/friendlist-scene/loading-bg.png");
    DrawLoading(1);
    int w = Engine::GameEngine::GetInstance().getVirtW();
    int h = Engine::GameEngine::GetInstance().getVirtH();
    int halfW = w / 2;
    int halfH = h / 2;
    Engine::ImageButton *btn;
    DrawLoading(2);


    PlayFont = al_load_font("Resource/fonts/imfell.ttf", 45, ALLEGRO_ALIGN_CENTER);
    Logo = al_load_bitmap("Resource/images/login-scene/logo-login.png");
    decor = al_load_bitmap("Resource/images/friendlist-scene/decor-line.png");

    DrawLoading(3);
    // Not safe if release resource while playing, however we only free while change scene, so it's fine.
    bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
    DrawLoading(4);
    // Not safe if release resource while playing, however we only free while change scene, so it's fine.
    bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
    DrawLoading(5);
    const int iconW = 87;
    const int iconH = 87;

    scrollOffset = 0;

    friendsIcon = al_load_bitmap("Resource/images/friendlist-scene/friendlist-btn.png");
    requestsIcon = al_load_bitmap("Resource/images/friendlist-scene/addfriend-btn.png");
    searchIcon = al_load_bitmap("Resource/images/friendlist-scene/search-btn.png");
    profile = al_load_bitmap("Resource/images/friendlist-scene/profile-icon.png");
    DrawLoading(6);

    std::ifstream in("Resource/account.txt");
    in >> curUser;
    ldb = getLdbData();
    DrawLoading(7);
    btn = new Engine::ImageButton("friendlist-scene/back-btn.png", "friendlist-scene/back-btn-hov.png", halfW - 496/2, h * 0.9 - 30, 496, 112);
    btn->SetOnClickCallback(std::bind(&LeaderboardScene::BackOnClick, this, 1));
    AddNewControlObject(btn);
    DrawLoading(9);
    online = find_online();
    DrawLoading(10);
}

void LeaderboardScene::Logout(int stage) {
    std::ofstream ofs("Resource/account.txt", std::ofstream::trunc);
    Engine::GameEngine::GetInstance().ChangeScene("login");
}
void LeaderboardScene::Terminate() {
    AudioHelper::StopSample(bgmInstance);
    bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
    if (Logo) al_destroy_bitmap(Logo);
    if (PlayFont) al_destroy_font(PlayFont);

    al_destroy_bitmap(friendsIcon);
    al_destroy_bitmap(requestsIcon);
    al_destroy_bitmap(searchIcon);
}
void LeaderboardScene::DrawLoading(int step) {
    ALLEGRO_DISPLAY* d = al_get_current_display();
    int W = al_get_display_width(d);
    int H = al_get_display_height(d);
    // 1) Reset transform so we're in true screen‐space
    ALLEGRO_TRANSFORM old;
    al_copy_transform(&old, al_get_current_transform());
    ALLEGRO_TRANSFORM identity;
    al_identity_transform(&identity);
    al_use_transform(&identity);

    // 2) Grab real display size

    // 3) Clear to white
    al_draw_scaled_bitmap(background, 0, 0,
                           al_get_bitmap_width(background), al_get_bitmap_height(background),
                           0, 0,
                           W, H, 0);

    // 4) Outline rectangle parameters
    const int totalSteps = 10;
    int barW =  int(W * 0.35f);
    int barH =  int(H * 0.65f);
    int x0   = (W - barW) / 2;
    int y0   = (H - barH) / 2;
    int x1   = x0 + barW;
    int y1   = y0 + barH;

    // 6) Compute segment widths & spacing
    //    inset from the border so we don’t overwrite the border
    const int inset = 4;
    float innerW    = float(barW - 2*inset);
    float innerH    = float(barH - 2*inset);

    // spacing between segments
    float spacing   = 4.0f;
    // solve: stepW * totalSteps + spacing*(totalSteps-1) = innerW
    float stepW     = (innerW - spacing*(totalSteps-1)) / totalSteps;

    step = std::clamp(step, 0, totalSteps);
    float progress = float(step) / float(totalSteps);   // 0.0 → 1.0
    float fillH    = innerH * progress;                 // how much of the bar to fill

    float xL = x0 + inset;
    float xR = x1 - inset;
    float yB = y1 - inset;
    float yT = yB - fillH;

    // 3) DRAW THE FILL **FIRST** (behind the PNG):
    al_draw_filled_rectangle(
      xL, yT,
      xR, yB,
      al_map_rgb(0, 0, 0)
    );

    // 4) NOW DRAW YOUR PNG (with the hollow monkey) on top:
    al_draw_scaled_bitmap(
      loadingBg,
      0,0,
      al_get_bitmap_width(loadingBg),
      al_get_bitmap_height(loadingBg),
      0, 0,
      W, H,
      0
    );

    // 5) flip & restore transforms…
    al_flip_display();
    al_use_transform(&old);
}
void LeaderboardScene::Draw() const {
    int w = Engine::GameEngine::GetInstance().getVirtW();
    int h = Engine::GameEngine::GetInstance().getVirtH();
    int halfW = w / 2;
    int halfH = h / 2;

    //background and decor
    al_draw_scaled_bitmap(background, 0, 0,
                            al_get_bitmap_width(background), al_get_bitmap_height(background),
                            0, 0,
                            w, h, 0);

    al_draw_scaled_bitmap(decor, 0, 0,
                        al_get_bitmap_width(decor), al_get_bitmap_height(decor),
                        117, 140, al_get_bitmap_width(decor), al_get_bitmap_height(decor), 0);

    al_draw_scaled_bitmap(decor, 0, 0,
                        al_get_bitmap_width(decor), al_get_bitmap_height(decor),
                        117, 900, al_get_bitmap_width(decor), al_get_bitmap_height(decor), 0);

    int sw = al_get_bitmap_width(Logo);
    int sh = al_get_bitmap_height(Logo);

    al_draw_text(PlayFont, al_map_rgb(0, 0, 0), halfW, 70, ALLEGRO_ALIGN_CENTER, "Leaderboard");

    int iconw = 87;
    int iconh = 87;

    if (ldb.size() == 0) {
        std::string nofriendTxt = "We are still waiting for WuKuo.";
        const int fontHeight = al_get_font_line_height(PlayFont);
        const int fontWidth = al_get_text_width(PlayFont, nofriendTxt.c_str());
        al_draw_text(PlayFont, al_map_rgb(0, 0, 0),
            halfW, halfH,
            ALLEGRO_ALIGN_CENTER, nofriendTxt.c_str());
    }

    int visible = std::min((int)ldb.size() - scrollOffset, MaxVisible);
    for (int i = 0; i < visible; ++i) {
        int idx = scrollOffset + i;
        const int offset = 36;
        const int fontHeight = al_get_font_line_height(PlayFont);
        const int starth = h * 0.27;
        const int deltah = 125;
        const int startw = w * 0.25 + 120;

        ALLEGRO_COLOR rankColor = al_map_rgb(0, 0, 0);
        if (idx + 1 == 1) {
            rankColor = al_map_rgb(0, 168, 107);
        } else if (idx + 1 == 2) {
            rankColor = al_map_rgb(255, 102, 97);
        } else if (idx + 1 == 3) {
            rankColor = al_map_rgb(255, 195, 0);
        }
        al_draw_text(PlayFont, rankColor,
        w * 0.217, starth + i * deltah - fontHeight/2 - 6,
        ALLEGRO_ALIGN_CENTER, std::to_string(idx + 1).c_str()
        );

        // background
        al_draw_filled_rounded_rectangle(
            w * 0.2 - offset, starth + i * deltah - offset,
            w * 0.8 + offset, starth + i * deltah + fontHeight + offset,
            50, 50, al_map_rgba(0, 0, 0, 50)
        );

        al_draw_scaled_bitmap(profile, 0, 0, al_get_bitmap_width(profile), al_get_bitmap_height(profile),
                                w*0.25, (starth - 10) + i * deltah - fontHeight/2,
                                al_get_bitmap_width(profile), al_get_bitmap_height(profile),0);
        // text
        al_draw_text(
            PlayFont, al_map_rgb(0,0,0),
            startw, starth + i * deltah - fontHeight/2,
            ALLEGRO_ALIGN_LEFT, ldb[idx].name.c_str()
        );

        al_draw_text(
            PlayFont, al_map_rgb(0,0,0),
            w * 0.8 - offset - 35, starth + i * deltah - fontHeight/2,
            ALLEGRO_ALIGN_LEFT, std::to_string(ldb[idx].point).c_str()
        );
    }

    int N = ldb.size();
    if (N > MaxVisible) {
        // define the top and bottom of your track in pixels
        float trackTop    = h * 0.24f;
        float trackBottom = h * 0.24f + h * 0.56f;
        float trackH      = trackBottom - trackTop;

        int visible   = MaxVisible;
        int maxOffset = N - visible;    // how many “extra” items there are

        // thumb height is proportional to fraction visible / total
        float thumbH = trackH * (float(visible) / float(N));

        // thumb Y moves from trackTop → trackBottom - thumbH
        float thumbY = trackTop
                     + (trackH - thumbH) * (float(scrollOffset) / float(maxOffset));

        // draw the track
        al_draw_filled_rectangle(
            w*0.93f, trackTop,
            w*0.94f, trackBottom,
            al_map_rgba(130, 0, 0, 50)
        );

        // draw the thumb
        al_draw_filled_rounded_rectangle(
            w*0.93f, thumbY,
            w*0.94f, thumbY + thumbH,
            10, 10,
            al_map_rgb(130, 0, 0)
            );

    }

    Group::Draw();
}
static bool mouseIn(int mx, int my, int x, int y, int w, int h)  {
    if (mx >= x && mx <= x + w && my >= y && my <= y + h) {
        return true;
    }
    return false;
}
void LeaderboardScene::Update(float deltatime) {
    IScene::Update(deltatime);

    Engine::Point mouse = Engine::GameEngine::GetInstance().GetMousePosition();

    const int offset = 10;

    int w = Engine::GameEngine::GetInstance().getVirtW();
    int h = Engine::GameEngine::GetInstance().getVirtH();
    int startx = w * 0.2;

    int sw = al_get_bitmap_width(Logo);
    int sh = al_get_font_line_height(PlayFont) * 2 + offset;

    int dxlogout = w * 0.8 - sw / 2;
    int dylogout = h * 0.8 - offset;

    const int iconw = 87;
    const int iconh = 87;


}
void LeaderboardScene::OnMouseDown(int button, int mx, int my) {
    IScene::OnMouseDown(button, mx, my);
    if (button & 1) {
        const int offset = 10;

        int w = Engine::GameEngine::GetInstance().getVirtW();
        int h = Engine::GameEngine::GetInstance().getVirtH();
        int startx = w * 0.2;

        int sw = al_get_bitmap_width(Logo);
        int sh = al_get_font_line_height(PlayFont) * 2 + offset;

        int logoutdx = w * 0.8 - sw / 2;
        int logoutdy = h * 0.8 - offset;


    }
}
void LeaderboardScene::OnMouseScroll(int mx, int my, int delta) {
    // delta > 0 means wheel up → scroll list up (i.e. show earlier friends)
    // delta < 0 means wheel down → scroll list down (show later friends)
    int maxOffset = std::max(0, (int)ldb.size() - MaxVisible);
    // subtract delta because positive delta should move the viewport up
    scrollOffset = std::clamp(scrollOffset - delta, 0, maxOffset);
}

void LeaderboardScene::BackOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("boarding");
}
void LeaderboardScene::PlayOnClick(int stage) {
    PlayScene *scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"));
    scene->MapId = stage;
    Engine::GameEngine::GetInstance().ChangeScene("play");
}
void LeaderboardScene::ScoreboardOnClick() {
    Engine::GameEngine::GetInstance().ChangeScene("scoreboard-scene");
}
void LeaderboardScene::BGMSlideOnValueChanged(float value) {
    AudioHelper::ChangeSampleVolume(bgmInstance, value);
    AudioHelper::BGMVolume = value;
}
void LeaderboardScene::SFXSlideOnValueChanged(float value) {
    AudioHelper::SFXVolume = value;
}
