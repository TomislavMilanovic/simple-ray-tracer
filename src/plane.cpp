#include "raytracer.h"
#include <iostream>

namespace raytracer
{
    bool Plane::intersect(const Ray &ray, intersectionList &list) const
    {
        float denom = glm::dot(normal, ray.dir);

        if(denom > 1e-6f)
        {
            Vector3f tmp = position - ray.start;
            float t0 = glm::dot(tmp, normal) / denom;

            if((t0 > 0.0f))
            {
                Intersection intersection;

                intersection.distance = t0;
                intersection.point = ray.start + t0 * ray.dir;
                intersection.normal = glm::normalize(intersection.point - position);
                //intersection.normal = -normal;
                intersection.solid = this;

                list.push_back(intersection);

                return true;
            }
            else return false;
        }

        return false;
    }
}
