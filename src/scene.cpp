#include <fstream>
#include <iostream>
#include "scene.h"

#include <typeinfo>
#include <vector>

//promijeniti pristup
#include "../lib/lodepng/lodepng.h"

namespace raytracer
{  
    void Scene::addAreaLight()
    {
        const Vector3f a(0.0f, 0.0f, 0.0f);
        const Vector3f b(5.0f, 0.0f, 0.0f);
        const Vector3f c(0.0f, 5.0f, 0.0f);

        const float ab_length = glm::length(a - b);
        const float ac_length = glm::length(a - c);

        const float dot_ab_ac = glm::dot(a - b, a - c);

        std::cout << "dot_ab_ac: " << dot_ab_ac << std::endl;

        Vector3f normal = glm::cross((a - b), (a - c));
        normal = glm::normalize(normal);

        if(isEqual(dot_ab_ac, 0.0f))
        {
            std::cout << "Točke odgovaraju pravokutnom trokutu!" << std::endl;
        }
        else
        {
            std::cout << "Ne!" << std::endl;
        }

        const Ray r1(b, glm::normalize(a - b));
        const Ray r2(c, glm::normalize(a - c));

        const float t1 = ab_length / 2.0f;
        const float t2 = ac_length / 2.0f;

        for(float t_x = 0.0f; t_x < ab_length; t_x += t1)
        {
            for(float t_y = 0.0f; t_y < ac_length; t_y += t2)
            {
                Vector3f perpendicular1 = glm::normalize(glm::perp(a - b, normal));
                Ray test_ray1(r1.start + t1 * r1.dir, perpendicular1);

                Vector3f perpendicular2 = glm::normalize(glm::perp(a - c, normal));
                Ray test_ray2(r2.start + t2 * r2.dir, perpendicular2);

                glm::mat2 A(test_ray1.dir.x, -test_ray2.dir.x, test_ray1.dir.y, -test_ray2.dir.y);
                float detA = glm::determinant(A);

                glm::mat2 At1(test_ray2.start.x - test_ray1.start.x, -test_ray2.dir.x, test_ray2.start.y - test_ray1.start.y, -test_ray2.dir.y);

                float detAt1 = glm::determinant(At1);
                float t1_solution = detAt1 / detA;

                glm::mat2 At2(test_ray1.dir.x, test_ray2.start.x - test_ray1.start.x, test_ray1.dir.y, test_ray2.start.y - test_ray1.start.y);

                float detAt2 = glm::determinant(At2);
                float t2_solution = detAt2 / detA;

                Vector3f newPoint1 = test_ray1.start + t1_solution * test_ray1.dir;
                Vector3f newPoint2 = test_ray2.start + t2_solution * test_ray2.dir;

                std::cout << newPoint1.x << " " << newPoint1.y << " " << newPoint1.z << " " << std::endl;
                std::cout << newPoint2.x << " " << newPoint2.y << " " << newPoint2.z << " " << std::endl;
            }
        }

        //area light prototype
        /*const float wall_factor = -2.0f;
        const float offsetY = -0.1f;
        const Vector3f a(-0.5f, 1.0f + offsetY, -1.25f);
        const Vector3f b(0.5f, 1.0f + offsetY, -1.25f);
        const Vector3f c(0.5f, 1.0f + offsetY, wall_factor + 0.25f);
        const Vector3f d(-0.5f, 1.0f + offsetY, wall_factor + 0.25f);

        const float step = 0.05f;

        int num = 0;

        for(float x = -0.5f; x < 0.5f; x += step)
        {
            for(float y = 1.0f + offsetY; y <= 1.0f + offsetY; y += step)
            {
                for(float z = wall_factor + 0.25f; z < -1.25f; z += step)
                {
                    Vector3f point(x, y, z);
                    lights.push_back(new PointLight(point, Color(1.0f, 1.0f, 1.0f), 0.9f / 220.0f));
                    num++;
                }
            }
        }

        std::cout << num << std::endl;*/
    }

