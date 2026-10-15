#include <iostream>

#include "triangles.hpp"
#include "graphic.hpp"

int main(int argc, char* argv[])
{
    TrianglesData* data = new TrianglesData();
    GenerateTriangles(data);

    GraphicOpen();
    while(GraphicUpdate(data));
    GraphicClose();

    delete data;
}