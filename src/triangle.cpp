#include "raytracer.h"
#include <iostream>

namespace raytracer
{
    bool Triangle::intersect(const Ray &ray, intersectionList &list) const
    {
        const Vector3f v0v1 = v[1] - v[0];
        const Vector3f v0v2 = v[2] - v[0];
        const Vector3f pvec = glm::cross(ray.dir, v0v2);
        const float det = glm::dot(v0v1, pvec);

        if(det < 0.1f) return false;

        const float invDet = 1.0f / det;

        const Vector3f tvec = ray.start - v[0];
        const float u = glm::dot(tvec, pvec) * invDet;
        if(u < 0.0f || u > 1.0f) return false;

        const Vector3f qvec = glm::cross(tvec, v0v1);
        const float v = glm::dot(ray.dir, qvec) * invDet;
        if(v < 0.0f || u + v > 1.0f) return false;

        Intersection intersection;

        intersection.distance = glm::dot(v0v2, qvec) * invDet;
        intersection.point = ray.start + intersection.distance*ray.dir;
        intersection.normal = glm::normalize(glm::cross(v0v1, v0v2));
        intersection.solid = this;

        list.push_back(intersection);

        return true;
    }

    const Vector3f Triangle::getMinPoint() const
    {
        Vector3f min(v[0]);
        for(unsigned i = 1; i < 3; ++i)
        {
            if(v[i].x < min.x)
                min.x = v[i].x;

            if(v[i].y < min.y)
                min.y = v[i].y;

            if(v[i].z < min.z)
                min.z = v[i].z;
        }
        return min;
    }
    const Vector3f Triangle::getMaxPoint() const
    {
        Vector3f max(v[0]);
        for(unsigned i = 1; i < 3; ++i)
        {
            if(v[i].x > max.x)
                max.x = v[i].x;

            if(v[i].y > max.y)
                max.y = v[i].y;

            if(v[i].z > max.z)
                max.z = v[i].z;
        }
        return max;
    }
}
