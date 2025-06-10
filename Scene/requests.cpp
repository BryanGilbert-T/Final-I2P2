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
#include "requests.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"

void RequestsScene::Initialize() {
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
    Logo = al_load_bitmap("Resource/images/stage-select/sunwukuo-logo.png");
    decor = al_load_bitmap("Resource/images/friendlist-scene/decor-line.png");

    DrawLoading(3);
    // Not safe if release resource while playing, however we only free while change scene, so it's fine.
    bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
    DrawLoading(4);
    // Not safe if release resource while playing, however we only free while change scene, so it's fine.
    bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);

    DrawLoading(5);
    const int iconW = 64;
    const int iconH = 64;

    scrollOffset = 0;

    friendsIcon = al_load_bitmap("Resource/images/friendlist-scene/friendlist-btn.png");
    requestsIcon = al_load_bitmap("Resource/images/friendlist-scene/addfriend-btn.png");
    searchIcon = al_load_bitmap("Resource/images/friendlist-scene/search-btn.png");
    profile = al_load_bitmap("Resource/images/friendlist-scene/profile-icon.png");

    DrawLoading(6);
    std::ifstream in("Resource/account.txt");
    in >> curUser;
    friends = getRequests(curUser);
    DrawLoading(7);

    btn = new Engine::ImageButton("friendlist-scene/back-btn.png", "friendlist-scene/back-btn-hov.png", halfW - 496/2, h * 0.9 - 30, 496, 112);
    btn->SetOnClickCallback(std::bind(&RequestsScene::BackOnClick, this, 1));
    AddNewControlObject(btn);

    online = find_online();
    DrawLoading(9);

    checkIcon = al_load_bitmap("Resource/images/friendlist-scene/accept-btn.png");
    crossIcon = al_load_bitmap("Resource/images/friendlist-scene/reject-btn.png");

    crossHover.assign(MaxVisible, false);
    checkHover.assign(MaxVisible, false);
    DrawLoading(10);
}

