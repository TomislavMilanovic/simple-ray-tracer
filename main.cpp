#include "src/raytracer.h"
#include "src/scene.h"
#include "src/utils.h"
#include "time.h"
#include <iostream>
#include <fstream>
#include <ctime>

#include <nlohmann/json.hpp>

using namespace raytracer;
using json = nlohmann::json;

void CornellBoxScene_Normal(Scene& scene)
{
    const Texture earth_texture = generate_texture("textures/world.topo.bathy.200412.3x5400x2700.png");
    const SphereTextureMap* sphere_earth = new SphereTextureMap(earth_texture);

    Texture height_map = generate_texture("textures/maps/Heightmap.png");
    SphereDisplacementMap* sphere_heightmap = new SphereDisplacementMap(height_map, 0.05f, 0.001f, 0.01f);

    scene.objects.push_back(new Sphere(Vector3f(-0.5f, -0.70f, -1.65f), 0.3f, Material(Color(0.0f, 1.0, 1.0f), 0.5f)));
    scene.objects.push_back(new Sphere(Vector3f(0.5f, -0.70f, -1.3f), 0.3f, Material(Color(0.0f, 1.0, 0.0f), 0.0f)));

    const float wall_factor = -2.0f;
    //left wall
    scene.objects.push_back(new Triangle(
                                    Vector3f(-1.0f, 1.0f, wall_factor),
                                    Vector3f(-1.0f, -1.0f, -1.0f),
                                    Vector3f(-1.0f, -1.0f, wall_factor),
                                    Material(Color(1.0f, 0.0f, 0.0f), 1.0f)
                                    ));
    scene.objects.push_back(new Triangle(
                                    Vector3f(-1.0f, 1.0f, wall_factor),
                                    Vector3f(-1.0f, 1.0f, -1.0f),
                                    Vector3f(-1.0f, -1.0f, -1.0f),
                                    Material(Color(1.0f, 0.0f, 0.0f), 1.0f)
                                    ));

    //right wall
    scene.objects.push_back(new Triangle(
                                    Vector3f(1.0f, 1.0f, wall_factor),
                                    Vector3f(1.0f, -1.0f, wall_factor),
                                    Vector3f(1.0f, -1.0f, -1.0f),
                                    Material(Color(0.0f, 1.0f, 0.0f), 0.3f)
                                    ));
    scene.objects.push_back(new Triangle(
                                    Vector3f(1.0f, 1.0f, wall_factor),
                                    Vector3f(1.0f, -1.0f, -1.0f),
                                    Vector3f(1.0f, 1.0f, -1.0f),
                                    Material(Color(0.0f, 1.0f, 0.0f), 0.3f)
                                    ));

    //floor
    scene.objects.push_back(new Triangle(
                                    Vector3f(1.0f, -1.0f, wall_factor),
                                    Vector3f(-1.0f, -1.0f, wall_factor),
                                    Vector3f(-1.0f, -1.0f, -1.0f),
                                    Material(Color(1.0f, 1.0f, 1.0f), 0.0f)
                                    ));
    scene.objects.push_back(new Triangle(
                                    Vector3f(1.0f, -1.0f, wall_factor),
                                    Vector3f(-1.0f, -1.0f, -1.0f),
                                    Vector3f(1.0f, -1.0f, -1.0f),
                                    Material(Color(1.0f, 1.0f, 1.0f), 0.0f)
                                    ));

    //back wall
    scene.objects.push_back(new Triangle(
                                    Vector3f(1.0f, 1.0f, wall_factor),
                                    Vector3f(-1.0f, -1.0f, wall_factor),
                                    Vector3f(1.0f, -1.0f, wall_factor),
                                    Material(Color(1.0f, 1.0f, 1.0f), 0.0f)
                                    ));
    scene.objects.push_back(new Triangle(
                                    Vector3f(1.0f, 1.0f, wall_factor),
                                    Vector3f(-1.0f, 1.0f, wall_factor),
                                    Vector3f(-1.0f, -1.0f, wall_factor),
                                    Material(Color(1.0f, 1.0f, 1.0f), 0.0f)
                                    ));

    //ceiling
    scene.objects.push_back(new Triangle(
                                    Vector3f(-1.0f, 1.0f, -1.0f),
                                    Vector3f(-1.0f, 1.0f, wall_factor),
                                    Vector3f(1.0f, 1.0f, wall_factor),
                                    Material(Color(1.0f, 1.0f, 1.0f), 0.0f)
                                    ));
    scene.objects.push_back(new Triangle(
                                    Vector3f(1.0f, 1.0f, wall_factor),
                                    Vector3f(1.0f, 1.0f, -1.0f),
                                    Vector3f(-1.0f, 1.0f, -1.0f),
                                    Material(Color(1.0f, 1.0f, 1.0f), 0.0f)
                                    ));

    //scene.lights.push_back(new PointLight(Vector3f(-0.7f, 1.0f - 0.1f, -1.5f), Color(1.0f, 1.0f, 1.0f), 0.5f));
    //scene.lights.push_back(new PointLight(Vector3f(0.7f, 1.0f - 0.1f, -1.5f), Color(1.0f, 1.0f, 1.0f), 0.5f));

    scene.addAreaLightUniform(0.1f);
    //scene.addAreaLightRandom(2);
}
void CornellBoxScene_AABB(Scene& scene, const std::string path)
{
    const Texture earth_texture = generate_texture(path + "textures/world.topo.bathy.200412.3x5400x2700.png");
    const SphereTextureMap* sphere_earth = new SphereTextureMap(earth_texture);

    /*Texture height_map = generate_texture("textures/maps/Heightmap.png");
    SphereDisplacementMap* sphere_heightmap = new SphereDisplacementMap(height_map, 0.05f, 0.001f, 0.01f);*/

    SolidObjects objs;

    objs.push_back(new Sphere(Vector3f(0.0f, -0.65f, -1.55f), 0.3f, Material(Color(0.0f, 1.0f, 1.0f), 0.5f), *sphere_earth));

    const float wall_factor = -2.0f;
    //left wall
    objs.push_back(new Triangle(
                                    Vector3f(-1.0f, 1.0f, wall_factor),
                                    Vector3f(-1.0f, -1.0f, -1.0f),
                                    Vector3f(-1.0f, -1.0f, wall_factor),
                                    Material(Color(1.0f, 0.0f, 0.0f), 1.0f)
                                    ));
    objs.push_back(new Triangle(
                                    Vector3f(-1.0f, 1.0f, wall_factor),
                                    Vector3f(-1.0f, 1.0f, -1.0f),
                                    Vector3f(-1.0f, -1.0f, -1.0f),
                                    Material(Color(1.0f, 0.0f, 0.0f), 1.0f)
                                    ));

    //right wall
    objs.push_back(new Triangle(
                                    Vector3f(1.0f, 1.0f, wall_factor),
                                    Vector3f(1.0f, -1.0f, wall_factor),
                                    Vector3f(1.0f, -1.0f, -1.0f),
                                    Material(Color(0.0f, 1.0f, 0.0f), 0.3f)
                                    ));
    objs.push_back(new Triangle(
                                    Vector3f(1.0f, 1.0f, wall_factor),
                                    Vector3f(1.0f, -1.0f, -1.0f),
                                    Vector3f(1.0f, 1.0f, -1.0f),
                                    Material(Color(0.0f, 1.0f, 0.0f), 0.3f)
                                    ));

    //floor
    objs.push_back(new Triangle(
                                    Vector3f(1.0f, -1.0f, wall_factor),
                                    Vector3f(-1.0f, -1.0f, wall_factor),
                                    Vector3f(-1.0f, -1.0f, -1.0f),
                                    Material(Color(1.0f, 1.0f, 1.0f), 0.0f)
                                    ));
    objs.push_back(new Triangle(
                                    Vector3f(1.0f, -1.0f, wall_factor),
                                    Vector3f(-1.0f, -1.0f, -1.0f),
                                    Vector3f(1.0f, -1.0f, -1.0f),
                                    Material(Color(1.0f, 1.0f, 1.0f), 0.0f)
                                    ));

    //back wall
    objs.push_back(new Triangle(
                                    Vector3f(1.0f, 1.0f, wall_factor),
                                    Vector3f(-1.0f, -1.0f, wall_factor),
                                    Vector3f(1.0f, -1.0f, wall_factor),
                                    Material(Color(1.0f, 1.0f, 1.0f), 0.0f)
                                    ));
    objs.push_back(new Triangle(
                                    Vector3f(1.0f, 1.0f, wall_factor),
                                    Vector3f(-1.0f, 1.0f, wall_factor),
                                    Vector3f(-1.0f, -1.0f, wall_factor),
                                    Material(Color(1.0f, 1.0f, 1.0f), 0.0f)
                                    ));

    //ceiling
    objs.push_back(new Triangle(
                                    Vector3f(-1.0f, 1.0f, -1.0f),
                                    Vector3f(-1.0f, 1.0f, wall_factor),
                                    Vector3f(1.0f, 1.0f, wall_factor),
                                    Material(Color(1.0f, 1.0f, 1.0f), 0.0f)
                                    ));
    objs.push_back(new Triangle(
                                    Vector3f(1.0f, 1.0f, wall_factor),
                                    Vector3f(1.0f, 1.0f, -1.0f),
                                    Vector3f(-1.0f, 1.0f, -1.0f),
                                    Material(Color(1.0f, 1.0f, 1.0f), 0.0f)
                                    ));

    scene.objects.push_back(new AABB(objs));

    scene.lights.push_back(new PointLight(Vector3f(-0.7f, 1.0f - 0.1f, -1.3f), Color(1.0f, 1.0f, 1.0f), 0.5f));
    scene.lights.push_back(new PointLight(Vector3f(0.7f, 1.0f - 0.1f, -1.3f), Color(1.0f, 1.0f, 1.0f), 0.5f));

    //scene.addAreaLightUniform(0.085f);
    //scene.addAreaLightRandom(2);
}
void CornellBoxScene_WithoutAABB(Scene& scene)
{
    const Texture earth_texture = generate_texture("textures/world.topo.bathy.200412.3x5400x2700.png");
    const SphereTextureMap* sphere_earth = new SphereTextureMap(earth_texture);

    //Texture height_map = generate_texture("textures/Orange-bumpmap.png");
    //SphereDisplacementMap sphere_heightmap(height_map, 0.2f, 0.001f, 0.01f);

    scene.objects.push_back(new Sphere(Vector3f(-0.2f, -0.65f, -1.4f), 0.3f, Material(Color(1.0f, 165.0f/255.0f, 0.0f), 0.0f), *sphere_earth));

    const float wall_factor = -2.0f;
    //left wall
    scene.objects.push_back(new Triangle(
                                    Vector3f(-1.0f, 1.0f, wall_factor),
                                    Vector3f(-1.0f, -1.0f, -1.0f),
                                    Vector3f(-1.0f, -1.0f, wall_factor),
                                    Material(Color(1.0f, 0.0f, 0.0f), 1.0f)
                                    ));
    scene.objects.push_back(new Triangle(
                                    Vector3f(-1.0f, 1.0f, wall_factor),
                                    Vector3f(-1.0f, 1.0f, -1.0f),
                                    Vector3f(-1.0f, -1.0f, -1.0f),
                                    Material(Color(1.0f, 0.0f, 0.0f), 1.0f)
                                    ));

    //right wall
    scene.objects.push_back(new Triangle(
                                    Vector3f(1.0f, 1.0f, wall_factor),
                                    Vector3f(1.0f, -1.0f, wall_factor),
                                    Vector3f(1.0f, -1.0f, -1.0f),
                                    Material(Color(0.0f, 1.0f, 0.0f), 0.3f)
                                    ));
    scene.objects.push_back(new Triangle(
                                    Vector3f(1.0f, 1.0f, wall_factor),
                                    Vector3f(1.0f, -1.0f, -1.0f),
                                    Vector3f(1.0f, 1.0f, -1.0f),
                                    Material(Color(0.0f, 1.0f, 0.0f), 0.3f)
                                    ));

    //floor
    scene.objects.push_back(new Triangle(
                                    Vector3f(1.0f, -1.0f, wall_factor),
                                    Vector3f(-1.0f, -1.0f, wall_factor),
                                    Vector3f(-1.0f, -1.0f, -1.0f),
                                    Material(Color(1.0f, 1.0f, 1.0f), 0.0f)
                                    ));
    scene.objects.push_back(new Triangle(
                                    Vector3f(1.0f, -1.0f, wall_factor),
                                    Vector3f(-1.0f, -1.0f, -1.0f),
                                    Vector3f(1.0f, -1.0f, -1.0f),
                                    Material(Color(1.0f, 1.0f, 1.0f), 0.0f)
                                    ));

    //back wall
    scene.objects.push_back(new Triangle(
                                    Vector3f(1.0f, 1.0f, wall_factor),
                                    Vector3f(-1.0f, -1.0f, wall_factor),
                                    Vector3f(1.0f, -1.0f, wall_factor),
                                    Material(Color(1.0f, 1.0f, 1.0f), 0.0f)
                                    ));
    scene.objects.push_back(new Triangle(
                                    Vector3f(1.0f, 1.0f, wall_factor),
                                    Vector3f(-1.0f, 1.0f, wall_factor),
                                    Vector3f(-1.0f, -1.0f, wall_factor),
                                    Material(Color(1.0f, 1.0f, 1.0f), 0.0f)
                                    ));

    //ceiling
    scene.objects.push_back(new Triangle(
                                    Vector3f(-1.0f, 1.0f, -1.0f),
                                    Vector3f(-1.0f, 1.0f, wall_factor),
                                    Vector3f(1.0f, 1.0f, wall_factor),
                                    Material(Color(1.0f, 1.0f, 1.0f), 0.0f)
                                    ));
    scene.objects.push_back(new Triangle(
                                    Vector3f(1.0f, 1.0f, wall_factor),
                                    Vector3f(1.0f, 1.0f, -1.0f),
                                    Vector3f(-1.0f, 1.0f, -1.0f),
                                    Material(Color(1.0f, 1.0f, 1.0f), 0.0f)
                                    ));

    scene.lights.push_back(new PointLight(Vector3f(-0.7f, 1.0f - 0.1f, -1.3f), Color(1.0f, 1.0f, 1.0f), 0.5f));
    scene.lights.push_back(new PointLight(Vector3f(0.7f, 1.0f - 0.1f, -1.3f), Color(1.0f, 1.0f, 1.0f), 0.5f));

    //scene.addAreaLightUniform(0.08f);
    //scene.addAreaLightRandom(2);
}
void OldScene(Scene& scene)
{
    SolidObjects objs;

    objs.push_back(new Sphere(Vector3f(140.0f, 200.0f, 0.0f), 40.0f, Material(Color(1.0f, 1.0f, 0.0f), 0.0f)));
    objs.push_back(new Sphere(Vector3f(280.0f, 200.0f, 0.0f), 40.0f, Material(Color(0.0f, 1.0f, 1.0f), 1.0f)));

    /*objs.push_back(new Triangle(Vector3f(480.0f, 500.0f, 0.0f),
                                Vector3f(440.0f, 500.0f, 0.0f),
                                Vector3f(460.0f, 400.0f, 0.0f),
                                Material(Color(1.0f, 1.0f, 1.0f), 0.0f)
                                ));

    objs.push_back(new Triangle(Vector3f(480.0f + 100.0f, 500.0f, 0.0f),
                                Vector3f(460.0f + 100.0f, 400.0f, 0.0f),
                                Vector3f(440.0f + 100.0f, 500.0f, 0.0f),
                                Material(Color(1.0f, 1.0f, 1.0f), 0.0f)
                                ));*/

    scene.objects.push_back(new Plane(Vector3f(350.0f, -450.0f, 975.0f), glm::normalize(Vector3f(0.0f, 1.0f, 1.0f)), Material(Color(0.0f, 1.0f, 1.0f), 0.0f)));

    scene.objects.push_back(new AABB(objs));
    scene.lights.push_back(new RealisticPointLight(Vector3f(210.0f, 50.0f, -800.0f), Color(1.0f, 1.0f, 1.0f), 10000.0f, 1.0f));
}

