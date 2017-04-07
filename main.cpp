#include "src/raytracer.h"
#include "src/scene.h"

//promijeniti pristup
#include "lib/lodepng/lodepng.h"

#include <iostream>

int main()
{
    using namespace raytracer;

    const glm::uint16 width = 800, height = 600, level = 15;

    //LOAD PNG EARTH TEXTURE, FOR TEST

    std::vector<unsigned char> buffer, image;
    lodepng::load_file(buffer, "Heightmap.png");

    unsigned w, h, jump = 1;

    unsigned error = lodepng::decode(image, w, h, buffer);

    if(error) std::cout << "dec error" << error << std::endl;
    else std::cout << "loaded" << std::endl;

    std::vector<Color> better;

    better.resize(w * h);

    //plot the pixels of the PNG file
    for(unsigned y = 0; y < h; y += jump)
    {
        for(unsigned x = 0; x < w; x += jump)
        {
          //get RGBA components
          glm::uint32 r = image[4 * y * w + 4 * x + 0]; //red
          glm::uint32 g = image[4 * y * w + 4 * x + 1]; //green
          glm::uint32 b = image[4 * y * w + 4 * x + 2]; //blue
          glm::uint32 a = image[4 * y * w + 4 * x + 3]; //alpha

          better[y * w + x] = Color(r / 255.0f, g / 255.0f, b / 255.0f);

          //std::cout << better[y * w + x].r << " " << better[y * w + x].g << " " << better[y * w + x].b << " " << std::endl;
        }
    }


    //

    Scene scene;

    scene.materials.push_back(Material(Color(1.0f, 0.0f, 0.0f), 1.0f));
    scene.materials.push_back(Material(Color(0.0f, 1.0f, 0.0f), 1.0f)); 
    scene.materials.push_back(Material(Color(0.0f, 1.0f, 1.0f), 0.0f)); 

    *scene.materials[2].image = better;

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


