#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_primitives.h>
#include <functional>
#include <memory>
#include <string>
#include <iostream>
#include <map>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "Signup.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"
#include "Engine/Sheets.hpp"


const int MAX_NAME = 12;
const int MAX_PASS = 36;
const float APPEAR_DURATION = 3.0;

void SignupScene::Initialize() {
    background = al_load_bitmap("Resource/images/friendlist-scene/friendlist-bg.png");
    loadingBg = al_load_bitmap("Resource/images/friendlist-scene/loading-bg.png");
    int w = Engine::GameEngine::GetInstance().getVirtW();
    int h = Engine::GameEngine::GetInstance().getVirtH();
    int halfW = w / 2;
    int halfH = h / 2;
    elapsed = 0;

    LoginBtnHov = false;
    userExistTimeStamp = -5;
    isSigningUp = true;

    font = al_load_font("Resource/fonts/imfell.ttf", 40, 0);
    AddNewObject(UserExistLabel = new Engine::Label("User already exists", "imfell.ttf", 45,  108, 500, 130, 0, 0, 255, 0, 0));

    UserExistLabel->Visible = false;

    Engine::ImageButton *btn;

    name = "";
    pass = "";

    BoxOneClicked = true;
    BoxTwoClicked = false;

    int textheight = halfH / 3 + 50;

    //BUTTON
    btn = new Engine::ImageButton("login-scene/signup-button.png", "login-scene/signup-button-hov.png",
                                  halfW - 248, h * 0.9 - 90, 496,116);
    btn->SetOnClickCallback(std::bind(&SignupScene::Signup, this, 1));
    AddNewControlObject(btn);



    BG = al_load_bitmap("Resource/images/login-scene/login-bg.png");

    loginMsg = al_load_bitmap("Resource/images/login-scene/login-msg.png");
    loginText = al_load_bitmap("Resource/images/login-scene/login-txt.png");
    logintextHov = al_load_bitmap("Resource/images/login-scene/login-txt-hov.png");

    // Not safe if release resource while playing, however we only free while change scene, so it's fine.
}

void SignupScene::Terminate() {
    if (font) al_destroy_font(font);
    IScene::Terminate();
}
void SignupScene::Signup(int stage) {
    if (name.empty()) return;
    if (pass.empty()) return;

    std::map<std::string, std::string> status = getUser(name);
    if (status["name"] == name) {
        this->RaiseExist();
        return;
    }
    const int totalSteps = 10;
    DrawLoading(1);
    createUser(name, pass);
    DrawLoading(2);
    set_online(name, true);
    DrawLoading(3);
    authUser(name, pass);
    for (int step = 4; step <= totalSteps; ++step) {
        al_rest(0.08);
        DrawLoading(step);
    }
    Engine::GameEngine::GetInstance().ChangeScene("boarding");
}

bool SignupMouseIn(int mx, int my, int x, int y, int w, int h) {
    if (mx >= x && mx <= x + w && my >= y && my <= y + h) {
        return true;
    }
    return false;
}
void SignupScene::Update(float deltaTime) {
    IScene::Update(deltaTime);
    Engine::Point mouse = Engine::GameEngine::GetInstance().GetMousePosition();
    elapsed += deltaTime;

    const float y_start = 500;
    const float y_end = 518;

    int w = Engine::GameEngine::GetInstance().getVirtW();
    int h = Engine::GameEngine::GetInstance().getVirtH();
    int halfW = w / 2;
    int halfH = h / 2;
    int loginmsgSW = al_get_bitmap_width(loginMsg);
    int loginmsgSH = al_get_bitmap_height(loginMsg);
    int logintxtSW = al_get_bitmap_width(loginText);
    int logintxtSH = al_get_bitmap_height(loginText);
    int msgMergedDX = loginmsgSW + logintxtSW - 50;


    if(SignupMouseIn(mouse.x, mouse.y, halfW - msgMergedDX/2 + loginmsgSW - 40 , halfH + 300, logintxtSW, logintxtSH)) {
        LoginBtnHov = true;
    }else {
        LoginBtnHov = false;
    }

    float dtExist = elapsed - userExistTimeStamp;
    if (dtExist >= 0 && dtExist < APPEAR_DURATION) {
        float y;
        if (dtExist < 1.0f) { // move down
            y = y_start + (dtExist / 1.0f) * (y_end - y_start); // y_start -> y_end
        } else if (dtExist < APPEAR_DURATION - 1.0f) { // stay
            y = y_end;
        } else if (dtExist < APPEAR_DURATION) { // move up
            y = y_end - ((dtExist - (APPEAR_DURATION - 1.0f)) / 1.0f) * (y_end - y_start); // y_end -> y_start
        }
        UserExistLabel->Position.y = y;
        UserExistLabel->Visible = true;
    } else {
        UserExistLabel->Visible = false;
    }
}

