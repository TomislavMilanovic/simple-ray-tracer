
#include <fstream>
#include "scene.h"

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
        float red = 0.0f;
        float green = 0.0f;
        float blue = 0.0f;

        float coef = 1.0f;
        glm::uint16 level = lvl;

        do
        {
            float t = std::numeric_limits<float>::infinity();
            glm::int16 currentObject = -1;

            for(glm::uint16 i = 0; i < objects.size(); ++i)
                if(objects[i]->intersect(r, t)) currentObject = i;
            if(currentObject == -1) break;

            Vector3f newDir = t * r.dir; // p = t*d + p0
            Vector3f newStart = r.start + newDir; //tocka na objektu

            Vector3f n = newStart - objects[currentObject]->position;
            n = glm::normalize(n);

            /*if(typeid(*scene.objects[currentObject]).name() == typeid(*scene.objects[2]).name())
                n = -glm::normalize(Vector3f(0.0f, 1.0f, 1.0f));*/

            //cout << n.x << " " << n.y << " " << n.z << " "<< endl;

            Material currentMat = objects[currentObject]->material;

            for(glm::uint16 i = 0; i < lights.size(); ++i)
            {
                Light currentLight = lights[i];

                Vector3f dist = currentLight.pos - newStart;
                if(glm::dot(n, dist) <= 0.0f) continue; //ako nikako nema svjetla

                Ray lightRay(newStart, glm::normalize(dist));

                bool inShadow = false;
                t = std::numeric_limits<float>::infinity();

                for(glm::uint16 j = 0; j < objects.size(); ++j)
                {
                    if(objects[j]->intersect(lightRay, t))
                    {
                        inShadow = true;
                        break;
                    }
                }
                if(!inShadow)
                {
                    float lambert = glm::dot(lightRay.dir, n) * coef;
                    red += lambert * currentLight.intensity.r * currentMat.diffuse.r;
                    green += lambert * currentLight.intensity.g * currentMat.diffuse.g;
                    blue += lambert * currentLight.intensity.b * currentMat.diffuse.b;
                }
            }

            coef *= currentMat.reflection;

            //odbijena zraka
            r.dir = glm::reflect(r.dir, n);
            r.start = newStart;
            level--;

        }while((coef > 0.0f) && (level > 0));

        return Color(glm::min(red*255.0f, 255.0f), glm::min(green*255.0f, 255.0f), glm::min(blue*255.0f, 255.0f));
    }
    void Scene::render(const glm::uint16 &width, const glm::uint16 &height, const glm::uint16 &level)
    {
        img.resize(width * height);

        for(glm::uint16 y = 0; y < height; ++y)
        {
            for(glm::uint16 x = 0; x < width; ++x)
            {
                Ray r;
                Color avg;
                int count = 0;

                for(float w = -0.9f; w <= 0.9f; w += 0.2f)
                {
                    r.start.x = x + w;
                    r.start.y = y + w;
                    r.start.z = -2000.0f;

                    r.dir.x = 0.0f;
                    r.dir.y = 0.0f;
                    r.dir.z = 1.0f;

                    count++;

                    Color tmp = trace(r, level);

                    avg.r += tmp.r;
                    avg.g += tmp.g;
                    avg.b += tmp.b;
                }

                avg.r /= (float)count;
                avg.g /= (float)count;
                avg.b /= (float)count;

                img[y*width + x] = avg;
            }
        }

        savepng("scene.png", width, height);

        img.clear();
    }
}

