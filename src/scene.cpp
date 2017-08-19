#include <fstream>
#include <iostream>
#include <random>
#include "scene.h"

#include <typeinfo>
#include <vector>
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

    void Scene::saveppm()
    {
        std::ofstream file;
        const std::string new_filename = filename + ".ppm";

        file.open(new_filename.c_str(), std::ios::out | std::ios::binary);
        file << "P6 " << width << " " << height << " " << "255\n";

        for(glm::uint32 i = 0; i < (glm::uint32)width * height; ++i)
        {
            file << (unsigned char)img[i].r << (unsigned char)img[i].g << (unsigned char)img[i].b;
        }

        file.close();
    }
    void Scene::savepng()
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

        lodepng::encode(filename + ".png", tempBuffer, width, height);
    }

    Color Scene::trace(Ray &r, const glm::uint16 &lvl)
    {
        Color result;

        float refl_coef = 1.0f;
        glm::uint16 level = lvl;

        do
        {
            Intersection closestIntersection;
            if(!(getClosestIntersection(r, closestIntersection)))
                    break;

            Material currentMat = closestIntersection.solid->surfaceMaterial(closestIntersection.point);

            for(glm::uint16 i = 0; i < lights.size(); ++i)
            {
                Vector3f dist = lights[i]->getDirection(closestIntersection.point);
                if(glm::dot(closestIntersection.normal, dist) <= 0.0f) continue; //ako nikako nema svjetla

                Ray lightRay(closestIntersection.point, glm::normalize(dist), true);

                bool inShadow = false;
                Intersection lightClosestIntersection;

                if((getClosestIntersection(lightRay, lightClosestIntersection)) &&
                   !(lightClosestIntersection.solid == closestIntersection.solid) &&
                   !(lightClosestIntersection.distance > glm::length(dist)))
                    inShadow = true;

                if(!inShadow)
                {
                    const Color diffuse = currentMat.diffuse();
                    const float lambert = glm::dot(lightRay.dir, closestIntersection.normal) * refl_coef;
                    const Color testLightIntensity = lights[i]->getIntensity(closestIntersection.point);

                    result += lambert * testLightIntensity * diffuse;
                }
            }

            refl_coef *= currentMat.reflection();

            //odbijena zraka
            r = Ray(closestIntersection.point + closestIntersection.normal * 0.01f, glm::reflect(r.dir, closestIntersection.normal));
            level--;

        }while((refl_coef > 0.0f) && (level > 0));

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

                Color tmp = trace(temp_r, lvl);

                avg += tmp;
            }
        }

        avg /= antialiasing*antialiasing;
        return avg;
    }
    Color Scene::supersampling_grid(Ray &r, const glm::uint16 &lvl)
    {
        Color avg = Color(0.0f, 0.0f, 0.0f);
        Ray temp_r;
        ProjectionInfo& projInfo = *projectionInfo;

        const float step_x = (projInfo.pixelWidth / (float)antialiasing);
        const float step_y = (projInfo.pixelHeight / (float)antialiasing);

        const float pw_half = projInfo.pixelWidth * 0.5f;
        const float ph_half = projInfo.pixelHeight * 0.5f;

        for(float i = -pw_half + step_x * pw_half; i < pw_half; i += step_x)
        {
            for(float j = -ph_half + step_y * ph_half; j < ph_half; j += step_y)
            {
                if(!projInfo.isOrthogonal)
                {
                    Vector3f newDir = r.dir + i * projInfo.u + j * projInfo.v;
                    newDir = glm::normalize(newDir);
                    temp_r = Ray(r.start, newDir);
                }
                else
                {
                    Vector3f newStart = r.start + i * projInfo.u + j * projInfo.v;
                    temp_r = Ray(newStart, r.dir);
                }

                Color tmp = trace(temp_r, lvl);

                avg += tmp;
            }
        }

        avg /= antialiasing * antialiasing;
        return avg;
    }

    Ray Scene::generate_rays(const int &y, const int &x)
    {
        Ray r;
        ProjectionInfo& projInfo = *projectionInfo;

        const Vector3f pixelCenter = projInfo.scanlineStart -
                projInfo.pixelHeight * projInfo.v * (glm::float32)y + projInfo.pixelWidth * projInfo.u * (glm::float32)x;

        if(!projInfo.isOrthogonal)
            r = Ray(projInfo.orig, pixelCenter);
        else
            r = Ray(pixelCenter, projInfo.w);

        return r;
    }
    Ray Scene::generate_rays_old(const int &y, const int &x)
    {
        Ray r(Vector3f((float)x, (float)y, -2000.0f), Vector3f(0.0f, 0.0f, 1.0f));

        return r;
    }

    void Scene::render(const glm::uint16 &level)
    {
        debugString("Render started.");
        img.resize(width * height);

        for(int y = 0; y < height; ++y)
        {
            for(int x = 0; x < width; ++x)
            {
                Ray r = generate_rays(y, x);
                //img[y*width + x] = trace(r, level);
                img[y*width + x] = supersampling_grid(r, level);
            }
        }

        savepng();

        img.clear();
        debugString("Render ended.");
    }

    bool Scene::getClosestIntersection(const Ray& r, Intersection& closestIntersection)
    {
        intersectionList list;

        for(glm::uint16 i = 0; i < objects.size(); ++i)
            objects[i]->intersect(r, list);

        if(list.size() == 0)
            return false;

       closestIntersection = list[0];

        for(glm::uint16 i = 0; i < list.size(); ++i)
        {
            if(list[i].distance < closestIntersection.distance)
                closestIntersection = list[i];
        }

        return true;
    }
}