void SignupScene::Draw() const {
    al_clear_to_color(al_map_rgb(223, 145, 107));
    //IScene::Draw();

    ALLEGRO_COLOR clicked_color     = al_map_rgb(130, 0, 0);
    ALLEGRO_COLOR not_clicked_color = al_map_rgba(130, 0, 0, 50);
    ALLEGRO_COLOR text_color        = al_map_rgb(0, 0, 0);

    int w     = Engine::GameEngine::GetInstance().getVirtW();
    int h     = Engine::GameEngine::GetInstance().getVirtH();
    int halfW = w / 2;
    int halfH = h / 2;

    // box positions & size
    int boxW = 650;
    int boxH = 70;
    int x1   = halfW - boxW/2;
    int y1   = halfH - 40;
    int x2   = x1;
    int y2   = y1 + boxH + 100;

    int thickness = 2;


    int loginmsgSW = al_get_bitmap_width(loginMsg);
    int loginmsgSH = al_get_bitmap_height(loginMsg);
    int logintxtSW = al_get_bitmap_width(loginText);
    int logintxtSH = al_get_bitmap_height(loginText);
    int msgMergedDX = loginmsgSW + logintxtSW - 50;

    int backSW = al_get_bitmap_width(BG);
    int backSH = al_get_bitmap_height(BG);

    al_draw_scaled_bitmap(BG, 0, 0, backSW, backSH, 0, 0, backSW, backSH, 0);

    Group::Draw();

    //message txt
    al_draw_tinted_scaled_bitmap(loginMsg, al_map_rgb_f(1, 1, 1),
                                0, 0, loginmsgSW, loginmsgSH,
                                halfW - msgMergedDX/2, halfH + 300, loginmsgSW, loginmsgSH, 0);
    if (LoginBtnHov) {
        al_draw_tinted_scaled_bitmap(logintextHov, al_map_rgb_f(1, 1, 1), //yg bisa dipencet
                                0, 0,  logintxtSW, logintxtSH,
                                halfW - msgMergedDX/2 + loginmsgSW - 40 , halfH + 300, logintxtSW, logintxtSH, 0);

    } else {
        al_draw_tinted_scaled_bitmap(loginText, al_map_rgb_f(1, 1, 1), //yg bisa dipencet
                                 0, 0,  logintxtSW, logintxtSH,
                                 halfW - msgMergedDX/2 + loginmsgSW - 40 , halfH + 300, logintxtSW, logintxtSH, 0);
    }
    // draw the two boxes
    al_draw_rectangle(x1, y1, x1 + boxW, y1 + boxH,
                     BoxOneClicked ? clicked_color : not_clicked_color,
                     thickness);
    al_draw_rectangle(x2, y2, x2 + boxW, y2 + boxH,
                     BoxTwoClicked ? clicked_color : not_clicked_color,
                     thickness);

    // compute vertical center for text
    float line_h = al_get_font_line_height(font);
    float yOff   = (boxH - line_h) * 0.5f;

    // draw the raw username
    al_draw_text(
      font,
      text_color,
      x1 + 15,           // small left padding
      y1 + 5,
      0,                 // no alignment flags = left align
      name.c_str()
    );

    // build a masked password (asterisks)
    std::string masked(pass.size(), '*');
    al_draw_text(
      font,
      text_color,
      x2 + 15,
      y2 + 15,
      0,
      masked.c_str()
    );
}

