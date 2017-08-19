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
            const Vector3f orig = Vector3f(0.0f,0.0f,0.0f);
            const Vector3f u = Vector3f(1.0f, 0.0f, 0.0f);
            const Vector3f v = Vector3f(0.0f, 1.0f, 0.0f);
            const Vector3f w = Vector3f(0.0f, 0.0f, -1.0f);

            const float scale = glm::tan(glm::radians(0.5f * fov));

            glm::float32 pixelHeight;
            glm::float32 pixelWidth;
            Vector3f scanlineStart;

            const bool isOrthogonal = false;
        };

        void addAreaLightRandom(const int &lightNum);
        void addAreaLightUniform(const float &step);

        void render(const glm::uint16 &level);

        Scene(const glm::uint16 w, const glm::uint16 h) : width(w), height(h)
        {
            projectionInfo.pixelHeight =  (2.0f * projectionInfo.scale) / (float)height;
            projectionInfo.pixelWidth = (2.0f * projectionInfo.scale) / (float)width;

            projectionInfo.scanlineStart = projectionInfo.orig + projectionInfo.w
                    - ((float)width / 2.0f) * projectionInfo.pixelWidth * projectionInfo.u
                    + ((float)height / 2.0f) * projectionInfo.pixelHeight * projectionInfo.v
                    + (projectionInfo.pixelWidth / 2.0f) * projectionInfo.u
                    - (projectionInfo.pixelHeight / 2.0f) * projectionInfo.v;
        }
    private:
        std::vector<Color> img;
        const glm::uint16 width, height;
        const glm::uint16 antialiasing = 1;
        ProjectionInfo projectionInfo;

        void saveppm(const std::string &filename, const glm::uint32 &width, const glm::uint32 &height);
        void savepng(const std::string &filename, const glm::uint32 &width, const glm::uint32 &height);
        Color trace(Ray &r, const glm::uint16 &lvl);

        Color supersampling_grid_old(Ray &r, const glm::uint16 &lvl);
        Color supersampling_grid(Ray &r, const glm::uint16 &lvl);

        Ray generate_rays(const int &y, const int &x);
        Ray generate_rays_old(const int &y, const int &x);

        bool getClosestIntersection(const Ray& r, Intersection& closestIntersection);
    };
}

#endif // SCENE_H
