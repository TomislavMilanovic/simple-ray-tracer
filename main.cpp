#include "src/raytracer.h"
#include "src/scene.h"
#include "src/utils.h"
#include "time.h"
#include <iostream>
#include <ctime>

using namespace raytracer;

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
void CornellBoxScene_AABB(Scene& scene)
{
    const Texture earth_texture = generate_texture("textures/world.topo.bathy.200412.3x5400x2700.png");
    const SphereTextureMap* sphere_earth = new SphereTextureMap(earth_texture);

    Texture height_map = generate_texture("textures/maps/Heightmap.png");
    SphereDisplacementMap* sphere_heightmap = new SphereDisplacementMap(height_map, 0.05f, 0.001f, 0.01f);

    SolidObjects objs;

    objs.push_back(new Sphere(Vector3f(0.0f, -0.65f, -1.55f), 0.3f, Material(Color(0.0f, 1.0f, 1.0f), 0.5f), *sphere_heightmap));

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

int main()
{
    const glm::uint16 width = 400, height = 400, level = 15;

    Scene::ProjectionInfo proj_info(false);
    Scene scene(width, height, level, 2, "BVHScene_WithAABB2", Scene::png, proj_info);

    //CornellBoxScene_AABB(scene);
    //CornellBoxScene_WithoutAABB(scene);
    //CornellBoxScene_Normal(scene);
    //OldScene(scene);

    //BVHScene_WithoutAABB(scene);
    BVHScene_WithAABB(scene);

    struct timespec start, finish;
    double elapsed;

    clock_gettime(CLOCK_MONOTONIC, &start);
    scene.render();
    clock_gettime(CLOCK_MONOTONIC, &finish);

    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1e9; //1e9

    std::cout << "Elapsed time: " << elapsed << " s" << std::endl;

    return 0;
}


