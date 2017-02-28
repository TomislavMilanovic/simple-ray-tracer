#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <limits>

#include <typeinfo>

#include "glm/glm.hpp"

using namespace std;

typedef glm::vec3 Vector3f;
typedef glm::vec3 Color;

class Light
{
public:
    Vector3f pos;
    Color intensity;

    Light(const Vector3f &p, const Color &intens) : pos(p) , intensity(intens){}
};
class Material
{
public:
    Color diffuse;
    float reflection;

    Material(const Color &d, const float &refl) : diffuse(d), reflection(refl) {}
};
class Ray
{
public:
    Ray() : start(Vector3f()), dir(Vector3f()) {}
    Ray(const Vector3f &st, const Vector3f &d) : start(st), dir(d) {/*empty*/}
    Vector3f start, dir;
};
class SolidObject
{
public:
     SolidObject(const Vector3f &pos, const Material &mat) : position(pos), material(mat) {/*empty*/}
     virtual bool intersect(const Ray &ray, float &t) const = 0;
     Vector3f position;
     Material material;
};

class Sphere : public SolidObject
{
public:
    Sphere(const Vector3f &pos, const float &rad, const Material &mat) : SolidObject(pos, mat), radius(rad) {/*empty*/}

    bool intersect(const Ray &ray, float &t) const
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

            if((t0 > 0.0f) && (t0 < t))
            {
                t = t0;
                return_value = true;
            }
            else return_value = false;
        }

        return return_value;
    }
private:
    float radius;
};
class Plane : public SolidObject
{
public:
    Plane(const Vector3f &pos, const Vector3f &n, const Material &mat) : SolidObject(pos, mat), normal(n) {/*empty*/}
    bool intersect(const Ray &ray, float &t) const
    {
        float denom = glm::dot(normal, ray.dir);

        if(denom > 1e-6f)
        {
            Vector3f tmp = position - ray.start;
            float t0 = glm::dot(tmp, normal) / denom;

            if((t0 > 0.0f) && (t0 < t))
            {
                t = t0;
                return true;
            }
            else return false;
        }

        return false;
    }

private:
    Vector3f normal;
};

class Scene
{
public:
    std::vector<SolidObject*> objects;
    std::vector<Light> lights;
    std::vector<Material> materials;
};

void saveppm(const std::string &filename, const std::vector <Color> &img, const glm::uint32 &width, const glm::uint32 &height)
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

Color trace(Ray &r, const glm::uint16 &lvl, const Scene &scene)
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

        for(glm::uint16 i = 0; i < scene.objects.size(); ++i)
            if(scene.objects[i]->intersect(r, t)) currentObject = i;
        if(currentObject == -1) break;

        Vector3f newDir = t * r.dir; // p = t*d + p0
        Vector3f newStart = r.start + newDir; //tocka na objektu

        Vector3f n = newStart - scene.objects[currentObject]->position;
        n = glm::normalize(n);

        /*if(typeid(*scene.objects[currentObject]).name() == typeid(*scene.objects[2]).name())
            n = -glm::normalize(Vector3f(0.0f, 1.0f, 1.0f));*/

        //cout << n.x << " " << n.y << " " << n.z << " "<< endl;

        Material currentMat = scene.objects[currentObject]->material;

        for(glm::uint16 i = 0; i < scene.lights.size(); ++i)
        {
            Light currentLight = scene.lights[i];

            Vector3f dist = currentLight.pos - newStart;
            if(glm::dot(n, dist) <= 0.0f) continue; //ako nikako nema svjetla

            Ray lightRay(newStart, glm::normalize(dist));

            bool inShadow = false;
            t = std::numeric_limits<float>::infinity();

            for(glm::uint16 j = 0; j < scene.objects.size(); ++j)
            {
                if(scene.objects[j]->intersect(lightRay, t))
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

void render(const glm::uint16 &width, const glm::uint16 &height, const glm::uint16 &level, std::vector<Color> &img, const Scene &scene)
{
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

                Color tmp = trace(r, level, scene);

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
}

int main()
{
    const glm::uint16 width = 800, height = 600, level = 15;

    Scene scene;

    scene.materials.push_back(Material(Color(1.0f, 0.0f, 0.0f), 0.5f));
    scene.materials.push_back(Material(Color(0.0f, 1.0f, 0.0f), 1.0f)); 
    scene.materials.push_back(Material(Color(0.0f, 1.0f, 1.0f), 0.0f)); 

    scene.objects.push_back(new Sphere(Vector3f(200.0f, 300.0f, 0.0f), 75.0f, scene.materials[0]));
    scene.objects.push_back(new Sphere(Vector3f(500.0f, 300.0f, 0.0f), 75.0f, scene.materials[2]));
    scene.objects.push_back(new Plane(Vector3f(350.0f, -450.0f, 975.0f), glm::normalize(Vector3f(0.0f, 1.0f, 1.0f)), scene.materials[2]));

    //scene.objects.push_back(new Plane(Vector3f(2000.0f, 375.0f, 50.0f), glm::normalize(Vector3f(0.0f, 1.0f, 1.0f)), scene.materials[2]));

    /*Vector3f n = glm::normalize(Vector3f(0.0f, 1.0f, 1.0f));
    cout << n.x << " " << n.y << " " << n.z << " "<< endl;*/

    //scene.lights.push_back(Light(Vector3f(-100.0f, -375.0f, -600.0f), Color(1.0f, 1.0f, 1.0f)));

    scene.lights.push_back(Light(Vector3f(350.0f, -375.0f, -1200.0f), Color(1.0f, 1.0f, 1.0f)));

    std::vector<Color> img(width * height);

    render(width, height, level, img, scene);

    saveppm("scene.ppm", img, width, height);

   return 0;
}


