#include "raytracer.h"
#include <iostream>

namespace raytracer
{
    bool Plane::intersect(const Ray &ray, intersectionList &list) const
    {
        float denom = glm::dot(normal, ray.dir);

        if(denom > 1e-6f)
        {
            Vector3f tmp = centroid - ray.start;
            float t0 = glm::dot(tmp, normal) / denom;

            if((t0 > 0.0f))
            {
                Intersection intersection;

                intersection.distance = t0;
                intersection.point = ray.start + t0 * ray.dir;
                intersection.normal = -normal;
                intersection.solid = this;

                list.push_back(intersection);

                return true;
            }
            else return false;
        }

        return false;
    }

    const Vector3f Plane::getMinPoint() const
    {
        return centroid * -INFINITY;
    }
    const Vector3f Plane::getMaxPoint() const
    {
        return centroid * INFINITY;
    }
}
