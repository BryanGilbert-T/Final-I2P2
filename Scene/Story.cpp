//
// Created by Arwen Rosetta Njoto on 14-Jun-25.
//
#include "Story.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/AudioHelper.hpp"
#include <iostream>
#include <vector>
#include <sstream>

Story::Story(int level, float fadeSeconds, float displaySeconds)
  : level_(level)
  , fadeTime_(fadeSeconds)
  , displayTime_(displaySeconds)
  , timer_(0)
  , alpha_(0)
  , currentFrame_(0)
  , phase_(FadeIn)
{}


Story::~Story() {
    Terminate();
}

void Story::Terminate() {
    for (auto* bmp : intro)
        if (bmp) al_destroy_bitmap(bmp);
    intro.clear();
    song.clear();
}

void Story::Initialize() {
    // load all bitmaps up front
    int frameCount = 3;
    switch (level_) {
        case 1: frameCount = 3; break;
        case 2: frameCount = 0; break;
        case 3: frameCount = 0; break;
        case 4: frameCount = 4; break;
        default: frameCount = 1; break;
    }

    // Build the filepaths and load:
    for (int i = 1; i <= frameCount; ++i) {
        std::ostringstream oss;
        // assumes images named map-<level>-<i>.png
        oss << "Resource/images/story/map-"
            << level_ << "-" << i << ".png";
        auto* bmp = al_load_bitmap(oss.str().c_str());
        if (!bmp) {
            std::cerr << "Failed loading " << oss.str() << "\n";
        }
        intro.push_back(bmp);
        song.emplace_back("story/map-" + std::to_string(level_) + "-" + std::to_string(i) + ".mp3");
    }


    // reset fade state
    timer_        = 0;
    alpha_        = 0;
    currentFrame_ = 0;
    phase_        = FadeIn;
    PlaySong = true;
}

void Story::Update(float deltaTime) {
    timer_ += deltaTime;

    switch (phase_) {
        case FadeIn:
            if (PlaySong) {
                AudioHelper::PlaySample(song[currentFrame_],
                    false, AudioHelper::SFXVolume, 0);
                PlaySong = false;
            }
            alpha_ = std::min(timer_ / fadeTime_, 1.0f);
            if (timer_ >= fadeTime_) {
                phase_ = Display;
                timer_ = 0;
            }
            break;

        case Display:
            if (timer_ >= displayTime_) {
                phase_ = FadeOut;
                timer_ = 0;
            }
            break;

        case FadeOut:
            alpha_ = 1 - std::min(timer_ / fadeTime_, 1.0f);
            if (timer_ >= fadeTime_) {
                currentFrame_++;
                if (currentFrame_ >= intro.size()) {
                    if (level_ == 1) {
                        // Done → back to play
                        Engine::GameEngine::GetInstance().ChangeScene("play");
                        return;
                    }
                    if (level_ == 4) {
                        Engine::GameEngine::GetInstance().ChangeScene("intro");
                        return;
                    }
                }
                // next frame
                phase_ = FadeIn;
                timer_ = 0;
                PlaySong = true;
            }
            break;
    }
}


void Story::Draw() const {
    if (currentFrame_ >= intro.size()) return;
    // set blender for alpha
    al_draw_tinted_bitmap(
        intro[currentFrame_],
        al_map_rgba_f(1,1,1, alpha_),
        0, 0, 0
    );
}
