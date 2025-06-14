//
// Created by Arwen Rosetta Njoto on 14-Jun-25.
//

#ifndef STORY_HPP
#define STORY_HPP
#include <memory>

#include "Engine/IScene.hpp"
#include <allegro5/allegro_audio.h>
#include  <vector>

class Story final : public Engine::IScene {
public:
    Story(int level, float fadeSeconds = 0.5f, float displaySeconds = 2.0f);
    ~Story() override;

    void Initialize() override;
    void Terminate()  override;
    void Update(float deltaTime) override;
    void Draw()   const override;
private:
    enum Phase { FadeIn, Display, FadeOut };

    std::vector<ALLEGRO_BITMAP*> intro;
    int   level_;
    float fadeTime_;
    float displayTime_;
    float timer_;
    float alpha_;               // 0→1 during fade
    size_t currentFrame_;
    Phase  phase_;
};


#endif //STORY_HPP
