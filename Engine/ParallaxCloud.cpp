//
// Created by Arwen Rosetta Njoto on 13-Jun-25.
//
#include "ParallaxCloud.hpp"
#include "GameEngine.hpp"
#include "Engine/map.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>

namespace Engine {

    void ParallaxCloud::Initialize( const std::vector<std::string>& paths, const std::vector<float>& factors)
    {

        layers.clear();
        parallaxFactors = factors;

        for (auto& p : paths) {
            ALLEGRO_BITMAP* bmp = al_load_bitmap(p.c_str());
            if (!bmp) {
                std::cerr << "Failed to load parallax layer: " << p << std::endl;
            }
            layers.push_back(bmp);
        }

        offsetX.assign(layers.size(), 0);
        offsetY.assign(layers.size(), 0);

    }

    void ParallaxCloud::Terminate() {
        for (auto* bmp : layers) {
            if (bmp) al_destroy_bitmap(bmp);
        }
        layers.clear();
    }

    void ParallaxCloud::SetLayerOffset(int layer, float dx, float dy) {
        if (layer < 0 || layer >= (int)offsetX.size()) return;
        offsetX[layer] = dx;
        offsetY[layer] = dy;
    }

    void ParallaxCloud::Update(float DeltaTime) {
        const float baseSpeed =  75.0f;
        for (size_t i = 0; i < layers.size(); i++) {
            // Slide each layer horizontally by its parallax factor
            offsetX[i] += parallaxFactors[i] * DeltaTime * baseSpeed;
        }
    }

    void ParallaxCloud::Draw(const Camera& cam) {
        int screenW = static_cast<int>(cam.width);
        int screenH = static_cast<int>(cam.height);

        for (size_t i = 0; i < layers.size(); i++) {
            ALLEGRO_BITMAP* bmp = layers[i];
            if (!bmp) continue;

            int bw = al_get_bitmap_width(bmp);
            int bh = al_get_bitmap_height(bmp);

            float factor = parallaxFactors[i];

            float rawX = offsetX[i];
            float wrapX = std::fmod(rawX, bw);
            if (wrapX < 0) wrapX += bw;
            float ox = -wrapX;

            PlayScene *scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"));
            //scene->MapId = stage;
            int BLOCK_SIZE = 64;
            int mapH = scene->MapHeight * BLOCK_SIZE;

            float baselineY = mapH - (1.5 * 1080);     // where the bottom of the bitmap lines up with bottom of screen
            float camJump   = std::min(cam.y, float(screenH)); // only care when cam.y < 0 (you’ve jumped up)
            float vertFactor = parallaxFactors[i];
            float fy         = baselineY - (camJump * vertFactor);
            float oy        = fy + offsetY[i];

            for (float x = ox; x <= screenW + (2*bw); x += bw) {
                al_draw_bitmap(bmp, x, oy, 0);
            }

        }
    }

} // namespace Engine