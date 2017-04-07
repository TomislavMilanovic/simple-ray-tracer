#include "raytracer.h"
#include <iostream>

namespace raytracer
{
    bool Sphere::intersect(const Ray &ray, intersectionList &list) const
    {
        glm::float32 t = 0.0f;
        Vector3f testPoint = ray.start + t*ray.dir;
        Vector3f pointBefore(-200.0f, -200.0f, -200.0f);

        float u = 0.0f;
        float v = 0.0f;

        int x_texture = 0;
        int y_texture = 0;

        Vector3f d;

        //printf("\rray.start.x: %f ray.start.y: %f\n", ray.start.x, ray.start.y);

        while(1)
        {
            //printf("!!\n");
            //t += 10.0f;

            /*if(ray.start.x >= 388 && ray.start.y >= 482)
            {
                printf("\r point before: %f %f %f\n", pointBefore.x, pointBefore.y, pointBefore.z);
                printf("\r point after: %f %f %f\n", testPoint.x, testPoint.y, testPoint.z);
            }*/

            Ray testRay(testPoint, glm::normalize(position - testPoint));

            Intersection testIntersection;

            if(intersect(testRay, testIntersection))
            {
                //printf("dist: %f\n", testIntersection.distance);

                d = -testIntersection.normal;

                u = 0.5f + (glm::atan(d.z, d.x)) / (2.0f*glm::pi<float>());
                v = 0.5f - (glm::asin(d.y)) / (glm::pi<float>());

                x_texture = (int)glm::floor(u * 256.0f);
                y_texture = (int)glm::floor(v * 256.0f);

                //printf("x_text: %d y_text: %d\n", x_texture, y_texture);

                float df = ((*material.image)[y_texture * 256 + x_texture].g);
                float du = 20.0f;

                float d_ = df*du;

                if(d_ < 1.0f)
                    d_ = 1.0f;

                //printf("d_: %f\n", d_);

                Vector3f displacedPoint = testIntersection.point + (testIntersection.normal * d_);
                if(glm::length(ray.start - displacedPoint) <= 10.0f) return false;

                t += glm::length(testPoint - displacedPoint);

                //printf("\rray.start.x: %f ray.start.y: %f\n", ray.start.x, ray.start.y);
                //if(glm::dot((testPoint - displacedPoint), (testIntersection.point - testPoint)) >= 0.0f)
                if(glm::length(pointBefore - testPoint) <= 1.0f)
                {
                    //printf("\r t: %f\n", t);
                    //printf("\rray.start.x: %f ray.start.y: %f\n", ray.start.x, ray.start.y);

                    Intersection intersection;

                    intersection.distance = t;
                    intersection.point = testPoint;
                    intersection.normal = glm::normalize(testPoint - position);
                    intersection.solid = this;

                    list.push_back(intersection);

                    return true;
                }
            }
            else
                return false;

            pointBefore = testPoint;
            testPoint = ray.start + t*ray.dir;

            if(t >= 5000.0f)
                return false;
        }
    }

    bool Sphere::intersect(const Ray &ray, Intersection &intsc) const
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
