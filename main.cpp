#include "src/raytracer.h"
#include "src/scene.h"

//promijeniti pristup
#include "lib/lodepng/lodepng.h"

#include <iostream>
#include <ctime>

const raytracer::Texture generate_texture(const std::string &filename)
{
    using namespace raytracer;
    std::vector<unsigned char> buffer, image;
    lodepng::load_file(buffer, filename);

    unsigned w, h;

    unsigned error = lodepng::decode(image, w, h, buffer);

    if(error) std::cout << "dec error" << error << std::endl;
    else std::cout << "loaded" << std::endl;

    std::vector<Color>* new_texture = new std::vector<Color>(w * h);
    std::vector<Color> &texture = *new_texture;

    //texture.resize(w * h);

    for(unsigned y = 0; y < h; ++y)
    {
        for(unsigned x = 0; x < w; ++x)
        {
          //get RGBA components
          glm::uint32 r = image[4 * y * w + 4 * x + 0]; //red
          glm::uint32 g = image[4 * y * w + 4 * x + 1]; //green
          glm::uint32 b = image[4 * y * w + 4 * x + 2]; //blue

          texture[y * w + x] = Color(r / 255.0f, g / 255.0f, b / 255.0f);
        }
    }

    return Texture(texture, w, h);
}

int main()
{
    using namespace raytracer;

    const glm::uint16 width = 800, height = 600, level = 15;

    Scene scene;

    Texture earth_texture = generate_texture("textures/earth_texture_big.png");
    SphereTextureMap sphere_earth(earth_texture);

    Texture height_map = generate_texture("textures/Heightmap.png");
    SphereDisplacementMap sphere_heightmap(height_map, 55.0f);

    //scene.objects.push_back(new Sphere(Vector3f(200.0f, 300.0f, 0.0f), 75.0f, Material(Color(1.0f, 0.0f, 0.0f), 0.0f), sphere_heightmap));
    //scene.objects.push_back(new Sphere(Vector3f(500.0f, 300.0f, 0.0f), 75.0f, Material(Color(0.0f, 1.0f, 1.0f), 0.0f)));
    scene.objects.push_back(new Sphere(Vector3f(200.0f, 300.0f, 100.0f), 75.0f, Material(Color(0.0f, 1.0f, 1.0f), 0.0f), sphere_earth));
    scene.objects.push_back(new Plane(Vector3f(350.0f, -450.0f, 975.0f), glm::normalize(Vector3f(0.0f, 1.0f, 1.0f)), Material(Color(0.0f, 1.0f, 1.0f), 0.0f)));
    scene.objects.push_back(new Triangle(
                                Vector3f(210.0f, 230.0f, 230.0f),
                                Vector3f(10.0f, 10.0f, 10.0f),
                                Vector3f(10.0f, 420.0f, 10.0f),
                                Material(Color(1.0f, 1.0f, 1.0f), 1.0f)
                                ));
    scene.objects.push_back(new Triangle(
                                Vector3f(210.0f, 230.0f, 230.0f),
                                Vector3f(210.0f, 10.0f, 230.0f),
                                Vector3f(10.0f, 10.0f, 10.0f),
                                Material(Color(1.0f, 1.0f, 1.0f), 1.0f)
                                ));

    scene.lights.push_back(Light(Vector3f(350.0f, -375.0f, -1200.0f), Color(1.0f, 1.0f, 1.0f)));

    std::cout << glm::normalize(Vector3f(0.0f, 1.0f, 1.0f)).x << " "
              << glm::normalize(Vector3f(0.0f, 1.0f, 1.0f)).y << " "
              << glm::normalize(Vector3f(0.0f, 1.0f, 1.0f)).z << std::endl;

    std::clock_t begin = clock();
    scene.render(width, height, level);
    std::clock_t end = clock();

    double elapsed_secs = double(end-begin) / CLOCKS_PER_SEC;

    std::cout << elapsed_secs << std::endl;

    return 0;
}


