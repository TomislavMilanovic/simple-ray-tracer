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
        typedef Ray (raytracer::Scene::*GenerateRaysFunc) (const int&, const int&) const;
        typedef Color (raytracer::Scene::*AntialiasingFunc) (const Ray&) const;
        typedef void (raytracer::Scene::*OutputFormatFunc) () const;

        enum ImageFormat { ppm, png };

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

            float scale;

            glm::float32 pixelHeight = 0;
            glm::float32 pixelWidth = 0;
            Vector3f scanlineStart = Vector3f();

            ProjectionInfo(const bool _isOrthogonal, const float _fov  = 100.0f) :
                fov(_fov), isOrthogonal(_isOrthogonal),
                scale(glm::tan(glm::radians(0.5f * fov))) {}

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

            void calc_cache(const glm::uint16 width, const glm::uint16 height)
            {
                scale = glm::tan(glm::radians(0.5f * fov));
                pixelHeight = (2.0f * scale) / (float)height;
                pixelWidth = (2.0f * scale) / (float)width;
                scanlineStart = orig + w
                        - ((float)width / 2.0f) * pixelWidth * u
                        + ((float)height / 2.0f) * pixelHeight * v
                        + (pixelWidth / 2.0f) * u
                        - (pixelHeight / 2.0f) * v;
            }
        };

        void addAreaLightRandom(const int &lightNum);
        void addAreaLightUniform(const float &step);

        void render();

        const glm::uint16& get_antialiasing() const { return antialiasing; }
        void set_antialiasing(const glm::uint16 aa)
        {
            antialiasing = aa;

            if(aa <= 1)
            {
                rays_func = (projectionInfo) ? &raytracer::Scene::generate_rays : &raytracer::Scene::generate_rays_old;
                aa_func = &raytracer::Scene::trace;
            }
            else
            {
                rays_func = (projectionInfo) ? &raytracer::Scene::generate_rays : &raytracer::Scene::generate_rays_old;
                aa_func = (projectionInfo) ? &raytracer::Scene::supersampling_grid : &raytracer::Scene::supersampling_grid_old;
            }
        }

        void set_image_format(const ImageFormat f)
        {
            switch(f)
            {
                case ppm:
                    output_func = &raytracer::Scene::saveppm;
                    break;
                case png:
                    output_func = &raytracer::Scene::savepng;
                    break;
                default:
                    output_func = &raytracer::Scene::savepng;
                    break;
            }
        }

        Scene(const glm::uint16 _width, const glm::uint16 _height, const glm::uint16 _level,
              const glm::uint16 _antialiasing, const std::string& _filename, const ImageFormat& _imgf,
              ProjectionInfo& _projinfo) :
            width(_width), height(_height), level(_level),
            filename(_filename), projectionInfo(&_projinfo)
        {
            projectionInfo->calc_cache(_width, _height);
            set_antialiasing(_antialiasing);
            set_image_format(_imgf);
        }

        Scene(const glm::uint16 _width, const glm::uint16 _height, const glm::uint16 _level,
              const glm::uint16 _antialiasing, const std::string& _filename, const ImageFormat& _imgf) :
            width(_width), height(_height), level(_level), filename(_filename)
        {
            set_antialiasing(_antialiasing);
            set_image_format(_imgf);
        }
    private:
        std::vector<Color> img;
        glm::uint16 width, height, level;
        glm::uint16 antialiasing;
        std::string filename;

        ProjectionInfo* projectionInfo = NULL;

        GenerateRaysFunc rays_func;
        AntialiasingFunc aa_func;
        OutputFormatFunc output_func = &raytracer::Scene::savepng;

        void saveppm() const;
        void savepng() const;

        Color trace(const Ray& ray) const;
        Color supersampling_grid_old(const Ray& r) const;
        Color supersampling_grid(const Ray& r) const;

        Ray generate_rays(const int& y, const int& x) const;
        Ray generate_rays_old(const int& y, const int& x) const;

        bool getClosestIntersection(const Ray& r, Intersection& closestIntersection) const;
    };
}

#endif // SCENE_H
