//
// Created by Arwen Rosetta Njoto on 10-Jun-25.
//
#include "ParallaxBackground.hpp"
#include "GameEngine.hpp"
#include "Engine/map.hpp"
#include <iostream>
#include <cmath>

namespace Engine {

    void ParallaxBackground::Initialize( const std::vector<std::string>& paths, const std::vector<float>& factors)
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

    void ParallaxBackground::Terminate() {
        for (auto* bmp : layers) {
            if (bmp) al_destroy_bitmap(bmp);
        }
        layers.clear();
    }

    void ParallaxBackground::SetLayerOffset(int layer, float dx, float dy) {
        offsetX[layer] = dx;
        offsetY[layer] = dy;
    }

    void ParallaxBackground::Draw(const Camera& cam) {
        al_clear_to_color(al_map_rgb(0, 0, 0));
        int screenW = static_cast<int>(cam.width);
        int screenH = static_cast<int>(cam.height);

        for (size_t i = 0; i < layers.size(); i++) {
            ALLEGRO_BITMAP* bmp = layers[i];
            if (!bmp) continue;

            int bw = al_get_bitmap_width(bmp);
            int bh = al_get_bitmap_height(bmp);
            float factor = parallaxFactors[i];

            // inside ParallaxBackground::Draw(...)
            float rawX = cam.x * factor + offsetX[i];
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
            float oy        = fy;


            for (float x = ox; x <= screenW + (2*bw); x += bw) {
                al_draw_bitmap(bmp, x, oy, 0);
            }

        }
    }

} // namespace Engine