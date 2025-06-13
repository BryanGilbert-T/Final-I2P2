#ifndef CHAT_H
#define CHAT_H

#include <list>
#include <vector>
#include <string>
#include "allegro5/allegro_font.h"
#include "Engine/utility.hpp"

struct DialogueEntry {
    std::string text;
    float speakerX, speakerY;       // world coords of who’s speaking
    int speakerW, speakerH;
};


class ChatBox {
public:
    ChatBox();

    // Start a new conversation:
    void start(const std::vector<DialogueEntry>& entries);

    // Call every frame:
    void update(float dt);
    void draw(Camera cam) const;

    bool isActive() const { return active; }
    // World‐space camera target: current speaker's position
    std::pair<float,float> getCameraTarget() const;

    void OnKeyDown(int key);
    void OnMouseClick();

private:
    std::vector<DialogueEntry> queue;
    int currentIndex = 0;
    bool active = false;

    // UI layout:
    int x, y, w, h;
    ALLEGRO_FONT* font;

    // Input cooldown so you don’t skip multiple lines at once:
    float inputCooldown = 0.0f;

    void advance();
};


#endif