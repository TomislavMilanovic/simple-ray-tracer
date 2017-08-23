#include <fstream>
#include <iostream>
#include <random>
#include "scene.h"

#include <typeinfo>
#include <vector>
#include "../lib/lodepng/lodepng.h"

namespace raytracer
{  
    void Scene::addAreaLightUniform(const float &step)
    {
        //area light prototype
        const float wall_factor = -2.0f;
        const float offsetY = -0.01f;
        std::vector<Vector3f> points;

        for(float x = -0.5f; x < 0.5f; x += step)
            for(float y = 1.0f + offsetY; y <= 1.0f + offsetY; y += step)
                for(float z = wall_factor + 0.25f; z < -1.25f; z += step)
                    points.push_back(Vector3f(x, y, z));

        debugFloat(points.size(), "Number of point lights:");

        for(unsigned i = 0; i < points.size(); ++i)
            lights.push_back(new RealisticPointLight(points[i], Color(1.0f, 1.0f, 1.0f), 14.0f / points.size(), 1.0f));
    }

    const glm::uint16& Scene::get_antialiasing() const { return antialiasing; }
    void Scene::set_antialiasing(const glm::uint16 aa)
    {
        antialiasing = aa;

        if(aa <= 1)
        {
            rays_func = (projectionInfo) ? &raytracer::Scene::generate_rays : &raytracer::Scene::generate_rays_old;
            aa_func = &raytracer::Scene::trace;
        }
        else
        {
            rays_func = (projectionInfo) ? &raytracer::Scene::generate_rays : &raytracer::Scene::generate_rays_old;
            aa_func = (projectionInfo) ? &raytracer::Scene::supersampling_grid : &raytracer::Scene::supersampling_grid_old;
        }
    }
    void Scene::set_image_format(const ImageFormat f)
    {
        switch(f)
        {
            case ppm:
                output_func = &raytracer::Scene::saveppm;
                break;
            case png:
                output_func = &raytracer::Scene::savepng;
                break;
            default:
                output_func = &raytracer::Scene::savepng;
                break;
        }
    }
    void Scene::set_projection_info(const float fov, const Vector3f orig, const Vector3f u, const Vector3f v, const Vector3f w, const bool isOrthogonal)
    {
        if(projectionInfo)
        {
            projectionInfo->fov = fov;
            projectionInfo->orig = orig;
            projectionInfo->u = u;
            projectionInfo->v = v;
            projectionInfo->w = w;
            projectionInfo->isOrthogonal = isOrthogonal;
            projectionInfo->calc_cache(width, height);
        }
    }

    void Scene::saveppm() const
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
    void Scene::savepng() const
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

    Color Scene::trace(const Ray& ray) const
    {
        Color result;
        Ray r = ray;

        float refl_coef = 1.0f;
        glm::uint16 lvl = level;

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

                Ray shadowRay(closestIntersection.point, glm::normalize(dist), true);

                bool inShadow = false;
                Intersection lightClosestIntersection;

                if((getClosestIntersection(shadowRay, lightClosestIntersection)) &&
                   !(lightClosestIntersection.solid == closestIntersection.solid) &&
                   !(lightClosestIntersection.distance > glm::length(dist)))
                    inShadow = true;

                if(!inShadow)
                {
                    const Color diffuse = currentMat.diffuse();
                    const float lambert = glm::dot(shadowRay.dir, closestIntersection.normal) * refl_coef;
                    const Color testLightIntensity = lights[i]->getIntensity(closestIntersection.point);

                    result += lambert * testLightIntensity * diffuse;
                }
            }

            refl_coef *= currentMat.reflection();

            //odbijena zraka
            r = Ray(closestIntersection.point, glm::reflect(r.dir, closestIntersection.normal));
            lvl--;

        }while((refl_coef > 0.0f) && (lvl > 0));

        return Color(glm::min(result.r*255.0f, 255.0f), glm::min(result.g*255.0f, 255.0f), glm::min(result.b*255.0f, 255.0f));
    }

    Color Scene::supersampling_grid_old(const Ray& r) const
    {
        Color avg;
        Ray temp_r;

        float step = (1.0f/(float)antialiasing);

        for(float i = -0.5f + step * 0.5f; i < 0.5f; i += step)
        {
            for(float j = -0.5f + step * 0.5f; j < 0.5f; j += step)
            {
                Vector3f newStart(r.start.x + i, r.start.y + j, r.start.z);
                temp_r = Ray(newStart, r.dir);
                Color tmp = trace(temp_r);
                avg += tmp;
            }
        }

        avg /= antialiasing*antialiasing;
        return avg;
    }
    Color Scene::supersampling_grid(const Ray& r) const
    {
        Color avg;
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
                Color tmp = trace(temp_r);
                avg += tmp;
            }
        }
        avg /= antialiasing * antialiasing;
        return avg;
    }

    Ray Scene::generate_rays(const int& y, const int& x) const
    {
        Ray r;
        ProjectionInfo& projInfo = *projectionInfo;

        const Vector3f pixelCenter = projInfo.scanlineStart -
                projInfo.pixelHeight * projInfo.v * (glm::float32)y + projInfo.pixelWidth * projInfo.u * (glm::float32)x;

        if(!projInfo.isOrthogonal)
            r = Ray(projInfo.orig, glm::normalize(pixelCenter));
        else
            r = Ray(pixelCenter, projInfo.w);

        return r;
    }
    Ray Scene::generate_rays_old(const int& y, const int& x) const
    {
        Ray r(Vector3f((float)x, (float)y, -2000.0f), Vector3f(0.0f, 0.0f, 1.0f));

        return r;
    }

    void Scene::render()
    {
        debugString("Render started.");
        img.resize(width * height);

        for(int y = 0; y < height; ++y)
        {
            for(int x = 0; x < width; ++x)
            {
                const Ray r = (this->*rays_func)(y, x);
                img[y*width + x] = (this->*aa_func)(r);
            }
        }

        (this->*output_func)();

        img.clear();
        debugString(filename + " saved.");
    }
    bool Scene::getClosestIntersection(const Ray& r, Intersection& closestIntersection) const
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

