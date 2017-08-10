#include "raytracer.h"
#include <iostream>

namespace raytracer
{
    bool Triangle::intersect(const Ray &ray, intersectionList &list) const
    {
        const Vector3f v0v1 = v1 - v0;
        const Vector3f v0v2 = v2 - v0;
        const Vector3f pvec = glm::cross(ray.dir, v0v2);
        const float det = glm::dot(v0v1, pvec);

        if(det < 0.1f) return false;

        const float invDet = 1.0f / det;

        const Vector3f tvec = ray.start - v0;
        const float u = glm::dot(tvec, pvec) * invDet;
        if(u < 0.0f || u > 1.0f) return false;

        const Vector3f qvec = glm::cross(tvec, v0v1);
        const float v = glm::dot(ray.dir, qvec) * invDet;
        if(v < 0.0f || u + v > 1.0f) return false;

        Intersection intersection;

        intersection.distance = glm::dot(v0v2, qvec) * invDet;
        intersection.point = ray.start + intersection.distance*ray.dir;
        intersection.normal = glm::normalize(glm::cross(v0v1, v0v2));
        //intersection.normal = glm::normalize(Vector3f(0.0f, 0.0f, -1.0f));
        intersection.solid = this;

        list.push_back(intersection);

        return true;
    }
}