void RequestsScene::Logout(int stage) {
    std::ofstream ofs("Resource/account.txt", std::ofstream::trunc);
    Engine::GameEngine::GetInstance().ChangeScene("login");
}
void RequestsScene::Terminate() {
    AudioHelper::StopSample(bgmInstance);
    bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
    if (Logo) al_destroy_bitmap(Logo);
    if (PlayFont) al_destroy_font(PlayFont);

    al_destroy_bitmap(friendsIcon);
    al_destroy_bitmap(requestsIcon);
    al_destroy_bitmap(searchIcon);
    al_destroy_bitmap(checkIcon);
    al_destroy_bitmap(crossIcon);
}
void RequestsScene::Draw() const {
    al_clear_to_color(al_map_rgb(255, 255, 255));

    int w = Engine::GameEngine::GetInstance().getVirtW();
    int h = Engine::GameEngine::GetInstance().getVirtH();
    int halfW = w / 2;
    int halfH = h / 2;


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

    int iconw = 87;
    int iconh = 87;

    al_draw_text(PlayFont, al_map_rgb(0, 0, 0), 117, 70, ALLEGRO_ALIGN_LEFT, "Friend Request");

    if (friends.size() == 0) {
        std::string nofriendTxt = "No requests.";
        al_draw_text(PlayFont, al_map_rgb(0, 0, 0),
            halfW, halfH,
            ALLEGRO_ALIGN_CENTER, nofriendTxt.c_str());
    }

    int visible = std::min((int)friends.size() - scrollOffset, MaxVisible);
    for (int i = 0; i < visible; ++i) {
        int idx = scrollOffset + i;
        const int offset = 36;
        const int fontHeight = al_get_font_line_height(PlayFont);
        const int starth = h * 0.27;
        const int deltah = 125;
        const int startw = w * 0.25 + 120;

        // background
        al_draw_filled_rounded_rectangle(
           w * 0.2 - offset, starth + i * deltah - offset,
           w * 0.8 + offset, starth + i * deltah + fontHeight + offset,
           50, 50, al_map_rgba(0, 0, 0, 50)
       );
        // text
        al_draw_filled_circle(w * 0.22, starth + i * deltah + (fontHeight / 2),
          25/2, (online.find(friends[idx])->second) ? al_map_rgb(46, 76, 12) : al_map_rgb(0, 0, 0));

        al_draw_scaled_bitmap(profile, 0, 0, al_get_bitmap_width(profile), al_get_bitmap_height(profile),
                                w*0.25, (starth - 10) + i * deltah - fontHeight/2,
                                al_get_bitmap_width(profile), al_get_bitmap_height(profile),0);
        al_draw_text(
            PlayFont, al_map_rgb(0,0,0),
            startw, starth + i * deltah - fontHeight/2,
            ALLEGRO_ALIGN_LEFT, friends[idx].c_str()
        );

        // cross icon
        // check icon
        const int ofs = 36;
        const int iconSize    = 48;    // desired width & height
        const int iconPadding = 24;    // space from right edge
        const int rectR   = w * 0.8  + ofs;
        const int entryY  = starth + i*deltah;

        // 2) center‐vertically on the entry
        float iconY = entryY + (fontHeight/2) - (iconSize/2);

        // 3) position the two icons flush‐right
        float crossX = rectR - iconPadding - iconSize;
        float checkX = crossX   - iconPadding - iconSize;

        ALLEGRO_COLOR checkTint = checkHover[i]
            ? al_map_rgba(255,255,255,255)
            : al_map_rgba(255,255,255,100);
        ALLEGRO_COLOR crossTint = crossHover[i]
            ? al_map_rgba(255,255,255,255)
            : al_map_rgba(255,255,255,100);

        // 4) draw them scaled into a square
        //    you can also use al_draw_tinted_scaled_bitmap if you need tinting
        al_draw_tinted_scaled_bitmap(
            checkIcon, checkTint,
            0,0,
            al_get_bitmap_width(checkIcon),
            al_get_bitmap_height(checkIcon),
            crossX, iconY,
            iconSize, iconSize,
            0
        );
        al_draw_tinted_scaled_bitmap(
            crossIcon, crossTint,
            0,0,
            al_get_bitmap_width(crossIcon),
            al_get_bitmap_height(crossIcon),
            checkX, iconY,
            iconSize, iconSize,
            0
        );

    }

    if (friendsHover) { // hover nya belum ada
        al_draw_tinted_scaled_bitmap(friendsIcon, al_map_rgba(255, 255, 255, 255),
                    0, 0, al_get_bitmap_width(friendsIcon), al_get_bitmap_height(friendsIcon),
                    halfW-iconw/2 - 100 - iconw, 35, iconw, iconh,
                    0);
    } else {
        al_draw_tinted_scaled_bitmap(friendsIcon, al_map_rgba(255, 255, 255, 100),
            0, 0, al_get_bitmap_width(friendsIcon), al_get_bitmap_height(friendsIcon),
            halfW-iconw/2 - 100 - iconw, 35, iconw, iconh,
            0);
    }

    // notification for requests
    if (friends.size() > 0) {
        al_draw_text(PlayFont,
            al_map_rgb(130, 0, 0),
            w * 0.5 + iconw/2, h * 0.05, ALLEGRO_ALIGN_LEFT,
            std::to_string(friends.size()).c_str());
    }
    if (requestHover) { // hover nya belum ada
        al_draw_tinted_scaled_bitmap(requestsIcon, al_map_rgba(255, 255, 255, 255),
                0, 0, al_get_bitmap_width(requestsIcon), al_get_bitmap_height(requestsIcon),
                halfW- iconw/2, 35, iconw, iconh,
                0);
    } else {
        al_draw_tinted_scaled_bitmap(requestsIcon, al_map_rgba(255, 255, 255, 255),
        0, 0, al_get_bitmap_width(requestsIcon), al_get_bitmap_height(requestsIcon),
        w * 0.5 - iconw/2, 35, iconw, iconh,
        0);
    }

    if (searchHover) { // hover nya belum ada
        al_draw_tinted_scaled_bitmap(searchIcon, al_map_rgba(255, 255, 255, 255),
              0, 0, al_get_bitmap_width(searchIcon), al_get_bitmap_height(searchIcon),
              halfW+iconw/2 + 100, 35, iconw, iconh,
              0);
    } else {
        al_draw_tinted_scaled_bitmap(searchIcon, al_map_rgba(255, 255, 255, 100),
      0, 0, al_get_bitmap_width(searchIcon), al_get_bitmap_height(searchIcon),
      halfW+iconw/2 + 100, 35, iconw, iconh,
      0);
    }

    int N = friends.size();
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
void RequestsScene::Update(float deltatime) {
    IScene::Update(deltatime);

    Engine::Point mouse = Engine::GameEngine::GetInstance().GetMousePosition();

    std::fill(checkHover.begin(),  checkHover.end(),  false);
    std::fill(crossHover.begin(), crossHover.end(), false);

    // your constants (must match Draw):
    const int ofs         = 36;
    const int fontH       = al_get_font_line_height(PlayFont);
    const float starth    = Engine::GameEngine::GetInstance().getVirtH() * 0.27f;
    const int deltah      = 125;
    const float rectR     = Engine::GameEngine::GetInstance().getVirtW() * 0.8f + ofs;
    const int iconSize    = 48;
    const int iconPadding = 24;

    int visible = std::min((int)friends.size() - scrollOffset, MaxVisible);
    for (int i = 0; i < visible; ++i) {
        float entryY = starth + i * deltah;
        float iconY  = entryY + (fontH/2.f) - (iconSize/2.f);
        float crossX = rectR - iconPadding - iconSize;
        float checkX = crossX  - iconPadding - iconSize;

        if (mouseIn(mouse.x, mouse.y, (int)crossX, (int)iconY, iconSize, iconSize))
            checkHover[i] = true;
        if (mouseIn(mouse.x, mouse.y, (int)checkX, (int)iconY, iconSize, iconSize))
            crossHover[i] = true;
    }

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

    if (mouseIn(mouse.x, mouse.y, w/2 - iconw/2 - 100 - iconw, 35, iconw, iconh)) {
        friendsHover = true;
        requestHover = false;
        searchHover = false;
    }
    else if (mouseIn(mouse.x, mouse.y, w/2 - iconw/2, 35, iconw, iconh)) {
        friendsHover = false;
        requestHover = true;
        searchHover = false;
    }
    else if (mouseIn(mouse.x, mouse.y, w/2+iconw/2+100, 35, iconw, iconh)) {
        friendsHover = false;
        requestHover = false;
        searchHover = true;
    }
    else {
        friendsHover = false;
        requestHover = false;
        searchHover = false;
    }
}
void RequestsScene::DrawLoading(int step) {
    // 1) Reset transform so we're in true screen‐space
    ALLEGRO_TRANSFORM old;
    al_copy_transform(&old, al_get_current_transform());
    ALLEGRO_TRANSFORM identity;
    al_identity_transform(&identity);
    al_use_transform(&identity);

    // 2) Grab real display size
    ALLEGRO_DISPLAY* disp = al_get_current_display();
    int W = al_get_display_width(disp);
    int H = al_get_display_height(disp);

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
void RequestsScene::onCheckClicked(int idx) {
    idx += scrollOffset;
    std::cout << friends[idx] << std::endl;
    addFriend(friends[idx], curUser);
    RemoveRequests(curUser, friends[idx]);
    friends.erase(friends.begin() + idx);
    removePending(friends[idx], curUser);
}
void RequestsScene::onCrossClicked(int idx) {
    idx += scrollOffset;
    std::cout << friends[idx] << std::endl;
    RemoveRequests(curUser, friends[idx]);
    friends.erase(friends.begin() + idx);
    removePending(friends[idx], curUser);
}

void RequestsScene::OnMouseDown(int button, int mx, int my) {
    IScene::OnMouseDown(button, mx, my);
    if (button & 1) {
        const int ofs         = 36;
        const int fontH       = al_get_font_line_height(PlayFont);
        const float starth    = Engine::GameEngine::GetInstance().getVirtH() * 0.27f;
        const int deltah      = 125;
        const float rectR     = Engine::GameEngine::GetInstance().getVirtW() * 0.8f + ofs;
        const int iconSize    = 48;
        const int iconPadding = 24;

        int visible = std::min((int)friends.size() - scrollOffset, MaxVisible);
        for (int i = 0; i < visible; ++i) {
            float entryY = starth + i * deltah;
            float iconY  = entryY + (fontH/2.f) - (iconSize/2.f);
            float crossX = rectR - iconPadding - iconSize;
            float checkX = crossX  - iconPadding - iconSize;

            if (checkHover[i])   { onCheckClicked(i);   break; }
            if (crossHover[i])   { onCrossClicked(i);   break; }
        }

        const int offset = 10;

        int w = Engine::GameEngine::GetInstance().getVirtW();
        int h = Engine::GameEngine::GetInstance().getVirtH();
        int startx = w * 0.2;

        int sw = al_get_bitmap_width(Logo);
        int sh = al_get_font_line_height(PlayFont) * 2 + offset;

        int logoutdx = w * 0.8 - sw / 2;
        int logoutdy = h * 0.8 - offset;

        if (friendsHover) {
            Engine::GameEngine::GetInstance().ChangeScene("friendlist");
        } else if (requestHover) {
        } else if (searchHover) {
            Engine::GameEngine::GetInstance().ChangeScene("search");
        }

    }
}
void RequestsScene::OnMouseScroll(int mx, int my, int delta) {
    // delta > 0 means wheel up → scroll list up (i.e. show earlier friends)
    // delta < 0 means wheel down → scroll list down (show later friends)
    int maxOffset = std::max(0, (int)friends.size() - MaxVisible);
    // subtract delta because positive delta should move the viewport up
    scrollOffset = std::clamp(scrollOffset - delta, 0, maxOffset);
}

void RequestsScene::BackOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("boarding");
}
void RequestsScene::PlayOnClick(int stage) {
    PlayScene *scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"));
    scene->MapId = stage;
    Engine::GameEngine::GetInstance().ChangeScene("play");
}
void RequestsScene::ScoreboardOnClick() {
    Engine::GameEngine::GetInstance().ChangeScene("scoreboard-scene");
}
void RequestsScene::BGMSlideOnValueChanged(float value) {
    AudioHelper::ChangeSampleVolume(bgmInstance, value);
    AudioHelper::BGMVolume = value;
}
void RequestsScene::SFXSlideOnValueChanged(float value) {
    AudioHelper::SFXVolume = value;
}
