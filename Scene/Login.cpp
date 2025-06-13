#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_primitives.h>
#include <functional>
#include <memory>
#include <string>
#include <iostream>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "Login.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"
#include "Engine/Sheets.hpp"

const int MAX_NAME = 12;
const int MAX_PASS = 36;
const float APPEAR_DURATION = 3.0;
// TODO Arwen - cantikin
void LoginScene::Initialize() {
    background = al_load_bitmap("Resource/images/friendlist-scene/friendlist-bg.png");
    loadingBg = al_load_bitmap("Resource/images/friendlist-scene/loading-bg.png");

    int w = Engine::GameEngine::GetInstance().getVirtW();
    int h = Engine::GameEngine::GetInstance().getVirtH();
    int halfW = w / 2;
    int halfH = h / 2;
    elapsed = 0;
    NotFoundTimeStamp = -5;
    WrongPasswordTimeStamp = -5;
    isSigningUp = false;

    SignUpBtnHovered = false;

    AddNewObject(NotFoundLabel = new Engine::Label("Name not found", "imfell.ttf", 45, 108, 500, 130, 0, 0, 255, 0, 0));
    AddNewObject(WrongPasswordLabel = new Engine::Label("Wrong Password", "imfell.ttf", 45, 1434, 500, 130, 0, 0, 255, 0, 0));
    NotFoundLabel->Visible = false;
    WrongPasswordLabel->Visible = false;

    font = al_load_font("Resource/fonts/imfell.ttf", 40, 0);

    Engine::ImageButton *btn;

    name = "";
    pass = "";

    BoxOneClicked = true;
    BoxTwoClicked = false;

    int textheight = halfH / 3 + 50;

    BG = al_load_bitmap("Resource/images/login-scene/login-bg.png");
    //BUTTON
    btn = new Engine::ImageButton("login-scene/login-button.png", "login-scene/login-button-hov.png",
                                    halfW - 248, h * 0.9 - 90, 496,116);
    btn->SetOnClickCallback(std::bind(&LoginScene::Login, this, 1));
    AddNewControlObject(btn);


    //IMAGES

    signupMsg = al_load_bitmap("Resource/images/login-scene/signup-msg.png");
    signupText = al_load_bitmap("Resource/images/login-scene/signup-txt.png");
    signupTextHov = al_load_bitmap("Resource/images/login-scene/signup-txt-hov.png");

    // Not safe if release resource while playing, however we only free while change scene, so it's fine.
}
void LoginScene::Terminate() {
    if (font) al_destroy_font(font);
    IScene::Terminate();
}
void LoginScene::Login(int stage) {
    if (name.empty()) return;
    if (pass.empty()) return;

    int status = authUser(name, pass);
    if (status == -1) { // no such name in database
        this->RaiseNotFound();
        return;
    } if (status == 0) { // password incorrect
        this->RaiseWrongPassword();
        return;
    }
    DrawLoading(1);
    set_online(name, true);
    const int totalSteps = 10;
    for (int step = 2; step <= totalSteps; ++step) {
        al_rest(0.08);
        DrawLoading(step);
    }
    Engine::GameEngine::GetInstance().ChangeScene("boarding");
}

void LoginScene::RaiseNotFound() {
    // kalo blm punya nama di database
    NotFoundTimeStamp = elapsed;
    NotFoundLabel->Visible = true;
}
void LoginScene::RaiseWrongPassword() {
    // kalo salah password
    WrongPasswordTimeStamp = elapsed;
    WrongPasswordLabel->Visible = true;
}
bool LoginmouseIn(int mx, int my, int x, int y, int w, int h) {
    if (mx >= x && mx <= x + w && my >= y && my <= y + h) {
        return true;
    }
    return false;
}
void LoginScene::Update(float deltaTime) {
    IScene::Update(deltaTime);
    Engine::Point mouse = Engine::GameEngine::GetInstance().GetMousePosition();
    elapsed += deltaTime;

    const float y_start = 500;
    const float y_end = 518;

    int w = Engine::GameEngine::GetInstance().getVirtW();
    int h = Engine::GameEngine::GetInstance().getVirtH();
    int halfW = w / 2;
    int halfH = h / 2;
    int signupmsgSW = al_get_bitmap_width(signupMsg);
    int signupmsgSH = al_get_bitmap_height(signupMsg);
    int signuptxtSW = al_get_bitmap_width(signupText);
    int signuptxtSH = al_get_bitmap_height(signupText);
    int msgMergedDX = signupmsgSW + signuptxtSW - 50;


    if(LoginmouseIn(mouse.x, mouse.y, halfW - msgMergedDX/2 + signupmsgSW - 40 , halfH + 300, signuptxtSW, signuptxtSH)) {
        SignUpBtnHovered = true;
    }else {
        SignUpBtnHovered = false;
    }

    float dtNotFound = elapsed - NotFoundTimeStamp;
    if (dtNotFound >= 0 && dtNotFound < APPEAR_DURATION) {
        float y;
        if (dtNotFound < 1.0f) { // move down
            y = y_start + (dtNotFound / 1.0f) * (y_end - y_start); // y_start -> y_end
        } else if (dtNotFound < APPEAR_DURATION - 1.0f) { // stay
            y = y_end;
        } else if (dtNotFound < APPEAR_DURATION) { // move up
            y = y_end - ((dtNotFound - (APPEAR_DURATION - 1.0f)) / 1.0f) * (y_end - y_start); // y_end -> y_start
        }
        NotFoundLabel->Position.y = y;
        NotFoundLabel->Visible = true;
    } else {
        NotFoundLabel->Visible = false;
    }

    // Animate WrongPassword label
    float dtWrong = elapsed - WrongPasswordTimeStamp;
    if (dtWrong >= 0 && dtWrong < APPEAR_DURATION) {
        float y;
        if (dtWrong < 1.0f) { // move down
            y = y_start + (dtWrong / 1.0f) * (y_end - y_start); // y_start -> y_end
        } else if (dtWrong < APPEAR_DURATION - 1.0f) { // stay
            y = y_end;
        } else if (dtWrong < APPEAR_DURATION) { // move up
            y = y_end - ((dtWrong - (APPEAR_DURATION - 1.0f)) / 1.0f) * (y_end - y_start); // y_end -> y_start
        }
        WrongPasswordLabel->Position.y = y;
        WrongPasswordLabel->Visible = true;
    } else {
        WrongPasswordLabel->Visible = false;
    }
}

