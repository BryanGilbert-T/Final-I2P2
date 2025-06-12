//
// Created by Arwen Rosetta Njoto on 13-Jun-25.
//

#ifndef PARALLAXCLOUD_HPP
#define PARALLAXCLOUD_HPP

#include <allegro5/allegro.h>
#include <vector>
#include <string>


#include "Engine/utility.hpp"

namespace Engine {

    class ParallaxCloud {
    public:
        /// paths = list of file‐paths for each layer,
        /// factors = how fast each layer moves (0.0 = static, 1.0 = same as foreground)
        void Initialize(const std::vector<std::string>& paths, const std::vector<float>& factors);
        void Terminate();
        void Draw(const Camera& cam);
        void Update(float DeltaTime);
        void SetLayerOffset(int layer, float dx, float dy);

    private:
        std::vector<ALLEGRO_BITMAP*> layers;
        std::vector<float> parallaxFactors;
        std::vector<float> offsetX, offsetY;
    };
}


#endif //PARALLAXCLOUD_HPP
