#include "raytracer.h"

namespace raytracer
{
    bool Sphere::intersect(const Ray &ray, float &t) const
    {
        bool return_value = false;

        float A = glm::dot(ray.dir, ray.dir);
        Vector3f dist = ray.start - position;
        float B = 2.0f * glm::dot(ray.dir, dist);
        float C = glm::dot(dist, dist) - (radius * radius);
        float discr = B * B - 4.0f * A * C;

        if(discr < 0.0f)
            return_value = false;
        else
        {
            float sqrtdiscr = glm::sqrt(discr);
            float t0 = (-B + sqrtdiscr) / (2.0f);
            float t1 = (-B - sqrtdiscr) / (2.0f);

            t0 = glm::min(t0, t1);

            if((t0 > 0.0f) && (t0 < t))
            {
                t = t0;
                return_value = true;
            }
            else return_value = false;
        }

        return return_value;
    }
}