void LoginScene::Draw() const {
    al_clear_to_color(al_map_rgb(223, 145, 107));
    //IScene::Draw();
    int backSW = al_get_bitmap_width(BG);
    int backSH = al_get_bitmap_height(BG);

    //bg plg blkg
    al_draw_scaled_bitmap(BG, 0, 0, backSW, backSH, 0, 0, backSW, backSH, 0);

    Group::Draw();

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

    int signupmsgSW = al_get_bitmap_width(signupMsg);
    int signupmsgSH = al_get_bitmap_height(signupMsg);
    int signuptxtSW = al_get_bitmap_width(signupText);
    int signuptxtSH = al_get_bitmap_height(signupText);
    int msgMergedDX = signupmsgSW + signuptxtSW - 50;

    //message txt
    al_draw_tinted_scaled_bitmap(signupMsg, al_map_rgb_f(1, 1, 1),
                                0, 0, signupmsgSW, signupmsgSH,
                                halfW - msgMergedDX/2, halfH + 300, signupmsgSW, signupmsgSH, 0);
    if (SignUpBtnHovered) {
        al_draw_tinted_scaled_bitmap(signupTextHov, al_map_rgb_f(1, 1, 1), //yg bisa dipencet
                                0, 0,  signuptxtSW, signuptxtSH,
                                halfW - msgMergedDX/2 + signupmsgSW - 40 , halfH + 300, signuptxtSW, signuptxtSH, 0);

    } else {
        al_draw_tinted_scaled_bitmap(signupText, al_map_rgb_f(1, 1, 1), //yg bisa dipencet
                                0, 0,  signuptxtSW, signuptxtSH,
                                halfW - msgMergedDX/2 + signupmsgSW - 40 , halfH + 300, signuptxtSW, signuptxtSH, 0);
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

void LoginScene::DrawLoading(int step) {
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

void LoginScene::OnKeyDown(int keyCode) {
    IScene::OnKeyDown(keyCode);

    if (keyCode == ALLEGRO_KEY_LSHIFT || keyCode == ALLEGRO_KEY_RSHIFT) shiftHeld = true;
    if (keyCode == ALLEGRO_KEY_CAPSLOCK) capsLockHeld = !capsLockHeld;

    if (BoxOneClicked) {
        if (keyCode >= ALLEGRO_KEY_A && keyCode <= ALLEGRO_KEY_Z && name.size() < MAX_NAME) {
            if (shiftHeld || capsLockHeld) {
                name += keyCode - ALLEGRO_KEY_A + 'A';
            } else {
                name += keyCode - ALLEGRO_KEY_A + 'a';
            }
        }
        if (keyCode == ALLEGRO_KEY_BACKSPACE && !name.empty()) {
            name.pop_back();
        }
    } else if (BoxTwoClicked) {
        if (keyCode >= ALLEGRO_KEY_A && keyCode <= ALLEGRO_KEY_Z && pass.size() < MAX_PASS) {
            if (shiftHeld || capsLockHeld) {
                pass += keyCode - ALLEGRO_KEY_A + 'A';
            } else {
                pass += keyCode - ALLEGRO_KEY_A + 'a';
            }
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
        int status = authUser(name, pass);
        if (status == -1) { // no such name in database
            this->RaiseNotFound();
            return;
        } if (status == 0) { // password incorrect
            this->RaiseWrongPassword();
            return;
        }
        DrawLoading(1);
        set_online(name, true);
        const int totalSteps = 10;
        for (int step = 2; step <= totalSteps; ++step) {
            al_rest(0.08);
            DrawLoading(step);
        }
        Engine::GameEngine::GetInstance().ChangeScene("boarding");
    }
}

void LoginScene::OnKeyUp(int keyCode) {
    IScene::OnKeyUp(keyCode);
    if (keyCode == ALLEGRO_KEY_LSHIFT || keyCode == ALLEGRO_KEY_RSHIFT) shiftHeld = false;
}

void LoginScene::OnMouseDown(int button, int mx, int my) {
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

    int signupmsgSW = al_get_bitmap_width(signupMsg);
    int signupmsgSH = al_get_bitmap_height(signupMsg);
    int signuptxtSW = al_get_bitmap_width(signupText);
    int signuptxtSH = al_get_bitmap_height(signupText);
    int msgMergedDX = signupmsgSW + signuptxtSW - 50;

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
        if(LoginmouseIn(mouse.x, mouse.y, halfW - msgMergedDX/2 + signupmsgSW - 40 , halfH + 300, signuptxtSW, signuptxtSH)) {
            Engine::GameEngine::GetInstance().ChangeScene("signup");
        }
    }

}

void LoginScene::BackOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("start");
}
