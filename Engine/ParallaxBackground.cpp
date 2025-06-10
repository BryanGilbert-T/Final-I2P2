//
// Created by Arwen Rosetta Njoto on 10-Jun-25.
//
#include "ParallaxBackground.hpp"
#include "GameEngine.hpp"
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
    }

    void ParallaxBackground::Terminate() {
        for (auto* bmp : layers) {
            if (bmp) al_destroy_bitmap(bmp);
        }
        layers.clear();
    }

    void ParallaxBackground::Draw(const Camera& cam) {
        int screenW = static_cast<int>(cam.width);
        int screenH = static_cast<int>(cam.height);

        for (size_t i = 0; i < layers.size(); i++) {
            ALLEGRO_BITMAP* bmp = layers[i];
            if (!bmp) continue;

            int bw = al_get_bitmap_width(bmp);
            int bh = al_get_bitmap_height(bmp);
            float factor = parallaxFactors[i];

            // compute “wrapped” offset
            float ox = std::fmod(cam.x * factor, bw);
            float oy = std::fmod(cam.y * factor, bh);

            if (ox > 0) ox -= bw;
            if (oy > 0) oy -= bh;

            // tile in X and Y to cover the screen
            for (float y = oy; y < screenH; y += bh) {
                for (float x = ox; x < screenW; x += bw) {
                    al_draw_bitmap(bmp, x, y, 0);
                }
            }
        }
    }

} // namespace Engine