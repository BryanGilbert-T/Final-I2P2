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
    int w = Engine::GameEngine::GetInstance().getVirtW();
    int h = Engine::GameEngine::GetInstance().getVirtH();
    int halfW = w / 2;
    int halfH = h / 2;
    elapsed = 0;
    NotFoundTimeStamp = -5;
    WrongPasswordTimeStamp = -5;

    SignUpBtnHovered = false;

    AddNewObject(NotFoundLabel = new Engine::Label("Name Not Found", "imfell.ttf", 120, halfW, -140, 255, 255, 255, 255, 0.5, 0.5));
    AddNewObject(WrongPasswordLabel = new Engine::Label("Wrong Password", "imfell.ttf", 120, halfW, -140, 255, 255, 255, 255, 0.5, 0.5));
    NotFoundLabel->Visible = false;
    WrongPasswordLabel->Visible = false;

    font = al_load_font("Resource/fonts/imfell.ttf", 40, 0);

    Engine::ImageButton *btn;

    name = "";
    pass = "";

    BoxOneClicked = true;
    BoxTwoClicked = false;

    int textheight = halfH / 3 + 50;

    //TEXT IMAGE
    usernameText = al_load_bitmap("Resource/images/login-scene/username.png");
    passwordText = al_load_bitmap("Resource/images/login-scene/password.png");

    //BUTTON
    btn = new Engine::ImageButton("login-scene/login-button.png", "login-scene/login-button-hov.png",
                                    halfW - 248, h * 0.9 - 90, 496,116);
    btn->SetOnClickCallback(std::bind(&LoginScene::Login, this, 1));
    AddNewControlObject(btn);

    bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);

    //IMAGES
    background = al_load_bitmap("Resource/images/login-scene/background.png");
    logo = al_load_bitmap("Resource/images/login-scene/logo-login.png");

    signupMsg = al_load_bitmap("Resource/images/login-scene/signup-msg.png");
    signupText = al_load_bitmap("Resource/images/login-scene/signup-txt.png");
    signupTextHov = al_load_bitmap("Resource/images/login-scene/signup-txt-hov.png");

    // Not safe if release resource while playing, however we only free while change scene, so it's fine.
    bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
}
void LoginScene::Terminate() {
    if (font) al_destroy_font(font);
    AudioHelper::StopSample(bgmInstance);
    bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
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

    const float y_start = -140;
    const float y_end = 90;

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
    IScene::Draw();

    ALLEGRO_COLOR clicked_color     = al_map_rgb(255, 255, 255);
    ALLEGRO_COLOR not_clicked_color = al_map_rgb(130, 130, 130);
    ALLEGRO_COLOR text_color        = al_map_rgb(255, 255, 255);

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

    int logoSW =al_get_bitmap_width(logo);
    int logoSH =al_get_bitmap_height(logo);
    int bgSW = al_get_bitmap_width(background); //background source width
    int bgSH = al_get_bitmap_height(background);
    int unameSW = al_get_bitmap_width(usernameText);
    int unameSH = al_get_bitmap_height(usernameText);
    int passSW = al_get_bitmap_width(passwordText);
    int passSH = al_get_bitmap_height(passwordText);
    int signupmsgSW = al_get_bitmap_width(signupMsg);
    int signupmsgSH = al_get_bitmap_height(signupMsg);
    int signuptxtSW = al_get_bitmap_width(signupText);
    int signuptxtSH = al_get_bitmap_height(signupText);
    int msgMergedDX = signupmsgSW + signuptxtSW - 50;

    //logo
    al_draw_tinted_scaled_bitmap(logo, al_map_rgb_f(1, 1, 1),
                                0, 0, logoSW, logoSH,
                                halfW - logoSW/2, 75, logoSW, logoSH, 0);

    //background
    al_draw_tinted_scaled_bitmap(background, al_map_rgb_f(1, 1, 1),
                                0, 0, bgSW, bgSH,
                                halfW - 400, halfH-165, bgSW, bgSH, 0);
    //texts
    al_draw_tinted_scaled_bitmap(usernameText, al_map_rgb_f(1, 1, 1),
                                0, 0, unameSW, unameSH,
                                halfW - unameSW/2, halfH-115, unameSW, unameSH, 0);
    al_draw_tinted_scaled_bitmap(passwordText, al_map_rgb_f(1, 1, 1),
                                0, 0, passSW, passSH,
                                halfW - passSW/2, halfH-125 + 165, unameSW, unameSH, 0);
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
void LoginScene::OnKeyDown(int keyCode) {
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
        int status = authUser(name, pass);
        if (status == -1) { // no such name in database
            this->RaiseNotFound();
            return;
        } if (status == 0) { // password incorrect
            this->RaiseWrongPassword();
            return;
        }
        Engine::GameEngine::GetInstance().ChangeScene("boarding");
    }
}

void LoginScene::OnMouseDown(int button, int mx, int my) {
    // let any controls (buttons, textboxes, etc.) handle the click first
    IScene::OnMouseDown(button, mx, my);

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
}

void LoginScene::BackOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("start");
}
