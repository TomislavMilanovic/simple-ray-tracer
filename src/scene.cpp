#include <fstream>
#include <iostream>
#include "scene.h"

#include <typeinfo>
#include <vector>

//promijeniti pristup
#include "../lib/lodepng/lodepng.h"

namespace raytracer
{
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
                Light currentLight = lights[i];

                Vector3f dist = currentLight.pos - closestIntersection->point;
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

                    const float r2 = glm::length(dist);
                    const Color testLightIntensity = 2000.0f * currentLight.intensity / (4.0f * glm::pi<float>() * r2);

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

        for(glm::uint16 y = 0; y < height; ++y)
        {
            for(glm::uint16 x = 0; x < width; ++x)
            {
                img[y*width + x] = supersampling_grid(3, y, x, level);
                //printf("height: %d width: %d\n", y, x);
            }
        }

        savepng("scene_distanceSquared.png", width, height);

        img.clear();
    }
}

