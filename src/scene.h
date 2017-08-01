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

        struct ProjectionInfo
        {
            const float fov = 100.0f;
            Vector3f orig = Vector3f(0.0f,0.0f,0.0f);
            Vector3f u = Vector3f(1.0f, 0.0f, 0.0f);
            Vector3f v = Vector3f(0.0f, 1.0f, 0.0f);
            Vector3f w = Vector3f(0.0f, 0.0f, -1.0f);
            bool isOrthogonal = false;
        };

        void addAreaLightRandom(const int &lightNum);
        void addAreaLightUniform(const float &step);

        void render(const glm::uint16 &level);

        Scene(const glm::uint16 w, const glm::uint16 h) : width(w), height(h) {}
    private:
        std::vector<Color> img;
        const glm::uint16 width, height;
        const glm::uint16 antialiasing = 1;
        const ProjectionInfo projectionInfo;

        void saveppm(const std::string &filename, const glm::uint32 &width, const glm::uint32 &height);
        void savepng(const std::string &filename, const glm::uint32 &width, const glm::uint32 &height);
        Color trace(Ray &r, const glm::uint16 &lvl);

        Color supersampling_grid_old(Ray &r, const glm::uint16 &lvl);

        Ray generate_rays(const int &y, const int &x);
        Ray generate_rays_old(const int &y, const int &x);
    };
}

#endif // SCENE_H
