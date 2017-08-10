#include "raytracer.h"
#include <iostream>

namespace raytracer
{
    bool AABB::intersect(const Ray &ray, intersectionList& list) const
    {
        if(ray.isLightRay)
            return false;

        glm::float32 t_min, t_max, t_min_y, t_max_y, t_min_z, t_max_z;

        t_min = (bounds[ray.sign[0]].x - ray.start.x) * ray.inv_dir.x;
        t_max = (bounds[1-ray.sign[0]].x - ray.start.x) * ray.inv_dir.x;

        t_min_y = (bounds[ray.sign[1]].y - ray.start.y) * ray.inv_dir.y;
        t_max_y = (bounds[1 - ray.sign[1]].y - ray.start.y) * ray.inv_dir.y;

        if( (t_min > t_max_y) || (t_min_y > t_max) )
            return false;

        if(t_min_y > t_min)
            t_min = t_min_y;
        if(t_max_y < t_max)
            t_max = t_max_y;

        t_min_z = (bounds[ray.sign[2]].z - ray.start.z) * ray.inv_dir.z;
        t_max_z = (bounds[1 - ray.sign[2]].z - ray.start.z) * ray.inv_dir.z;

        if( (t_min > t_max_z) || (t_min_z > t_max) )
            return false;

        if(t_min_z > t_min)
            t_min = t_min_z;
        if(t_max_z < t_max)
            t_max = t_max_z;

        if(glm::min(t_min, t_max) < 0.0f)
            return false;

        Intersection intersection;

        intersection.distance = glm::min(t_min, t_max);
        intersection.point = ray.start + glm::min(t_min, t_max) * ray.dir;
        intersection.normal = glm::normalize(Vector3f(0.0f, 0.0f, -1.0f));
        intersection.solid = this;

        list.push_back(intersection);

        return true;
    }
}
