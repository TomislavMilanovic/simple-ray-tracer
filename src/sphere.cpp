#include "raytracer.h"
#include <iostream>

namespace raytracer
{
    bool Sphere::intersect1(const Ray &ray, intersectionList &list, const glm::float32 start_t) const
    {
        glm::float32 t = start_t;
        Vector3f testPoint = ray.start + t*ray.dir;
        //Vector3f pointBefore(-200.0f, -200.0f, -200.0f);

        float u = 0.0f;
        float v = 0.0f;

        int x_texture = 0;
        int y_texture = 0;

        Vector3f d;

        if(glm::length(position - ray.start) <= radius)
            return false;

        //printf("\rray.start.x: %f ray.start.y: %f\n", ray.start.x, ray.start.y);

        while(1)
        {
            Ray testRay(testPoint, glm::normalize(position - testPoint));

            Intersection testIntersection;

            if(intersect(testRay, testIntersection, radius))
            {
                //printf("dist: %f\n", testIntersection.distance);

                d = -testIntersection.normal;

                u = 0.5f + (glm::atan(d.z, d.x)) / (2.0f*glm::pi<float>());
                v = 0.5f - (glm::asin(d.y)) / (glm::pi<float>());

                const float H = 256.0f;
                const float W = 256.0f;

                x_texture = (int)glm::floor(u * H) % (int)H/*+ 200*/;
                y_texture = (int)glm::floor(v * W);

                //printf("x_text: %d y_text: %d\n", x_texture, y_texture);

                float df = 0.3f * ((*material.image)[y_texture * (int)H + x_texture].r) +
                           0.59f * ((*material.image)[y_texture * (int)H + x_texture].g) +
                           0.11f * ((*material.image)[y_texture * (int)H + x_texture].b);

                float du = 40.0f;

                float d_ = df*du;

                /*if(d_ < 1.0f)
                    d_ = 0.0f;*/

                //printf("d_: %f\n", d_);

                Vector3f displacedPoint = testIntersection.point + (testIntersection.normal * d_);
                if(t == start_t && glm::length(ray.start - displacedPoint) <= 20.0f) return false;

                //t += glm::length(testPoint - displacedPoint);
                t += 0.5f;

                //printf("\rray.start.x: %f ray.start.y: %f\n", ray.start.x, ray.start.y);
                //if(glm::dot((testPoint - displacedPoint), (testIntersection.point - testPoint)) >= 0.0f)
                if(/*glm::length(pointBefore - testPoint) <= 1.0f ||*/
                        //glm::dot((testPoint - displacedPoint), (testIntersection.point - testPoint)) >= 0.0f)

                 glm::length(testPoint - displacedPoint) <= 1.0f)
                {
                    //printf("\r t: %f\n", t);
                    //printf("\rray.start.x: %f ray.start.y: %f\n", ray.start.x, ray.start.y);

                    Intersection intersection;

                    intersection.distance = t;
                    intersection.point = testPoint;
                    //intersection.point = displacedPoint;
                    intersection.normal = glm::normalize(intersection.point - position);
                    intersection.solid = this;

                    list.push_back(intersection);

                    return true;
                }
            }
            else
                return false;

            //pointBefore = testPoint;
            testPoint = ray.start + t*ray.dir;

            if(t > 2000.0f)
                return false;
        }
    }
    bool Sphere::intersect2(const Ray &ray, intersectionList &list) const
    {
        const float h = 257.0f;
        const float w = 257.0f;

        float max_displacement = 0.0f;

        /*for(int y = 0; y < h; ++y)
        {
            for(int x = 0; x < w; ++x)
            {
                float df = 0.3f * ((*material.image)[y * (int)h + x].r) +
                           0.59f * ((*material.image)[y * (int)h + x].g) +
                           0.11f * ((*material.image)[y * (int)h + x].b);

                if(df > max_displacement) max_displacement = df;
            }
        }*/

        //0.996078372001648
        //printf("max: %.15f\n", max_displacement);

        float df = 0.996078372001648f;
        float du = 40.0f;

        float d_ = df*du;

        float t = 0.0f;

        Intersection testIntersection;

        if(intersect(ray, testIntersection, radius + d_))
        {
            t = testIntersection.distance;
            return intersect1(ray, list, t);
        }
        else
        {
            return false;
        }
    }

    bool Sphere::intersect(const Ray &ray, intersectionList &list) const
    {
        return intersect2(ray, list);
    }


    bool Sphere::intersect(const Ray &ray, Intersection &intsc, const glm::float32 &radius) const
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

            if((t0 > 0.0f))
            {
                Intersection intersection;

                intersection.distance = t0;
                intersection.point = ray.start + t0 * ray.dir;
                intersection.normal = glm::normalize(intersection.point - position);
                intersection.solid = this;

                intsc = intersection;

                return_value = true;
            }
            else return_value = false;
        }

        return return_value;
    }
}
