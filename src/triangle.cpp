#include "raytracer.h"
#include <iostream>

namespace raytracer
{
    bool Triangle::intersect(const Ray &ray, intersectionList &list) const
    {
        const Vector3f v0v1 = v1 - v0;
        const Vector3f v0v2 = v2 - v0;
        const Vector3f pvec = glm::cross(ray.dir, v0v2);
        const float det = glm::dot(v0v1, pvec);

        if(det < 0.1f) return false;

        const float invDet = 1.0f / det;

        const Vector3f tvec = ray.start - v0;
        const float u = glm::dot(tvec, pvec) * invDet;
        if(u < 0.0f || u > 1.0f) return false;

        const Vector3f qvec = glm::cross(tvec, v0v1);
        const float v = glm::dot(ray.dir, qvec) * invDet;
        if(v < 0.0f || u + v > 1.0f) return false;

        Intersection intersection;

        intersection.distance = glm::dot(v0v2, qvec) * invDet;
        intersection.point = ray.start + intersection.distance*ray.dir;
        intersection.normal = glm::normalize(glm::cross(v0v1, v0v2));
        //intersection.normal = glm::normalize(Vector3f(0.0f, 0.0f, -1.0f));
        intersection.solid = this;

        list.push_back(intersection);

        return true;

        /*const Vector3f v0v1 = v1 - v0;
        const Vector3f v0v2 = v2 - v0;
        const Vector3f N = glm::cross(v0v1, v0v2);
        const float denom = glm::dot(N, N);

        const float NdotRayDirection = glm::dot(N, ray.dir);
        if(glm::abs(NdotRayDirection) < 0.01f) {

            return false;
        }

        const float d = glm::dot(N, v0);

        const float t = (glm::dot(N, ray.start) + d) / NdotRayDirection;

        if(t < 0.0f) {

            return false;
        }

        const Vector3f P = ray.start + t * ray.dir;

        Vector3f C;

        Vector3f edge0 = v1 - v0;
        Vector3f vp0 = P - v0;
        C = glm::cross(edge0, vp0);
        if (glm::dot(N, C) < 0.0f) return false;

        Vector3f edge1 = v2 - v1;
        Vector3f vp1 = P - v1;
        C = glm::cross(edge1, vp1);
        if (glm::dot(N, C) < 0.0f) return false;

        Vector3f edge2 = v0 - v2;
        Vector3f vp2 = P - v2;
        C = glm::cross(edge2, vp2);
        if (glm::dot(N, C) < 0.0f) return false;

        Intersection intersection;

        intersection.distance = t;
        intersection.point = ray.start + t*ray.dir;
        intersection.normal = glm::normalize(glm::cross(v0v1, v0v2));
        //intersection.normal = glm::normalize(Vector3f(0.0f, 0.0f, 1.0f));
        intersection.solid = this;

        list.push_back(intersection);

        return true;*/

    }
}
