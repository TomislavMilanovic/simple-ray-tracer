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
        std::vector<Light> lights;
        std::vector<Material> materials;

        void render(const glm::uint16 &width, const glm::uint16 &height, const glm::uint16 &level);
    private:
        std::vector<Color> img;

        void saveppm(const std::string &filename, const glm::uint32 &width, const glm::uint32 &height);
        void savepng(const std::string &filename, const glm::uint32 &width, const glm::uint32 &height);
        Color trace(Ray &r, const glm::uint16 &lvl);
    };
}

#endif // SCENE_H