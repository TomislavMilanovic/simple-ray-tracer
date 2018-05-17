#include "raytracer.h"
#include <iostream>

#include "../lib/lodepng/lodepng.h"

namespace raytracer
{
    const Texture generate_texture(const std::string &filename)
    {
        using namespace raytracer;
        std::vector<unsigned char> buffer, image;
        lodepng::load_file(buffer, filename);

        unsigned w, h;

        unsigned error = lodepng::decode(image, w, h, buffer);

        /*if(error) std::cout << "Error while loading texture!" << error << std::endl;
        else std::cout << "Texture " << filename << " loaded." << std::endl;*/

        std::vector<Color>* new_texture = new std::vector<Color>(w * h);
        std::vector<Color> &texture = *new_texture;

        for(unsigned y = 0; y < h; ++y)
        {
            for(unsigned x = 0; x < w; ++x)
            {
              //get RGBA components
              glm::uint32 r = image[4 * y * w + 4 * x + 0]; //red
              glm::uint32 g = image[4 * y * w + 4 * x + 1]; //green
              glm::uint32 b = image[4 * y * w + 4 * x + 2]; //blue

              texture[y * w + x] = Color(r / 255.0f, g / 255.0f, b / 255.0f);
            }
        }

        return Texture(texture, w, h);
    }
}


