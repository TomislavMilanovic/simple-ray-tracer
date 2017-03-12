#include "raytracer.h"

namespace raytracer
{
    bool Plane::intersect(const Ray &ray, float &t) const
    {
        float denom = glm::dot(normal, ray.dir);

        if(denom > 1e-6f)
        {
            Vector3f tmp = position - ray.start;
            float t0 = glm::dot(tmp, normal) / denom;

            if((t0 > 0.0f) && (t0 < t))
            {
                t = t0;
                return true;
            }
            else return false;
        }

        return false;
    }
}
