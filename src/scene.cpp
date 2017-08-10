#include <fstream>
#include <iostream>
#include <random>
#include "scene.h"

#include <typeinfo>
#include <vector>

//promijeniti pristup
#include "../lib/lodepng/lodepng.h"

namespace raytracer
{  
    void Scene::addAreaLightRandom(const int &lightNum)
    {
        //area light prototype
        const float wall_factor = -2.0f;
        const float offsetY = -0.01f;
        const Vector3f a(-0.5f, 1.0f + offsetY, -1.25f);
        const Vector3f b(0.5f, 1.0f + offsetY, -1.25f);
        const Vector3f c(0.5f, 1.0f + offsetY, wall_factor + 0.25f);
        const Vector3f d(-0.5f, 1.0f + offsetY, wall_factor + 0.25f);

        std::random_device rd;
        std::mt19937 e2(rd());

        std::uniform_real_distribution<> x_dist(-0.5f, 0.5f);
        std::uniform_real_distribution<> y_dist(1.0f + offsetY, 1.0f + offsetY);
        std::uniform_real_distribution<> z_dist(wall_factor + 0.25f, -1.25f);

        for(int i = 0; i < lightNum; i++)
        {
            Vector3f point(x_dist(e2), y_dist(e2), z_dist(e2));
            lights.push_back(new RealisticPointLight(point, Color(1.0f, 1.0f, 1.0f), 14.0f / (float)lightNum));
        }
    }

    void Scene::addAreaLightUniform(const float &step)
    {
        //area light prototype
        const float wall_factor = -2.0f;
        const float offsetY = -0.01f;
        const Vector3f a(-0.5f, 1.0f + offsetY, -1.25f);
        const Vector3f b(0.5f, 1.0f + offsetY, -1.25f);
        const Vector3f c(0.5f, 1.0f + offsetY, wall_factor + 0.25f);
        const Vector3f d(-0.5f, 1.0f + offsetY, wall_factor + 0.25f);

        int num = 0;

        for(float x = -0.5f; x < 0.5f; x += step)
        {
            for(float y = 1.0f + offsetY; y <= 1.0f + offsetY; y += step)
            {
                for(float z = wall_factor + 0.25f; z < -1.25f; z += step)
                {
                    Vector3f point(x, y, z);
                    //std::cout << x << " " << y << " " << z << std::endl;
                    //if(isPointInRectangle(point, a, b, d))
                    {
                        //std::cout << x << " " << y << " " << z << std::endl;
                        lights.push_back(new RealisticPointLight(point, Color(1.0f, 1.0f, 1.0f), 14.0f / 220.0f));
                        num++;
                    }
                    /*else
                    {
                        std::cout << x << " " << y << " " << z << std::endl;
                    }*/
                }
            }
        }

        std::cout << num << std::endl;
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

                Ray lightRay(closestIntersection->point, glm::normalize(dist), true);

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
            r = Ray(closestIntersection->point + closestIntersection->normal * 0.01f, glm::reflect(r.dir, closestIntersection->normal));
            level--;

        }while((coef > 0.0f) && (level > 0));

        return Color(glm::min(result.r*255.0f, 255.0f), glm::min(result.g*255.0f, 255.0f), glm::min(result.b*255.0f, 255.0f));
    }

    Color Scene::supersampling_grid_old(Ray &r, const glm::uint16 &lvl)
    {
        Color avg;
        Ray temp_r;

        float step = (1.0f/(float)antialiasing);

        for(float i = -0.5f; i < 0.5f; i += step)
        {
            for(float j = -0.5f; j < 0.5f; j += step)
            {
                Vector3f newStart(r.start.x + i, r.start.y + j, r.start.z);
                temp_r = Ray(newStart, r.dir);

                if(r.start.x == 0 && r.start.y == 0)
                    debugVec3f(temp_r.start);

                Color tmp = trace(temp_r, lvl);

                avg.r += tmp.r;
                avg.g += tmp.g;
                avg.b += tmp.b;
            }
        }

        avg.r /= antialiasing*antialiasing;
        avg.g /= antialiasing*antialiasing;
        avg.b /= antialiasing*antialiasing;

        return avg;
    }

    Ray Scene::generate_rays(const int &y, const int &x)
    {
        Ray r;

        const float scale = glm::tan(glm::radians(0.5f * projectionInfo.fov));
        const glm::float32 pixelHeight = (2.0f * scale) / (float)height;
        const glm::float32 pixelWidth = (2.0f * scale) / (float)width;

        const Vector3f scanlineStart = projectionInfo.orig + projectionInfo.w - ((float)width / 2.0f) * pixelWidth * projectionInfo.u
                + ((float)height / 2.0f) * pixelHeight * projectionInfo.v
                + (pixelWidth / 2.0f) * projectionInfo.u
                - (pixelHeight / 2.0f) * projectionInfo.v;

        const Vector3f pixelCenter = scanlineStart -
                pixelHeight * projectionInfo.v * (glm::float32)y + pixelWidth * projectionInfo.u * (glm::float32)x;

        if(!projectionInfo.isOrthogonal)
            r = Ray(projectionInfo.orig, glm::normalize(pixelCenter));
        else
            r = Ray(pixelCenter - projectionInfo.w, projectionInfo.w);

        return r;
    }

    Ray Scene::generate_rays_old(const int &y, const int &x)
    {
        Ray r(Vector3f(x, y, -2000.0f), Vector3f(0.0f, 0.0f, 1.0f));

        return r;
    }

    void Scene::render(const glm::uint16 &level)
    {
        img.resize(width * height);

        for(int y = 0; y < height; ++y)
        {
            for(int x = 0; x < width; ++x)
            {
                Ray r = generate_rays_old(y, x);
                img[y*width + x] = trace(r, level);
                //img[y*width + x] = supersampling_grid_old(r, level);
            }
        }

        savepng("scene.png", width, height);

        img.clear();
    }
}