void BVHScene_WithoutAABB(Scene& scene)
{
    scene.objects.push_back(new Sphere(Vector3f(-0.5f, -0.70f, -1.35f), 0.3f, Material(Color(0.0f, 1.0f, 1.0f), 0.5f)));
    scene.objects.push_back(new Sphere(Vector3f(0.5f, -0.70f, -1.65f), 0.3f, Material(Color(0.0f, 1.0f, 0.0f), 0.0f)));
    scene.objects.push_back(new Sphere(Vector3f(0.0f, -0.85f, -1.65f), 0.15f, Material(Color(0.0f, 1.0f, 0.5f), 0.2f)));
    scene.objects.push_back(new Sphere(Vector3f(-0.5f, -0.10f, -1.35f), 0.3f, Material(Color(1.0f, 1.0f, 0.2f), 0.1f)));
    scene.objects.push_back(new Sphere(Vector3f(-0.5f, 0.50f, -1.35f), 0.3f, Material(Color(1.0f, 192.0f/255.0f, 203.0f/255.0f), 0.7f)));

    const float wall_factor = -2.0f;
    //left wall
    scene.objects.push_back(new Triangle(
                                    Vector3f(-1.0f, 1.0f, wall_factor),
                                    Vector3f(-1.0f, -1.0f, -1.0f),
                                    Vector3f(-1.0f, -1.0f, wall_factor),
                                    Material(Color(1.0f, 0.0f, 0.0f), 1.0f)
                                    ));
    scene.objects.push_back(new Triangle(
                                    Vector3f(-1.0f, 1.0f, wall_factor),
                                    Vector3f(-1.0f, 1.0f, -1.0f),
                                    Vector3f(-1.0f, -1.0f, -1.0f),
                                    Material(Color(1.0f, 0.0f, 0.0f), 1.0f)
                                    ));

    //right wall
    scene.objects.push_back(new Triangle(
                                    Vector3f(1.0f, 1.0f, wall_factor),
                                    Vector3f(1.0f, -1.0f, wall_factor),
                                    Vector3f(1.0f, -1.0f, -1.0f),
                                    Material(Color(0.0f, 1.0f, 0.0f), 0.3f)
                                    ));
    scene.objects.push_back(new Triangle(
                                    Vector3f(1.0f, 1.0f, wall_factor),
                                    Vector3f(1.0f, -1.0f, -1.0f),
                                    Vector3f(1.0f, 1.0f, -1.0f),
                                    Material(Color(0.0f, 1.0f, 0.0f), 0.3f)
                                    ));

    //floor
    scene.objects.push_back(new Triangle(
                                    Vector3f(1.0f, -1.0f, wall_factor),
                                    Vector3f(-1.0f, -1.0f, wall_factor),
                                    Vector3f(-1.0f, -1.0f, -1.0f),
                                    Material(Color(1.0f, 1.0f, 1.0f), 0.0f)
                                    ));
    scene.objects.push_back(new Triangle(
                                    Vector3f(1.0f, -1.0f, wall_factor),
                                    Vector3f(-1.0f, -1.0f, -1.0f),
                                    Vector3f(1.0f, -1.0f, -1.0f),
                                    Material(Color(1.0f, 1.0f, 1.0f), 0.0f)
                                    ));

    //back wall
    scene.objects.push_back(new Triangle(
                                    Vector3f(1.0f, 1.0f, wall_factor),
                                    Vector3f(-1.0f, -1.0f, wall_factor),
                                    Vector3f(1.0f, -1.0f, wall_factor),
                                    Material(Color(1.0f, 1.0f, 1.0f), 0.0f)
                                    ));
    scene.objects.push_back(new Triangle(
                                    Vector3f(1.0f, 1.0f, wall_factor),
                                    Vector3f(-1.0f, 1.0f, wall_factor),
                                    Vector3f(-1.0f, -1.0f, wall_factor),
                                    Material(Color(1.0f, 1.0f, 1.0f), 0.0f)
                                    ));

    //ceiling
    scene.objects.push_back(new Triangle(
                                    Vector3f(-1.0f, 1.0f, -1.0f),
                                    Vector3f(-1.0f, 1.0f, wall_factor),
                                    Vector3f(1.0f, 1.0f, wall_factor),
                                    Material(Color(1.0f, 1.0f, 1.0f), 0.0f)
                                    ));
    scene.objects.push_back(new Triangle(
                                    Vector3f(1.0f, 1.0f, wall_factor),
                                    Vector3f(1.0f, 1.0f, -1.0f),
                                    Vector3f(-1.0f, 1.0f, -1.0f),
                                    Material(Color(1.0f, 1.0f, 1.0f), 0.0f)
                                    ));

    //scene.lights.push_back(new PointLight(Vector3f(-0.7f, 1.0f - 0.1f, -1.5f), Color(1.0f, 1.0f, 1.0f), 0.5f));
    //scene.lights.push_back(new PointLight(Vector3f(0.7f, 1.0f - 0.1f, -1.5f), Color(1.0f, 1.0f, 1.0f), 0.5f));

    scene.addAreaLightUniform(0.08f);
}
void BVHScene_WithAABB(Scene& scene)
{
    SolidObjects objs;

    objs.push_back(new Sphere(Vector3f(-0.5f, -0.70f, -1.35f), 0.3f, Material(Color(0.0f, 1.0f, 1.0f), 0.5f)));
    objs.push_back(new Sphere(Vector3f(0.5f, -0.70f, -1.65f), 0.3f, Material(Color(0.0f, 1.0f, 0.0f), 0.0f)));
    objs.push_back(new Sphere(Vector3f(0.0f, -0.85f, -1.65f), 0.15f, Material(Color(0.0f, 1.0f, 0.5f), 0.2f)));
    objs.push_back(new Sphere(Vector3f(-0.5f, -0.10f, -1.35f), 0.3f, Material(Color(1.0f, 1.0f, 0.2f), 0.1f)));
    objs.push_back(new Sphere(Vector3f(-0.5f, 0.50f, -1.35f), 0.3f, Material(Color(1.0f, 192.0f/255.0f, 203.0f/255.0f), 0.7f)));

    const float wall_factor = -2.0f;
    //left wall
    objs.push_back(new Triangle(
                                    Vector3f(-1.0f, 1.0f, wall_factor),
                                    Vector3f(-1.0f, -1.0f, -1.0f),
                                    Vector3f(-1.0f, -1.0f, wall_factor),
                                    Material(Color(1.0f, 0.0f, 0.0f), 1.0f)
                                    ));
    objs.push_back(new Triangle(
                                    Vector3f(-1.0f, 1.0f, wall_factor),
                                    Vector3f(-1.0f, 1.0f, -1.0f),
                                    Vector3f(-1.0f, -1.0f, -1.0f),
                                    Material(Color(1.0f, 0.0f, 0.0f), 1.0f)
                                    ));

    //right wall
    objs.push_back(new Triangle(
                                    Vector3f(1.0f, 1.0f, wall_factor),
                                    Vector3f(1.0f, -1.0f, wall_factor),
                                    Vector3f(1.0f, -1.0f, -1.0f),
                                    Material(Color(0.0f, 1.0f, 0.0f), 0.3f)
                                    ));
    objs.push_back(new Triangle(
                                    Vector3f(1.0f, 1.0f, wall_factor),
                                    Vector3f(1.0f, -1.0f, -1.0f),
                                    Vector3f(1.0f, 1.0f, -1.0f),
                                    Material(Color(0.0f, 1.0f, 0.0f), 0.3f)
                                    ));

    //floor
    objs.push_back(new Triangle(
                                    Vector3f(1.0f, -1.0f, wall_factor),
                                    Vector3f(-1.0f, -1.0f, wall_factor),
                                    Vector3f(-1.0f, -1.0f, -1.0f),
                                    Material(Color(1.0f, 1.0f, 1.0f), 0.0f)
                                    ));
    objs.push_back(new Triangle(
                                    Vector3f(1.0f, -1.0f, wall_factor),
                                    Vector3f(-1.0f, -1.0f, -1.0f),
                                    Vector3f(1.0f, -1.0f, -1.0f),
                                    Material(Color(1.0f, 1.0f, 1.0f), 0.0f)
                                    ));

    //back wall
    objs.push_back(new Triangle(
                                    Vector3f(1.0f, 1.0f, wall_factor),
                                    Vector3f(-1.0f, -1.0f, wall_factor),
                                    Vector3f(1.0f, -1.0f, wall_factor),
                                    Material(Color(1.0f, 1.0f, 1.0f), 0.0f)
                                    ));
    objs.push_back(new Triangle(
                                    Vector3f(1.0f, 1.0f, wall_factor),
                                    Vector3f(-1.0f, 1.0f, wall_factor),
                                    Vector3f(-1.0f, -1.0f, wall_factor),
                                    Material(Color(1.0f, 1.0f, 1.0f), 0.0f)
                                    ));

    //ceiling
    objs.push_back(new Triangle(
                                    Vector3f(-1.0f, 1.0f, -1.0f),
                                    Vector3f(-1.0f, 1.0f, wall_factor),
                                    Vector3f(1.0f, 1.0f, wall_factor),
                                    Material(Color(1.0f, 1.0f, 1.0f), 0.0f)
                                    ));
    objs.push_back(new Triangle(
                                    Vector3f(1.0f, 1.0f, wall_factor),
                                    Vector3f(1.0f, 1.0f, -1.0f),
                                    Vector3f(-1.0f, 1.0f, -1.0f),
                                    Material(Color(1.0f, 1.0f, 1.0f), 0.0f)
                                    ));

    //scene.lights.push_back(new PointLight(Vector3f(-0.7f, 1.0f - 0.1f, -1.5f), Color(1.0f, 1.0f, 1.0f), 0.5f));
    //scene.lights.push_back(new PointLight(Vector3f(0.7f, 1.0f - 0.1f, -1.5f), Color(1.0f, 1.0f, 1.0f), 0.5f));

    scene.objects.push_back(new AABB(objs));

    scene.addAreaLightUniform(0.08f);
}

