#include "src/raytracer.h"
#include "src/scene.h"
#include "src/utils.h"
#include <iostream>
#include <ctime>

int main()
{
    using namespace raytracer;

    const glm::uint16 width = 800, height = 800, level = 15;

    Scene::ProjectionInfo proj_info(false);
    Scene scene(width, height, level, 1, "scene_real", Scene::png, proj_info);

    Texture earth_texture = generate_texture("textures/world.topo.bathy.200412.3x5400x2700.png");
    SphereTextureMap sphere_earth(earth_texture);

    Texture height_map = generate_texture("textures/Orange-bumpmap.png");
    SphereDisplacementMap sphere_heightmap(height_map, 0.2f, 0.001f, 0.01f);

    /*scene.objects.push_back(new Sphere(Vector3f(-0.3f, -0.75f, -1.5f), 0.3f, Material(Color(1.0f, 165.0f/255.0f, 0.0f), 0.0f), sphere_earth));

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

    scene.lights.push_back(new PointLight(Vector3f(-0.7f, 1.0f - 0.1f, -1.5f), Color(1.0f, 1.0f, 1.0f), 0.5f));
    scene.lights.push_back(new PointLight(Vector3f(0.7f, 1.0f - 0.1f, -1.5f), Color(1.0f, 1.0f, 1.0f), 0.5f));*/

    SolidObjects objs;

    objs.push_back(new Sphere(Vector3f(-0.3f, -0.75f, -1.5f), 0.3f, Material(Color(1.0f, 165.0f/255.0f, 0.0f), 0.0f), sphere_earth));

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

    scene.lights.push_back(new PointLight(Vector3f(-0.7f, 1.0f - 0.1f, -1.5f), Color(1.0f, 1.0f, 1.0f), 0.5f));
    scene.lights.push_back(new PointLight(Vector3f(0.7f, 1.0f - 0.1f, -1.5f), Color(1.0f, 1.0f, 1.0f), 0.5f));

    //scene.addAreaLightUniform(0.05f);
    //scene.addAreaLightRandom(2);

    //Old scene

    /*SolidObjects objs;

    objs.push_back(new Sphere(Vector3f(440.0f, 300.0f, 0.0f), 40.0f, Material(Color(0.0f, 1.0f, 1.0f), 5.0f)));
    objs.push_back(new Sphere(Vector3f(280.0f, 300.0f, 0.0f), 40.0f, Material(Color(0.0f, 1.0f, 1.0f), 0.0f)));
    objs.push_back(new Plane(Vector3f(350.0f, -450.0f, 975.0f), glm::normalize(Vector3f(0.0f, 1.0f, 1.0f)), Material(Color(0.0f, 1.0f, 1.0f), 0.0f)));

    scene.objects.push_back(new AABB(objs));
    scene.lights.push_back(new RealisticPointLight(Vector3f(350.0f, 100.0f, -800.0f), Color(1.0f, 1.0f, 1.0f), 10000.0f, 1.0f));*/

    std::clock_t begin = clock();
    scene.render();
    std::clock_t end = clock();

    double elapsed_secs = double(end-begin) / CLOCKS_PER_SEC;

    std::cout << "Elapsed time: " << elapsed_secs << " s" << std::endl;

    return 0;
}


