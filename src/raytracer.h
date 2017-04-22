#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <vector>
#include <stdio.h>

//promijeniti pristup
#include "../lib/glm/glm.hpp"
#include "../lib/glm/gtc/constants.hpp"

namespace raytracer
{
    struct Intersection;

    typedef glm::vec3 Vector3f;
    typedef glm::vec2 Vector2f;
    typedef glm::vec3 Color;
    typedef std::vector<Intersection> intersectionList;

    inline Vector2f UV_mapping(const Vector3f& normal, const float& height, const float& width)
    {
        const float u = 0.5f + (glm::atan(normal.z, normal.x)) / (2.0f*glm::pi<float>());
        const float v = 0.5f - (glm::asin(normal.y)) / (glm::pi<float>());

        return Vector2f((int)glm::floor(u * width), (int)glm::floor(v * height));
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

        DisplacementMap(const Texture& d_map, const float& du_) : displacement_map(d_map), du(du_) {}
    protected:
        Texture displacement_map;
        float du;
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
    };


    class Light
    {
    public:
        Vector3f pos;
        Color intensity;

        Light(const Vector3f &p, const Color &intens) : pos(p) , intensity(intens){}
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
        Sphere(const Vector3f &pos, const float &rad, const Material &mat) : SolidObject(pos, mat), radius(rad) {/*empty*/}
        Sphere(const Vector3f &pos, const float &rad, const Material &mat, const SphereTextureMap &txt) : SolidObject(pos, mat), radius(rad), texture_map(&txt) {/*empty*/}

        bool intersect(const Ray &ray, intersectionList &list) const;
        bool intersect1(const Ray &ray, intersectionList &list,const glm::float32 start_t) const;
        bool intersect2(const Ray &ray, intersectionList &list) const;

        bool intersect(const Ray &ray, Intersection &intsc, const glm::float32 &radius) const;

        Material surfaceMaterial(const Vector3f &surfacePoint) const
        {
            if(texture_map == NULL)
            {
                return material;
            }
            else
            {
                const SphereTextureMap &text_map = *texture_map;
                Material currentMaterial = material;

                Vector3f normal = glm::normalize(position - surfacePoint);
                Color pointColor = text_map.getTextureMapping(normal);

                currentMaterial.setDiffuse(pointColor);
                return currentMaterial;
            }
        }

        float radius;
    private:
        const SphereTextureMap *texture_map = NULL;
        const SphereDisplacementMap *disp_map;
    };

    class Plane : public SolidObject
    {
    public:
        Plane(const Vector3f &pos, const Vector3f &n, const Material &mat) : SolidObject(pos, mat), normal(n) {/*empty*/}
        bool intersect(const Ray &ray, intersectionList &list) const;
    private:
        Vector3f normal;
    };
}

#endif // RAYTRACER_H
