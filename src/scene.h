#ifndef SCENE_H
#define SCENE_H

#include <string>
#include <vector>
#include "raytracer.h"

namespace raytracer
{
    class Scene
    {
    public:
        std::vector<SolidObject*> objects;
        std::vector<Light*> lights;

        void addAreaLightRandom(const int &lightNum);
        void addAreaLightUniform(const float &step);

        void render(const glm::uint16 &width, const glm::uint16 &height, const glm::uint16 &level);
    private:
        std::vector<Color> img;

        void saveppm(const std::string &filename, const glm::uint32 &width, const glm::uint32 &height);
        void savepng(const std::string &filename, const glm::uint32 &width, const glm::uint32 &height);
        Color trace(Ray &r, const glm::uint16 &lvl);

        Color supersampling_grid_old(const int &n, const float &y, const float &x, const glm::uint16 &level);
    };
}

#endif // SCENE_H
