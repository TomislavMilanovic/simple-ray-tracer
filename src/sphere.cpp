#include "raytracer.h"
#include <iostream>

namespace raytracer
{
    bool Sphere::normal_intersect(const Ray &ray, Intersection &intsc, const glm::float32 &custom_radius) const
    {
        bool return_value = false;

        const float A = glm::dot(ray.dir, ray.dir);
        const Vector3f dist = ray.start - position;
        const float B = 2.0f * glm::dot(ray.dir, dist);
        const float C = glm::dot(dist, dist) - (custom_radius * custom_radius);
        const float discr = B * B - 4.0f * A * C;

        if(discr < 0.0f)
            return_value = false;
        else
        {
            const float sqrtdiscr = glm::sqrt(discr);
            const float t0 = (-B + sqrtdiscr) / (2.0f);
            const float t1 = (-B - sqrtdiscr) / (2.0f);

            const float t = glm::min(t0, t1);

            if((t > 0.0f))
            {
                Intersection intersection;

                intersection.distance = t;
                intersection.point = ray.start + t * ray.dir;
                intersection.normal = glm::normalize(intersection.point - position);
                intersection.solid = this;

                intsc = intersection;

                return_value = true;
            }
            else return_value = false;
        }

        return return_value;
    }
    bool Sphere::normal_intersect_wrapper(const Ray &ray, intersectionList &list) const
    {
        Intersection testIntersection;
        const bool isIntersect = normal_intersect(ray, testIntersection, radius);

        if(isIntersect) list.push_back(testIntersection);

        return isIntersect;
    }

    bool Sphere::disp_mapping_intersect(const Ray &ray, intersectionList &list, const glm::float32 start_t) const
    {
        glm::float32 t = start_t;
        Vector3f testPoint = ray.start + t*ray.dir;

        const SphereDisplacementMap &disp_map = *displacement_map;

        const float max_disp_df = disp_map.get_max_displacement();
        const float max_disp_du = disp_map.get_du();

        if(glm::length(position - ray.start) <= radius)
            return false;

        while(1)
        {
            Ray testRay(testPoint, glm::normalize(position - testPoint));

            Intersection testIntersection;

            if(normal_intersect(testRay, testIntersection, radius))
            {
                const SphereDisplacementMap &disp_map = *displacement_map;
                float d_ = disp_map.getDispMapping(testIntersection.normal);

                Vector3f displacedPoint = testIntersection.point + (testIntersection.normal * d_);
                if(t == start_t && glm::length(ray.start - displacedPoint) <= 0.01f) return false;

                t += 0.001f;

                if(//glm::dot((testPoint - displacedPoint), (testIntersection.point - testPoint)) >= 0.0f &&
                   glm::length(testPoint - displacedPoint) <= 0.01f)
                {
                    Intersection intersection;

                    intersection.distance = t;
                    //intersection.point = testPoint;
                    intersection.point = displacedPoint;
                    intersection.normal = glm::normalize(intersection.point - position);
                    intersection.solid = this;

                    list.push_back(intersection);

                    return true;
                }
            }
            else
                return false;

            testPoint = ray.start + t*ray.dir;

            //testirati da li je trenutna točka unutar "proširene" sfere

            if(!isPointInSphere(testPoint, position, radius + max_disp_df * max_disp_du))
                return false;
        }
    }
    bool Sphere::disp_mapping_intersect_wrapper(const Ray &ray, intersectionList &list) const
    {
        const SphereDisplacementMap &disp_map = *displacement_map;

        const float df = disp_map.get_max_displacement();
        const float du = disp_map.get_du();

        const float d_ = df*du;

        float t = 0.0f;

        Intersection testIntersection;

        if(normal_intersect(ray, testIntersection, radius + d_))
        {
            t = testIntersection.distance;
            return disp_mapping_intersect(ray, list, t);
        }

        return false;
    }

    bool Sphere::intersect(const Ray &ray, intersectionList &list) const
    {
        return (this->*intersect_func)(ray, list);
    }

    Material Sphere::get_normal_material(const Vector3f &surfacePoint) const
    {
        return material;
    }
    Material Sphere::get_texture_material(const Vector3f &surfacePoint) const
    {
        const SphereTextureMap &text_map = *texture_map;
        Material currentMaterial = material;

        Vector3f normal = glm::normalize(surfacePoint - position);
        Color pointColor = text_map.getTextureMapping(normal);

        currentMaterial.setDiffuse(pointColor);

        return currentMaterial;
    }
    Material Sphere::surfaceMaterial(const Vector3f &surfacePoint) const
    {
         return (this->*material_func)(surfacePoint);
    }
}