    void Scene::saveppm(const std::string &filename, const glm::uint32 &width, const glm::uint32 &height)
    {
        std::ofstream file;

        file.open(filename.c_str(), std::ios::out | std::ios::binary);
        file << "P6 " << width << " " << height << " " << "255\n";

        for(glm::uint32 i = 0; i < width * height; ++i)
        {
            file << (unsigned char)img[i].r << (unsigned char)img[i].g << (unsigned char)img[i].b;
        }

        file.close();
    }
    void Scene::savepng(const std::string &filename, const glm::uint32 &width, const glm::uint32 &height)
    {
        const unsigned char ALPHA = 255;

        //0 - 255
        //0x00 - 0xff - 4B po boji
        const unsigned BYTES_PER_PIXEL = 4;
        const unsigned TEMP_BUFFER_SIZE = width * height * BYTES_PER_PIXEL;

        std::vector<unsigned char> tempBuffer(TEMP_BUFFER_SIZE);
        unsigned int bufferIndex = 0;

        for(glm::uint16 y = 0; y < height; ++y)
        {
            for(glm::uint16 x = 0; x < width; ++x)
            {
                tempBuffer[bufferIndex++] = img[y*width + x].r;
                tempBuffer[bufferIndex++] = img[y*width + x].g;
                tempBuffer[bufferIndex++] = img[y*width + x].b;
                tempBuffer[bufferIndex++] = ALPHA;
            }

        }

        lodepng::encode(filename, tempBuffer, width, height);
    }
    Color Scene::trace(Ray &r, const glm::uint16 &lvl)
    {
        Color result;
        intersectionList list, testList;

        float coef = 1.0f;
        glm::uint16 level = lvl;

        do
        {
            Intersection *closestIntersection = NULL;
            list.clear();

            for(glm::uint16 i = 0; i < objects.size(); ++i)
                objects[i]->intersect(r, list);

            if(list.size() == 0)
                break;

            closestIntersection = &list[0];

            for(glm::uint16 i = 0; i < list.size(); ++i)
            {
                if(list[i].distance < closestIntersection->distance)
                {
                    closestIntersection = &list[i];
                }
            }

            Material currentMat = closestIntersection->solid->surfaceMaterial(closestIntersection->point);

            for(glm::uint16 i = 0; i < lights.size(); ++i)
            {
                Vector3f dist = lights[i]->getDirection(closestIntersection->point);
                if(glm::dot(closestIntersection->normal, dist) <= 0.0f) continue; //ako nikako nema svjetla

                Ray lightRay(closestIntersection->point, glm::normalize(dist));

                bool inShadow = false;

                //Staviti da ako je sjena da završava algoritam općenito

                for(glm::uint16 j = 0; j < objects.size(); ++j)
                {
                    testList.clear();
                    if(objects[j]->intersect(lightRay, testList))
                    {
                        if(testList.back().solid == closestIntersection->solid)
                            continue;

                        if(testList.back().distance > (glm::length(dist)))
                            continue;

                        inShadow = true;
                        break;
                    }
                }
                testList.clear();
                //inShadow = false;
                if(!inShadow)
                {
                    const Color diffuse = currentMat.diffuse();
                    float lambert = glm::dot(lightRay.dir, closestIntersection->normal) * coef;

                    //Color testLightIntensity = (14.0f / (float)areaLightPoints.size()) * Vector3f(1.0f, 1.0f, 1.0f) / (4.0f * glm::pi<float>() * r2);

                    const Color testLightIntensity = lights[i]->getIntensity(closestIntersection->point);

                    result.r += lambert * testLightIntensity.r * diffuse.r;
                    result.g += lambert * testLightIntensity.g * diffuse.g;
                    result.b += lambert * testLightIntensity.b * diffuse.b;
                }
            }

            coef *= currentMat.reflection();

            //odbijena zraka
            r.dir = glm::reflect(r.dir, closestIntersection->normal);
            r.start = closestIntersection->point;
            level--;

        }while((coef > 0.0f) && (level > 0));

        return Color(glm::min(result.r*255.0f, 255.0f), glm::min(result.g*255.0f, 255.0f), glm::min(result.b*255.0f, 255.0f));
    }

    Color Scene::supersampling_grid(const int &n, const float &y, const float &x, const glm::uint16 &level)
    {
        Color avg;
        Ray r;

        float step = (n % 2 == 0) ? (0.5f * (1.0f/(float)n)) : ((1.0f/(float)n));

        if(n % 2 != 0)
            step /= 2.0f;

        for(float i = -0.5f + step; i < 0.5f; i += step*2.0f)
        {
            for(float j = -0.5f +  step; j < 0.5f; j += step*2.0f)
            {
                r.start.x = x + i;
                r.start.y = y + j;
                r.start.z = -2000.0f;

                r.dir.x = 0.0f;
                r.dir.y = 0.0f;
                r.dir.z = 1.0f;

                Color tmp = trace(r, level);

                avg.r += tmp.r;
                avg.g += tmp.g;
                avg.b += tmp.b;
            }
        }

        avg.r /= n*n;
        avg.g /= n*n;
        avg.b /= n*n;

        return avg;
    }

    void Scene::render(const glm::uint16 &width, const glm::uint16 &height, const glm::uint16 &level)
    {
        img.resize(width * height);

        //std::cout << areaLightPoints[0].x << std::endl;

        /*for(glm::uint16 y = 0; y < height; ++y)
        {
            for(glm::uint16 x = 0; x < width; ++x)
            {
                img[y*width + x] = supersampling_grid(3, y, x, level);
                //printf("height: %d width: %d\n", y, x);
            }
        }*/

        //možda ukloniti float kod width
        const float imageAspectRatio = (float)width / (float)height;
        const float fov = 100.0f;
        const float scale = glm::tan(glm::radians(0.5f * fov));
        //const glm::mat4 cameraToWorld;

        const glm::vec3 orig(0.0f,0.0f,0.0f);

        Ray r;

        for(glm::uint16 j = 0; j < height; ++j)
        {
            for(glm::uint16 i = 0; i < width; ++i)
            {
                const float x = (2.0f * (i + 0.5f) / (float)width - 1.0f) * imageAspectRatio * scale;
                const float y = (1.0f - 2.0f * (j + 0.5f) / (float)height) * scale;

                //std::cout << x << " " << y << " " << std::endl;

                const glm::vec3 dir = glm::normalize(glm::vec3(x, y, -1.0f) - orig);

                r.start.x = orig.x;
                r.start.y = orig.y;
                r.start.z = orig.z;

                r.dir.x = dir.x;
                r.dir.y = dir.y;
                r.dir.z = dir.z;

                //std::cout << dir.x << " " << dir.y << " " << dir.z << std::endl;

                img[j*width + i] = trace(r, level);
            }
        }

        savepng("scene_distanceSquared.png", width, height);

        img.clear();
    }
}

