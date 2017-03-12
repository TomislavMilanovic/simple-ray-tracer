#include "src/raytracer.h"
#include "src/scene.h"

int main()
{
    using namespace raytracer;

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

    scene.render(width, height, level);

    return 0;
}


