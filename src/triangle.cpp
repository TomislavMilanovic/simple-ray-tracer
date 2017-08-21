#include "raytracer.h"
#include <iostream>

namespace raytracer
{
    bool Triangle::intersect(const Ray &ray, intersectionList &list) const
    {
        const float eps = 0.1f;
        const Vector3f e1 = v[1] - v[0];
        const Vector3f e2 = v[2] - v[0];
        const Vector3f p = glm::cross(ray.dir, e2);
        const float det = glm::dot(e1, p);

        if(culling)
        {
            if(det < eps) return false;
        }
        else
        {
            if(fabs(det) < eps) return false;
        }

        const float invDet = 1.0f / det;

        const Vector3f s = ray.start - v[0];
        const float u = glm::dot(s, p) * invDet;
        if(u < 0.0f || u > 1.0f) return false;

        const Vector3f q = glm::cross(s, e1);
        const float v = glm::dot(ray.dir, q) * invDet;
        if(v < 0.0f || u + v > 1.0f) return false;

        Intersection intersection;

        intersection.distance = glm::dot(e2, q) * invDet;
        intersection.point = ray.start + intersection.distance*ray.dir;
        intersection.normal = glm::sign(det) * glm::normalize(glm::cross(e1, e2));
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
