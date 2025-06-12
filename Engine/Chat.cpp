#include "Chat.hpp"
#include "fstream"
#include <allegro5/allegro_ttf.h>


Chat::Chat() {
    playfont = al_load_ttf_font("imfell.ttf", 12, 0);
}

void Chat::Init(int x) {

}

Chat::~Chat() {
    if (playfont) al_destroy_font(playfont);
}

void Chat::Draw() {
}

void Chat::Update(int deltatime) {

}
void Chat::Next() {
    if (!content.empty())
        content.pop_front();
}