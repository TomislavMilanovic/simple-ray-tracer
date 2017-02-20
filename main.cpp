#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <limits>

#include "glm/glm.hpp"

using namespace std;

class Color
{
public:
    glm::float32 r() const {return c.x;}
    glm::float32 g() const {return c.y;}
    glm::float32 b() const {return c.z;}

    Color() : c(glm::vec3()) {}
    Color(const glm::float32 &r, const glm::float32 &g, const glm::float32 &b) {c = glm::vec3(r, g, b);}
private:
    glm::vec3 c;
};
class Light
{
public:
    glm::vec3 pos;
    Color intensity;

    Light(const glm::vec3 &p, const Color &intens) : pos(p) , intensity(intens){}
};
class Material
{
public:
    Color diffuse;
    glm::float32 reflection;

    Material(const Color &d, const glm::float32 &refl) : diffuse(d), reflection(refl) {}
};
class Ray
{
public:
    Ray() : start(glm::vec3()), dir(glm::vec3()) {}
    Ray(const glm::vec3 &st, const glm::vec3 &d) : start(st), dir(d) {/*empty*/}
    glm::vec3 start, dir;
};
class Sphere
{
public:

    Sphere(const glm::vec3 &pos, const glm::float32 &rad, const Material &mat) : position(pos), radius(rad), material(mat) {/*empty*/}

    bool intersect(const Ray &ray, glm::float32 &t) const
    {
        bool return_value = false;

        glm::float32 A = glm::dot(ray.dir, ray.dir);
        glm::vec3 dist = ray.start - position;
        glm::float32 B = 2.0f * glm::dot(ray.dir, dist);
        glm::float32 C = glm::dot(dist, dist) - (radius * radius);
        glm::float32 discr = B * B - 4.0f * A * C;

        if(discr < 0.0f)
            return_value = false;
        else
        {
            glm::float32 sqrtdiscr = glm::sqrt(discr);
            glm::float32 t0 = (-B + sqrtdiscr) / (2.0f);
            glm::float32 t1 = (-B - sqrtdiscr) / (2.0f);

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

    glm::vec3 position;
    glm::float32 radius;
    Material material;
};
class Scene
{
public:
    std::vector<Sphere> spheres;
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
        file << (unsigned char)img[i].r() << (unsigned char)img[i].g() << (unsigned char)img[i].b();
    }

    file.close();
}

Color trace(Ray &r, const glm::uint16 &lvl, const Scene &scene)
{
    glm::float32 red = 0.0f;
    glm::float32 green = 0.0f;
    glm::float32 blue = 0.0f;

    glm::float32 coef = 1.0f;
    glm::uint16 level = lvl;

    do
    {
        glm::float32 t = std::numeric_limits<glm::float32>::infinity();
        glm::int16 currentSphere = -1;

        for(glm::uint16 i = 0; i < scene.spheres.size(); ++i)
            if(scene.spheres[i].intersect(r, t)) currentSphere = i;
        if(currentSphere == -1) break;

        glm::vec3 newDir = t * r.dir; // p = t*d + p0
        glm::vec3 newStart = r.start + newDir; //tocka na sferi

        glm::vec3 n = newStart - scene.spheres[currentSphere].position;
        n = glm::normalize(n);

        Material currentMat = scene.spheres[currentSphere].material;

        for(glm::uint16 i = 0; i < scene.lights.size(); ++i)
        {
            Light currentLight = scene.lights[i];

            glm::vec3 dist = currentLight.pos - newStart;
            if(glm::dot(n, dist) <= 0.0f) continue; //ako nikako nema svjetla (s wikipedije)

            Ray lightRay(newStart, glm::normalize(dist));

            bool inShadow = false;
            t = std::numeric_limits<glm::float32>::infinity();

            for(glm::uint16 j = 0; j < scene.spheres.size(); ++j)
            {
                if(scene.spheres[j].intersect(lightRay, t))
                {
                    inShadow = true;
                    break;
                }
            }
            if(!inShadow)
            {
                glm::float32 lambert = glm::dot(lightRay.dir, n) * coef;
                red += lambert * currentLight.intensity.r() * currentMat.diffuse.r();
                green += lambert * currentLight.intensity.g() * currentMat.diffuse.g();
                blue += lambert * currentLight.intensity.b() * currentMat.diffuse.b();
            }
        }

        coef *= currentMat.reflection;

        //odbijena zraka
        r.start = newStart;
        r.dir = r.dir - ((2.0f * glm::dot(r.dir, n)) * n);
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

            r.start.x = x;
            r.start.y = y;
            r.start.z = -2000.0f;

            r.dir.x = 0.0f;
            r.dir.y = 0.0f;
            r.dir.z = 1.0f;

            img[y*width + x] = trace(r, level, scene);
        }
    }
}

int main()
{
    const glm::uint16 width = 800, height = 600, level = 15;

    Scene scene;

    scene.materials.push_back(Material(Color(1.0f, 0.0f, 0.0f), 0.2f)); //red
    scene.materials.push_back(Material(Color(0.0f, 1.0f, 0.0f), 0.5f)); //green
    scene.materials.push_back(Material(Color(0.0f, 0.0f, 1.0f), 0.9f)); //blue

    scene.spheres.push_back(Sphere(glm::vec3(200.0f, 300.0f, 0.0f), 100.0f, scene.materials[0]));
    scene.spheres.push_back(Sphere(glm::vec3(400.0f, 400.0f, 0.0f), 100.0f, scene.materials[1]));
    scene.spheres.push_back(Sphere(glm::vec3(500.0f, 140.0f, 0.0f), 100.0f, scene.materials[2]));

    scene.lights.push_back(Light(glm::vec3(0.0f, 240.0f, -100.0f), Color(1.0f, 1.0f, 1.0f)));
    scene.lights.push_back(Light(glm::vec3(3200.0f, 3000.0f, -1000.0f), Color(0.6f, 0.7f, 1.0f)));
    scene.lights.push_back(Light(glm::vec3(600.0f, 0.0f, -100.0f), Color(0.3f, 0.5f, 1.0f)));

    std::vector<Color> img(width * height);

    render(width, height, level, img, scene);

    saveppm("test2.ppm", img, width, height);

    /*std::vector< glm::vec3 > img(width * height);

    for(glm::uint16 i = 0; i < height; ++i)
    {
        for(glm::uint16 j = 0; j < width; ++j)
        {
            if(i <= height/2)
                img[i * width + j] = glm::vec3(255.0f, 255.0f, 255.0f);
            else
                img[i * width + j] = glm::vec3(56.0f, 88.0f, 123.0f);
        }
    }*/

   return 0;
}


