#ifndef RAYTRACER_H
#define RAYTRACER_H

//promijeniti pristup
#include "../lib/glm/glm.hpp"

namespace raytracer
{
    typedef glm::vec3 Vector3f;
    typedef glm::vec3 Color;

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
        Color diffuse;
        float reflection;

        Material(const Color &d, const float &refl) : diffuse(d), reflection(refl) {}
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
         virtual bool intersect(const Ray &ray, float &t) const = 0;
         Vector3f position;
         Material material;
    };
    class Sphere : public SolidObject
    {
    public:
        Sphere(const Vector3f &pos, const float &rad, const Material &mat) : SolidObject(pos, mat), radius(rad) {/*empty*/}

        bool intersect(const Ray &ray, float &t) const;
    private:
        float radius;
    };
    class Plane : public SolidObject
    {
    public:
        Plane(const Vector3f &pos, const Vector3f &n, const Material &mat) : SolidObject(pos, mat), normal(n) {/*empty*/}
        bool intersect(const Ray &ray, float &t) const;

    private:
        Vector3f normal;
    };
}

#endif // RAYTRACER_H
