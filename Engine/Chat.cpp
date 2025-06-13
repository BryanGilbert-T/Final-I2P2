// ChatBox.cpp
#include "Chat.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/utility.hpp"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_image.h>

ChatBox::ChatBox()
{
    int screenW = Engine::GameEngine::GetInstance().getVirtW();
    int screenH = Engine::GameEngine::GetInstance().getVirtH();

    active = false;

    this->x = 50;
    this->y = screenH - 200;
    this->w = 400;
    this->h = 150;
    font = al_load_font("Resource/fonts/imfell.ttf", 24, 0);
}

// Begin conversation:
void ChatBox::start(const std::vector<DialogueEntry>& entries) {
    queue = entries;
    currentIndex = 0;
    active = true;
}

// Called from your game loop:
void ChatBox::update(float dt) {
    if (!active) return;

    inputCooldown = std::max(0.0f, inputCooldown - dt);
}

void ChatBox::OnKeyDown(int key) {
    if (key == ALLEGRO_KEY_F && inputCooldown <= 0) {
        advance();
        inputCooldown = 0.2f;
    }
}

void ChatBox::OnMouseClick() {
    if (inputCooldown <= 0) {
        advance();
        inputCooldown = 0.2f;
    }
}

void ChatBox::advance() {
    ++currentIndex;
    if (currentIndex >= (int)queue.size()) {
        active = false;
    }
}

void ChatBox::draw(Camera cam) const {
    if (!active) return;
    // semi-transparent black

    int screenW = Engine::GameEngine::GetInstance().getVirtW();
    int screenH = Engine::GameEngine::GetInstance().getVirtH();

    const auto& entry = queue[currentIndex];
    int dx = (entry.speakerX + screenW / 2 + 32) - cam.x - (w - entry.speakerW) / 2;
    int dy = (entry.speakerY + screenH / 2 + 32) - cam.y - h;

    al_draw_filled_rectangle(dx, dy, dx + w, dy + h, al_map_rgba_f(0, 0, 0, 0.7f));
    // border
    al_draw_rectangle(dx, dy, dx + w, dy + h, al_map_rgb(255,255,255), 2);

    al_draw_text(font, al_map_rgb(255,255,255),
                 dx + 20, dy + 20, 0, entry.text.c_str());
}

// Return where the camera should move:
std::pair<float,float> ChatBox::getCameraTarget() const {
    if (!active) return {0,0};
    const auto& e = queue[currentIndex];
    return { e.speakerX, e.speakerY };
}