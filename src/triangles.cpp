#include <random>

#include "triangles.hpp"

void GenerateTriangles(TrianglesData* data, size_t number) {
    if (!data) return;

    auto& triangles = data->GetTriangles();
    triangles.reserve(number);

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<double> dis(0.0, 1.0);

    for (size_t i = 0; i < number; ++i)
    {
        Point v1(dis(gen), dis(gen), dis(gen));
        Point v2(dis(gen), dis(gen), dis(gen));
        Point v3(dis(gen), dis(gen), dis(gen));

        triangles.emplace_back(v1, v2, v3);
    }
}

void InterceptTriangles(TrianglesData* data) {
    // TODO calculate interceptions of trinagles
}