void SignupScene::DrawLoading(int step) {
    ALLEGRO_DISPLAY* d = al_get_current_display();
    int W = al_get_display_width(d);
    int H = al_get_display_height(d);

    ALLEGRO_TRANSFORM old;
    al_copy_transform(&old, al_get_current_transform());
    ALLEGRO_TRANSFORM identity;
    al_identity_transform(&identity);
    al_use_transform(&identity);


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

void SignupScene::RaiseExist() {
    userExistTimeStamp = elapsed;
    UserExistLabel->Visible = true;
}


void SignupScene::OnKeyDown(int keyCode) {
    IScene::OnKeyDown(keyCode);

    if (BoxOneClicked) {
        if (keyCode >= ALLEGRO_KEY_A && keyCode <= ALLEGRO_KEY_Z && name.size() < MAX_NAME) {
            name += keyCode - ALLEGRO_KEY_A + 'a';
        }
        if (keyCode == ALLEGRO_KEY_BACKSPACE && !name.empty()) {
            name.pop_back();
        }
    } else if (BoxTwoClicked) {
        if (keyCode >= ALLEGRO_KEY_A && keyCode <= ALLEGRO_KEY_Z && pass.size() < MAX_PASS) {
            pass += keyCode - ALLEGRO_KEY_A + 'a';
        }
        if (keyCode >= ALLEGRO_KEY_0 && keyCode <= ALLEGRO_KEY_9 && name.size() < MAX_NAME) {
            pass += keyCode - ALLEGRO_KEY_0;
        }
        if (keyCode == ALLEGRO_KEY_BACKSPACE && !pass.empty()) {
            pass.pop_back();
        }
    }

    if (keyCode == ALLEGRO_KEY_ENTER) {
        if (name.empty()) return;
        if (pass.empty()) return;
        std::map<std::string, std::string> status = getUser(name);
        if (status["name"] == name) {
            this->RaiseExist();
            return;
        }
        const int totalSteps = 10;
        DrawLoading(1);
        createUser(name, pass);
        DrawLoading(2);
        set_online(name, true);
        DrawLoading(3);
        authUser(name, pass);
        for (int step = 4; step <= totalSteps; ++step) {
            al_rest(0.08);
            DrawLoading(step);
        }
        Engine::GameEngine::GetInstance().ChangeScene("boarding");
    }
}

void SignupScene::OnMouseDown(int button, int mx, int my) {
    // let any controls (buttons, textboxes, etc.) handle the click first
    IScene::OnMouseDown(button, mx, my);
    Engine::Point mouse = Engine::GameEngine::GetInstance().GetMousePosition();


    // only react on left‐click
    if (button != 1) return;

    // recompute the same geometry you used in Draw()
    int w      = Engine::GameEngine::GetInstance().getVirtW();
    int h      = Engine::GameEngine::GetInstance().getVirtH();
    int halfW = w / 2;
    int halfH  = h / 2;

    int boxW = 650;
    int boxH = 70;
    int x1   = halfW - boxW/2;
    int y1   = halfH - 40;
    int x2   = x1;
    int y2   = y1 + boxH + 100;
    int loginmsgSW = al_get_bitmap_width(loginMsg);
    int loginmsgSH = al_get_bitmap_height(loginMsg);
    int logintxtSW = al_get_bitmap_width(loginText);
    int logintxtSH = al_get_bitmap_height(loginText);
    int msgMergedDX = loginmsgSW + logintxtSW - 50;


    bool inBox1 = (mx >= x1 && mx <= x1 + boxW
                && my >= y1 && my <= y1 + boxH);
    bool inBox2 = (mx >= x2 && mx <= x2 + boxW
                && my >= y2 && my <= y2 + boxH);

    if (inBox1) {
        BoxOneClicked = true;
        BoxTwoClicked = false;
    }
    else if (inBox2) {
        BoxOneClicked = false;
        BoxTwoClicked = true;
    }
    else {
        BoxOneClicked = false;
        BoxTwoClicked = false;
    }

    if (button & 1) {
        if(SignupMouseIn(mouse.x, mouse.y, halfW - msgMergedDX/2 + loginmsgSW - 40 , halfH + 300, logintxtSW, logintxtSH)) {
            Engine::GameEngine::GetInstance().ChangeScene("login");
        }
    }

}

void SignupScene::BackOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("start");
}
