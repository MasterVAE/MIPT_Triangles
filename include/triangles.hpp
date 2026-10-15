#pragma once

#include <vector>

struct Point
{
    double x;
    double y;
    double z;

    Point() : x(0.0), y(0.0), z(0.0) {}
    Point(double x_c, double y_c, double z_c) : x(x_c), y(y_c), z(z_c) {}
    Point(const Point& other) : x(other.x), y(other.y), z(other.z) {}
    Point(Point&& other) noexcept : x(other.x), y(other.y), z(other.z) {}
};

struct Triangle
{
    Point vertices[3];
    bool interception;

    Triangle(const Point& v1, const Point& v2, const Point& v3)
        : vertices{v1, v2, v3}, interception(false) {}
};

class TrianglesData
{
    std::vector<Triangle> triangles;
public:
    std::vector<Triangle>& GetTriangles() { return triangles; }
};

void GenerateTriangles(TrianglesData*, size_t);
void InterceptTriangles(TrianglesData*);