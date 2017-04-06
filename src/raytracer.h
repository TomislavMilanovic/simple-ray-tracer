#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <vector>

//promijeniti pristup
#include "../lib/glm/glm.hpp"
#include "../lib/glm/gtc/constants.hpp"

namespace raytracer
{
    struct Intersection;

    typedef glm::vec3 Vector3f;
    typedef glm::vec3 Color;
    typedef std::vector<Intersection> intersectionList;

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
        std::vector<Color> *image;

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
         virtual bool intersect(const Ray &ray, intersectionList &list) const = 0;
         Vector3f position;
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
        bool intersect(const Ray &ray, intersectionList &list) const;
    private:
        float radius;
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