const Scene* buildSceneWithJSON(const json& scene_json, const std::string& path)
{
    const glm::uint16
            width = scene_json["settings"]["width"],
            height = scene_json["settings"]["height"],
            level = scene_json["settings"]["level"],
            antialiasing = scene_json["settings"]["antialiasing"];

    Scene::ProjectionInfo* proj_info = new Scene::ProjectionInfo(false);
    Scene* scene = new Scene(width, height, level, antialiasing, "TEST", Scene::png, *proj_info);
    SolidObjects* objs = new SolidObjects();

    for(int i = 0; i < scene_json["objects"].size(); ++i)
    {
        if(scene_json["objects"][i]["type"] == "Sphere")
        {
            const Texture earth_texture = generate_texture(path + scene_json["objects"][i]["texture"].get<std::string>());
            const SphereTextureMap* sphere_earth = new SphereTextureMap(earth_texture);

            objs->push_back(new Sphere(Vector3f(
                                          scene_json["objects"][i]["position"]["x"],
                                          scene_json["objects"][i]["position"]["y"],
                                          scene_json["objects"][i]["position"]["z"]),
                                          scene_json["objects"][i]["radius"],
                            Material(Color(
                                         scene_json["objects"][i]["material"]["color"]["r"],
                                         scene_json["objects"][i]["material"]["color"]["g"],
                                         scene_json["objects"][i]["material"]["color"]["b"]),
                                         scene_json["objects"][i]["material"]["reflectivity"]), *sphere_earth));
        }
        else if(scene_json["objects"][i]["type"] == "Triangle")
        {
            objs->push_back(new Triangle(
                               Vector3f(scene_json["objects"][i]["position"][0]["x"],
                                        scene_json["objects"][i]["position"][0]["y"],
                                        scene_json["objects"][i]["position"][0]["z"]),
                               Vector3f(scene_json["objects"][i]["position"][1]["x"],
                                        scene_json["objects"][i]["position"][1]["y"],
                                        scene_json["objects"][i]["position"][1]["z"]),
                               Vector3f(scene_json["objects"][i]["position"][2]["x"],
                                        scene_json["objects"][i]["position"][2]["y"],
                                        scene_json["objects"][i]["position"][2]["z"]),
                               Material(Color(
                                        scene_json["objects"][i]["material"]["color"]["r"],
                                        scene_json["objects"][i]["material"]["color"]["g"],
                                        scene_json["objects"][i]["material"]["color"]["b"]),
                                        scene_json["objects"][i]["material"]["reflectivity"])
                                        ));
        }
    }

    scene->objects.push_back(new AABB(*objs));

    for(int i = 0; i < scene_json["lights"].size(); ++i)
    {
        if(scene_json["lights"][i]["type"] == "PointLight")
        {
            scene->lights.push_back(new PointLight(
                                       Vector3f(scene_json["lights"][i]["position"]["x"],
                                                scene_json["lights"][i]["position"]["y"],
                                                scene_json["lights"][i]["position"]["z"]),
                                       Color(scene_json["lights"][i]["color"]["r"],
                                             scene_json["lights"][i]["color"]["g"],
                                             scene_json["lights"][i]["color"]["b"]),
                                       scene_json["lights"][i]["intensity"]));
        }
    }

    return scene;
}

