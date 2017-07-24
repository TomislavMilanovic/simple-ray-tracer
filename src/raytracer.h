#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <vector>
#include <stdio.h>
#include <iostream>

//promijeniti pristup
#include "../lib/glm/glm.hpp"
#include "../lib/glm/gtc/constants.hpp"
#include "../lib/glm/gtx/perpendicular.hpp"

namespace raytracer
{
    struct Intersection;

    typedef glm::vec3 Vector3f;
    typedef glm::vec2 Vector2f;
    typedef glm::vec3 Color;
    typedef std::vector<Intersection> intersectionList;

    inline Vector2f UV_mapping(const Vector3f& normal, const float& height, const float& width)
    {
        // zamijeniti + i - za ispravne teksture
        const float u = 0.5f - (glm::atan(normal.z, normal.x)) / (2.0f*glm::pi<float>());
        const float v = 0.5f - (glm::asin(normal.y)) / (glm::pi<float>());

        return Vector2f((int)glm::floor(u * width), (int)glm::floor(v * height));
    }
    inline bool isPointInSphere(const Vector3f &point, const Vector3f &position, const float &custom_radius)
    {
        if(glm::pow(point.x - position.x, 2.0f) +
           glm::pow(point.y - position.y, 2.0f) +
           glm::pow(point.z - position.z, 2.0f) <= custom_radius * custom_radius)
        {
           return true;
        }

        return false;
    }
    inline bool isPointInRectangle(const Vector3f &m, const Vector3f &a, const Vector3f &b, const Vector3f &d)
    {
        const float am_ab = glm::dot(a*m, a*b);
        const float ab_ab = glm::dot(a*b, a*b);

        const float am_ad = glm::dot(a*m, a*d);
        const float ad_ad = glm::dot(a*d, a*d);

        if((0.0f <= am_ab && am_ab <= ab_ab) && (0.0f <= am_ad && am_ad <= ad_ad))
            return true;

        return false;
    }
    inline bool isEqual(const glm::float32 &x, const glm::float32 &y)
    {
      const glm::float32 epsilon = 0.25f;
      return glm::abs(x - y) <= epsilon * glm::abs(x);
    }
    inline bool isPointBetweenTwoPoints(const Vector3f &a, const Vector3f &b, const Vector3f &c)
    {
        if(isEqual(glm::length(c - a) + glm::length(b - c), glm::length(b - a)))
        {
            return true;
        }

        return false;
    }


    class Ray
    {
    public:
        Ray() : start(Vector3f()), dir(Vector3f()) {}
        Ray(const Vector3f &st, const Vector3f &d) : start(st), dir(d) {}
        Vector3f start, dir;
    };
    class Material
    {
    public:
        const Color& diffuse() const {return _diffuse; }
        const float& reflection() const {return _reflection; }

        void setDiffuse(const Color diffuse) { _diffuse = diffuse; }
        void setReflection(const float reflection) { _reflection = reflection; }

        Material(const Color &d, const float &refl) : _diffuse(d), _reflection(refl)  {}
    private:
        Color _diffuse;
        float _reflection;
    };
    class Texture
    {
    public:
        const unsigned& width() const {return w;}
        const unsigned& height() const {return h;}
        const Color& texture(const unsigned &x, const unsigned &y) const {return t[y * w + x];}
        Texture(const std::vector<Color> &t, const unsigned &w, const unsigned &h) :w(w), h(h), t(t) {}
    private:
        const unsigned w, h;
        const std::vector<Color> &t;
    };
    class TextureMap
    {
    public:
        virtual Color getTextureMapping(Vector3f& point) const = 0;

        TextureMap(const Texture& t_map) : texture_map(t_map) {}
    //protected:
        Texture texture_map;
    };
    class DisplacementMap
    {
    public:
        virtual float getDispMapping(Vector3f& point) const = 0;

        const float& get_max_displacement() const {return max_displacement;}
        const float& get_du() const {return du;}

        DisplacementMap(const Texture& d_map, const float& du_) : displacement_map(d_map), du(du_), max_displacement(calculate_max_displacement()) {}
    protected:
        Texture displacement_map;
        const float du;
        const float max_displacement;
    private:
        float calculate_max_displacement() const
        {
            float max_disp = 0.0f;

            for(unsigned y = 0; y < displacement_map.height(); ++y)
            {
                for(unsigned x = 0; x < displacement_map.width(); ++x)
                {
                    const Color& map = displacement_map.texture(x, y);
                    float df = 0.3f * map.r + 0.59f * map.g + 0.11f * map.b;

                    if(df > max_disp) max_disp = df;
                 }
            }

            return max_disp;
        }
    };

    class SphereTextureMap : public TextureMap
    {
    public:
        Color getTextureMapping(Vector3f& normal) const
        {
            const Vector2f &UV = UV_mapping(normal, texture_map.height(), texture_map.width());
            return texture_map.texture(UV.x, UV.y);
        }

        SphereTextureMap(const Texture& t_map) : TextureMap(t_map) {}
    };

    class SphereDisplacementMap : public DisplacementMap
    {
    public:
        float getDispMapping(Vector3f& point) const
        {
            const Vector2f &UV = UV_mapping(-point, displacement_map.height(), displacement_map.width());
            const Color &color = displacement_map.texture(UV.x, UV.y);
            const float df = 0.3f * (color.r) + 0.59f * (color.g) + 0.11f * (color.b);
            return df * du;
        }

