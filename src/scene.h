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
        public:
            const float fov = 100.0f;
            const Vector3f orig = Vector3f();
            const Vector3f u = Vector3f(1.0f, 0.0f, 0.0f);
            const Vector3f v = Vector3f(0.0f, 1.0f, 0.0f);
            const Vector3f w = Vector3f(0.0f, 0.0f, -1.0f);
            const bool isOrthogonal = false;

            const float scale;

            const glm::float32 pixelHeight;
            const glm::float32 pixelWidth;
            const Vector3f scanlineStart;

            ProjectionInfo(const glm::uint16 width, const glm::uint16 height, const bool _isOrthogonal = false, const float _fov = 100.0f) :
                fov(_fov), isOrthogonal(_isOrthogonal),
                scale(glm::tan(glm::radians(0.5f * fov))),
                pixelHeight((2.0f * scale) / (float)height),
                pixelWidth((2.0f * scale) / (float)width),
                scanlineStart(orig + w
                        - ((float)width / 2.0f) * pixelWidth * u
                        + ((float)height / 2.0f) * pixelHeight * v
                        + (pixelWidth / 2.0f) * u
                        - (pixelHeight / 2.0f) * v){}

            ProjectionInfo(const float _fov,
                           const Vector3f _orig,
                           const Vector3f _u,
                           const Vector3f _v,
                           const Vector3f _w,
                           const bool _isOrthogonal,
                           const glm::uint16 height,
                           const glm::uint16 width
                           ) :
                fov(_fov), orig(_orig), u(_u), v(_v), w(_w),
                isOrthogonal(_isOrthogonal),
                scale(glm::tan(glm::radians(0.5f * _fov))),
                pixelHeight((2.0f * scale) / (float)height),
                pixelWidth((2.0f * scale) / (float)width),
                scanlineStart(orig + w
                        - ((float)width / 2.0f) * pixelWidth * u
                        + ((float)height / 2.0f) * pixelHeight * v
                        + (pixelWidth / 2.0f) * u
                        - (pixelHeight / 2.0f) * v){}
        };

        void addAreaLightRandom(const int &lightNum);
        void addAreaLightUniform(const float &step);

        void render(const glm::uint16 &level);

        /*Scene(const glm::uint16 _width, const glm::uint16 _height, const glm::uint16 _level,
              const glm::uint16 _antialiasing, const std::string& _filename,
              const bool _isOrthogonal = false,
              const float _fov = 100.0f,
              const Vector3f _orig = Vector3f(),
              const Vector3f _u = Vector3f(1.0f, 0.0f, 0.0f),
              const Vector3f _v = Vector3f(0.0f, 1.0f, 0.0f),
              const Vector3f _w = Vector3f(0.0f, 0.0f, -1.0f)) :
            width(_width), height(_height), level(_level), antialiasing(_antialiasing),
            filename(_filename), projectionInfo(new ProjectionInfo(_fov, _orig, _u, _v, _w, _isOrthogonal, _width, _height)){}*/

        Scene(const glm::uint16 _width, const glm::uint16 _height, const glm::uint16 _level,
              const glm::uint16 _antialiasing, const std::string& _filename,
              ProjectionInfo& _projinfo) :
            width(_width), height(_height), level(_level), antialiasing(_antialiasing),
            filename(_filename), projectionInfo(&_projinfo){}

        Scene(const glm::uint16 _width, const glm::uint16 _height, const glm::uint16 _level,
              const glm::uint16 _antialiasing, const std::string& _filename) :
            width(_width), height(_height), level(_level), antialiasing(_antialiasing),
            filename(_filename) {}
    private:
        std::vector<Color> img;
        glm::uint16 width, height, level;
        glm::uint16 antialiasing;
        std::string filename;

        ProjectionInfo* projectionInfo = NULL;

        void saveppm();
        void savepng();
        Color trace(Ray &r, const glm::uint16 &lvl);

        Color supersampling_grid_old(Ray &r, const glm::uint16 &lvl);
        Color supersampling_grid(Ray &r, const glm::uint16 &lvl);

        Ray generate_rays(const int &y, const int &x);
        Ray generate_rays_old(const int &y, const int &x);

        bool getClosestIntersection(const Ray& r, Intersection& closestIntersection);
    };
}

#endif // SCENE_H
