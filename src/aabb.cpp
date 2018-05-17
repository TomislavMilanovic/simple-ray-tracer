#include "raytracer.h"
#include <iostream>

namespace raytracer
{
    bool AABB::intersect(const Ray &ray, intersectionList& list) const
    {
        if(right == NULL && left)
            return left->intersect(ray, list);

        glm::float32 t_min, t_max, t_min_y, t_max_y, t_min_z, t_max_z;

        t_min = (bounds[ray.sign[0]].x - ray.start.x) * ray.inv_dir.x;
        t_max = (bounds[1 - ray.sign[0]].x - ray.start.x) * ray.inv_dir.x;

        t_min_y = (bounds[ray.sign[1]].y - ray.start.y) * ray.inv_dir.y;
        t_max_y = (bounds[1 - ray.sign[1]].y - ray.start.y) * ray.inv_dir.y;

        //if(std::isnan(t_min) || std::isnan(t_max) || std::isnan(t_min_y) || std::isnan(t_max_y))
            //return false;

        if( (t_min > t_max_y) || (t_min_y > t_max) )
            return false;

        if(t_min_y > t_min)
            t_min = t_min_y;

        if(t_max_y < t_max)
            t_max = t_max_y;

        t_min_z = (bounds[ray.sign[2]].z - ray.start.z) * ray.inv_dir.z;
        t_max_z = (bounds[1 - ray.sign[2]].z - ray.start.z) * ray.inv_dir.z;

        //if(std::isnan(t_min_z) || std::isnan(t_max_z))
            //return false;

        if( (t_min > t_max_z) || (t_min_z > t_max) )
            return false;

        bool child_intersect_1 = false;
        bool child_intersect_2 = false;

        if(left)
            child_intersect_1 = left->intersect(ray, list);

        if(right)
            child_intersect_2 = right->intersect(ray, list);

        if(child_intersect_1 || child_intersect_2)
            return true;
        else
            return false;
    }

    const Vector3f AABB::getMinPoint() const
    {
        return bounds[0];
    }
    const Vector3f AABB::getMaxPoint() const
    {
        return bounds[1];
    }
}
