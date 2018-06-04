#include "../../lib/lodepng/lodepng.h"
#include "../../lib/glm/glm.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

typedef glm::vec3 Color;

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
    for (std::string line; std::getline(std::cin, line);)
    {
        std::vector<std::string> splitted = split(line, '\t');
        std::cout << (unsigned char)std::stoi(splitted[1]);
    }

    return 0;
}
