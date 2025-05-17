#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_primitives.h>
#include <functional>
#include <memory>
#include <string>

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
// TODO Arwen
void LoginScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().getVirtW();
    int h = Engine::GameEngine::GetInstance().getVirtH();
    int halfW = w / 2;
    int halfH = h / 2;

    font = al_load_font("Resource/fonts/pirulen.ttf", 72, 0);

    Engine::ImageButton *btn;

    name = "";
    pass = "";

    BoxOneClicked = true;
    BoxTwoClicked = false;

    int textheight = halfH / 3 + 50;

    AddNewObject(new Engine::Label("User:", "pirulen.ttf", 120, 150, textheight, 10, 255, 255, 255, 0, 0.5));
    AddNewObject(new Engine::Label("Pass:", "pirulen.ttf", 120, 150, textheight + 120, 10, 255, 255, 255, 0, 0.5));

    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, h * 0.9, 400, 100);
    btn->SetOnClickCallback(std::bind(&LoginScene::Login, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Login", "pirulen.ttf", 48, halfW, h * 0.9 + 50, 0, 0, 0, 255, 0.5, 0.5));
    bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);

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
        return;
    } else if (status == 0) { // password incorrect
        return;
    }
    Engine::GameEngine::GetInstance().ChangeScene("boarding");
}
void LoginScene::Draw() const {
    IScene::Draw();

    ALLEGRO_COLOR clicked_color     = al_map_rgb(255, 255, 255);
    ALLEGRO_COLOR not_clicked_color = al_map_rgb(130, 130, 130);
    ALLEGRO_COLOR text_color        = al_map_rgb(255, 255, 255);

    int w     = Engine::GameEngine::GetInstance().getVirtW();
    int h     = Engine::GameEngine::GetInstance().getVirtH();
    int halfH = h / 2;

    // box positions & size
    int x1   = 150 + 500;
    int y1   = halfH / 3;
    int x2   = x1;
    int y2   = y1 + 120;
    int boxW = w - 100 - x1;
    int boxH = 100;
    int thickness = 5;

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
      x1 + 10,           // small left padding
      y1 + yOff,
      0,                 // no alignment flags = left align
      name.c_str()
    );

    // build a masked password (asterisks)
    std::string masked(pass.size(), '*');
    al_draw_text(
      font,
      text_color,
      x2 + 10,
      y2 + yOff,
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
        if (keyCode == ALLEGRO_KEY_BACKSPACE) {
            name.pop_back();
        }
    } else if (BoxTwoClicked) {
        if (keyCode >= ALLEGRO_KEY_A && keyCode <= ALLEGRO_KEY_Z && pass.size() < MAX_PASS) {
            pass += keyCode - ALLEGRO_KEY_A + 'a';
        }
        if (keyCode >= ALLEGRO_KEY_0 && keyCode <= ALLEGRO_KEY_9 && name.size() < MAX_NAME) {
            pass += keyCode - ALLEGRO_KEY_0;
        }
        if (keyCode == ALLEGRO_KEY_BACKSPACE) {
            pass.pop_back();
        }
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
    int halfH  = h / 2;

    int x1     = 150 + 500;
    int y1     = halfH / 3;
    int boxW   = w - 100 - x1;   // from x1 to (w - 100)
    int boxH   = 100;            // height of each box

    int x2     = x1;
    int y2     = y1 + 120;

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