        SphereDisplacementMap(const Texture& d_map, const float& du_) : DisplacementMap(d_map, du_) {}
    };

    class Light
    {
    public:
        virtual Vector3f getDirection(const Vector3f &surfacePoint) const = 0;
        virtual Color getIntensity(const Vector3f &surfacePoint) const = 0;
    };
    class PointLight : public Light
    {
    public:
        Vector3f getDirection(const Vector3f &surfacePoint) const
        {
            return pos - surfacePoint;
        }
        Color getIntensity(const Vector3f &surfacePoint) const
        {
            return lightColor * intensity;
        }
        PointLight(const Vector3f &p, const Color &col, const glm::float32 &intens) : pos(p) , lightColor(col), intensity(intens){}
    protected:
        Vector3f pos;
        Color lightColor;
        glm::float32 intensity;
    };
    class RealisticPointLight : public PointLight
    {
    public:
        Color getIntensity(const Vector3f &surfacePoint) const
        {
            const float r2 = glm::pow(glm::length(getDirection(surfacePoint)), 1.0f);
            return (lightColor * intensity) / (4.0f * glm::pi<float>() * r2);
        }
        RealisticPointLight(const Vector3f &p, const Color &col, const glm::float32 &intens) : PointLight(p, col, intens){}
    };
    class SolidObject
    {
    public:
         SolidObject(const Vector3f &pos, const Material &mat) : position(pos), material(mat) {}
         virtual bool intersect(const Ray &ray, intersectionList &list) const = 0;
         virtual Material surfaceMaterial(const Vector3f &surfacePoint) const { return material; }
         Vector3f position;
    protected:
         Material material;
    };
    struct Intersection
    {
        glm::float32 distance;
        Vector3f point;
        Vector3f normal;
        const SolidObject *solid;
    };
    class Sphere : public SolidObject
    {
    public:
        Sphere(const Vector3f &pos, const float &rad, const Material &mat) : SolidObject(pos, mat), radius(rad)
        {
            intersect_func = &raytracer::Sphere::normal_intersect_wrapper;
            material_func = &raytracer::Sphere::get_normal_material;
        }
        Sphere(const Vector3f &pos, const float &rad, const Material &mat, const SphereTextureMap &txt) : SolidObject(pos, mat), radius(rad), texture_map(&txt)
        {
            intersect_func = &raytracer::Sphere::normal_intersect_wrapper;
            material_func = &raytracer::Sphere::get_texture_material;
        }
        Sphere(const Vector3f &pos, const float &rad, const Material &mat, const SphereDisplacementMap &disp) : SolidObject(pos, mat), radius(rad), displacement_map(&disp)
        {
            intersect_func = &raytracer::Sphere::disp_mapping_intersect_wrapper;
            material_func = &raytracer::Sphere::get_normal_material;
        }
        Sphere(const Vector3f &pos, const float &rad, const Material &mat, const SphereTextureMap &txt, const SphereDisplacementMap &disp) : SolidObject(pos, mat), radius(rad), texture_map(&txt), displacement_map(&disp)
        {
            intersect_func = &raytracer::Sphere::disp_mapping_intersect_wrapper;
            material_func = &raytracer::Sphere::get_texture_material;
        }

        bool intersect(const Ray &ray, intersectionList &list) const;

        Material surfaceMaterial(const Vector3f &surfacePoint) const;

        float radius;
    private:     
        bool normal_intersect(const Ray &ray, Intersection &intsc, const glm::float32 &custom_radius) const;
        bool disp_mapping_intersect(const Ray &ray, intersectionList &list,const glm::float32 start_t) const;

        bool normal_intersect_wrapper(const Ray &ray, intersectionList &list) const;
        bool disp_mapping_intersect_wrapper(const Ray &ray, intersectionList &list) const;

        Material get_normal_material(const Vector3f &surfacePoint) const;
        Material get_texture_material(const Vector3f &surfacePoint) const;

        typedef bool (raytracer::Sphere::*IntersectFunc) (const Ray&, intersectionList&) const;
        IntersectFunc intersect_func;

        typedef Material (raytracer::Sphere::*MaterialFunc) (const Vector3f&) const;
        MaterialFunc material_func;

        const SphereTextureMap *texture_map = NULL;
        const SphereDisplacementMap *displacement_map = NULL;
    };

    class Plane : public SolidObject
    {
    public:
        Plane(const Vector3f &pos, const Vector3f &n, const Material &mat) : SolidObject(pos, mat), normal(n) {/*empty*/}
        bool intersect(const Ray &ray, intersectionList &list) const;
    private:
        Vector3f normal;
    };

    class Triangle : public SolidObject
    {
    public:
        Triangle(const Vector3f &_v0, const Vector3f &_v1, const Vector3f &_v2, const Material &_mat) : SolidObject(Vector3f(0.0f,0.0f,0.0f), _mat), v0(_v0), v1(_v1), v2(_v2) {}
        bool intersect(const Ray &ray, intersectionList &list) const;
    private:
        const Vector3f v0;
        const Vector3f v1;
        const Vector3f v2;
    };
}

#endif // RAYTRACER_H
