#include "../../lib/lodepng/lodepng.h"
#include "../../lib/glm/glm.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

typedef glm::vec3 Color;

struct HexCharStruct
{
  unsigned char c;
  HexCharStruct(unsigned char _c) : c(_c) { }
};

inline std::ostream& operator<<(std::ostream& o, const HexCharStruct& hs)
{
  return (o << std::hex << (int)hs.c);
}

inline HexCharStruct hex(unsigned char _c)
{
  return HexCharStruct(_c);
}

void savepng(const std::vector<Color>& img, const int height, const int width)
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

    std::vector<unsigned char> png;

    lodepng::encode(png, tempBuffer, width, height);
    //std::string test(png.begin(), png.end());
    for(glm::uint16 i = 0; i < png.size(); ++i)
    {
        std::cout << i << "\t" << (int)png[i] << "\n";
    }

    //std::cout << test;

    //lodepng::encode("test.png", tempBuffer, width, height);
}

template<typename Out>
void split(const std::string &s, char delim, Out result) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

int main()
{
    int x = 0, y = 0;
    int width = 0, height = 0;
    std::vector<Color> img;
    int i = 0;

    for (std::string line; std::getline(std::cin, line);)
    {
        //std::cout << "\t" << line << "\n";

        std::vector<std::string> splitted = split(line, '\t');
        std::vector<std::string> splitted2 = split(splitted[1], ',');

        //std::cout << ++i << "\t" << splitted[1];

        if(std::stoi(splitted[0]) == -1)
        {
            width = std::stoi(splitted2[0]);
            height = std::stoi(splitted2[1]);
            img.resize(width * height);
        }
        else
        {
            img[y*width + x].r = std::atof(splitted2[0].c_str());
            img[y*width + x].g = std::atof(splitted2[1].c_str());
            img[y*width + x].b = std::atof(splitted2[2].c_str());
            ++x;
            if(x == width)
            {
                x = 0;
                ++y;
            }
        }
    }

    savepng(img, height, width);

    return 0;
}
