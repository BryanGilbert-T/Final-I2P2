#ifndef CHAT_H
#define CHAT_H

#include <list>
#include <string>
#include "allegro5/allegro_font.h"

class Content {
public:
    std::string text;
    int x;
    int y;
};

class Chat {
private:
    std::list<Content> content;
    const int offset = 12;
    ALLEGRO_FONT* playfont;

public:
    void Init(int x);
    void Draw();
    void Update(int deltatime);
    void Next();
    Chat();
    ~Chat();
};


#endif