int main(int argc, char *argv[])
{
    if(argc != 4)
    {
        std::cout << "Invalid number of arguments!" << std::endl;
        std::cout << "Usage: ./simpleraytracer relative_path_to_input start_row end_row" << std::endl;
        return 1;
    }

    std::string path = argv[1];
    int start_row = std::stoi(argv[2]);
    int end_row = std::stoi(argv[3]);

    std::ifstream scene_file(path + "scene.json");
    json scene_json;
    scene_file >> scene_json;

    //std::string s = scene_json.dump();
    //std::cout << "JSON: " << s << std::endl;

    Scene* scene = buildSceneWithJSON(scene_json, path);

    //CornellBoxScene_AABB(scene, path);
    //CornellBoxScene_WithoutAABB(scene);
    //CornellBoxScene_Normal(scene);
    //OldScene(scene);

    //BVHScene_WithoutAABB(scene);
    //BVHScene_WithAABB(scene);

    struct timespec start, finish;
    double elapsed;

    clock_gettime(CLOCK_MONOTONIC, &start);
    const std::vector<std::string> img_part = scene->string_render(start_row, end_row);
    std::cout << "COLOR LIST START" << std::endl;
    for(int i = 0; i < img_part.size(); ++i)
    {
        std::cout << img_part[i];
    }
    //scene->render();
    std::cout << "COLOR LIST END" << std::endl;
    clock_gettime(CLOCK_MONOTONIC, &finish);

    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1e9; //1e9

    std::cout << "Elapsed time: " << elapsed << " s" << std::endl;

    return 0;
}


