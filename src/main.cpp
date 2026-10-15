#include <iostream>
#include <string>

#include "triangles.hpp"
#include "graphic.hpp"

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <number>" << std::endl;
        return 1;
    }

    size_t number = std::stoul(argv[1]);
 
    TrianglesData data;
    GenerateTriangles(data, number);
    InterceptTriangles(data);

    Graphic(data);
}