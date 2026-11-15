#pragma once
#include <iostream>
#include <cmath>
#include <vector>
#include <cstdint>

constexpr float epsilon = 1e-6f;

inline constexpr int canonical_form[8][3] = {
    // single vertex in positive half-space -> permuntion
    {0, 0, 0}, // (000) 
    {0, 1, 2}, // (001) 
    {1, 2, 0}, // (010)
    {2, 1, 0}, // (100)
    // vertices in different half-spaces -> permution + transposition
    {2, 0, 1}, // (011)
    {1, 0, 2}, // (101)
    {0, 2, 1}, // (110)
    {0, 0, 0}  // (111)
};
namespace Triangles{

    struct Point_t {

        float x, y, z; 

        Point_t() = default;
        Point_t (float x, float y, float z) : x(x), y(y), z(z) {}

        void Valid() const {};
        void Equal() const {};

    };

    struct Vector_t {
            float coord_x, coord_y, coord_z; 

            Vector_t() = default; 
            Vector_t(float x, float y, float z) :coord_x(x), coord_y(y), coord_z(z) {}

            Vector_t(const Point_t & p1, const Point_t &p2) :
                coord_x(p2.x - p1.x),
                coord_y(p2.y - p1.y),
                coord_z(p2.z - p1.z) {}
    }; 

    struct Triangle_t {

        Point_t vertices[3];
        Vector_t edge1, edge2, edge3;

        Triangle_t(const Point_t &p1, const Point_t &p2, const Point_t &p3,
                   const Vector_t& vec1, const Vector_t& vec2, 
                   const Vector_t &vec3) : vertices{p1, p2, p3},
                   edge1(vec1), edge2(vec2), edge3(vec3) {}
    };

    struct Plane_t {

        Vector_t normal;
        float distance;
    };

    inline Vector_t ComputeCrossProduct(const Vector_t& vec1, const Vector_t& vec2){

        return Vector_t (vec1.coord_y * vec2.coord_z - vec1.coord_z * vec2.coord_y,
                         vec1.coord_z * vec2.coord_x - vec1.coord_x * vec2.coord_z,
                         vec1.coord_x * vec2.coord_y - vec1.coord_y * vec2.coord_x);
    }

    inline float ComputeDotProduct(const Vector_t& vec1, const Vector_t& vec2){

        return (vec1.coord_x * vec2.coord_x +
                vec1.coord_y * vec2.coord_y +
                vec1.coord_z * vec2.coord_z);
    }

    inline float ComputeDeterminant(const Point_t& a, const Point_t& b, 
                                    const Point_t& c, const Point_t& d){
        Vector_t ab(a, b);
        Vector_t ac(a, c);
        Vector_t ad(a, d);
        
        Vector_t normal = ComputeCrossProduct(ab, ac);
        
        return ComputeDotProduct(normal, ad);
    }

    bool isTrianglesIntersect(){
        
    }
    void ProcessComplanalTriangles(const Triangle_t& tri1, const Triangle_t& tri2) {

    }

    bool DefineDeterminantSigns(const Triangle_t& tri1, const Triangle_t& tri2,
                                const Plane_t& plane1, const Plane_t& plane2){
        //Intersaction of vertices tri2 with plane1

        float dist_p1 = ComputeDeterminant(tri2.vertices[0], tri2.vertices[1], 
                                           tri2.vertices[2], tri1.vertices[0]);
        float dist_q1 = ComputeDeterminant(tri2.vertices[0], tri2.vertices[1], 
                                           tri2.vertices[2], tri1.vertices[1]);
        float dist_r1 = ComputeDeterminant(tri2.vertices[0], tri2.vertices[1], 
                                           tri2.vertices[2], tri1.vertices[2]);

        //Intersaction of vertices tri1 with plane2

        float dist_p2 = ComputeDeterminant(tri1.vertices[0], tri1.vertices[1], 
                                           tri1.vertices[2], tri2.vertices[0]);
        float dist_q2 = ComputeDeterminant(tri1.vertices[0], tri1.vertices[1], 
                                           tri1.vertices[2], tri2.vertices[1]);
        float dist_r2 = ComputeDeterminant(tri1.vertices[0], tri1.vertices[1], 
                                           tri1.vertices[2], tri2.vertices[2]);

       if (std::abs(dist_p1) < epsilon &&  std::abs(dist_q1) < 
                     epsilon && std::abs(dist_r1) < epsilon) {
            ProcessComplanalTriangles(tri1, tri2);
            return false;
       }

       uint32_t mask1 = (dist_p1 > 0.0f) | ((dist_q1 > 0.0f) << 1) | ((dist_r1 > 0.0f) << 2);
       if (mask1 == 0 || mask1 == 7) return false;
       #if 0
           form = canonical_form[mask1]
           canonical = new Point_t[3];
           for(int i = 0; i < 3; i++) {
                canonical[i] = tri2.verticies[form[i]]
           }
           tri2.verticies = canonical

       #endif

       Point_t p1 = tri2.vertices[ canonical_form[mask1][0] ]; 
       Point_t q1 = tri2.vertices[ canonical_form[mask1][1] ];
       Point_t r1 = tri2.vertices[ canonical_form[mask1][2] ];
       
       uint32_t mask2 = (dist_p2 > 0.0f) | ((dist_q2 > 0.0f) << 1) | ((dist_r2 > 0.0f) << 2);
       if (mask2 == 0 || mask2 == 7) return false;
       
       Point_t p2 = tri1.vertices[ canonical_form[mask2][0] ]; 
       Point_t q2 = tri1.vertices[ canonical_form[mask2][1] ];
       Point_t r2 = tri1.vertices[ canonical_form[mask2][2] ];
           
       float overlap_predicate1 = ComputeDeterminant(p1, q1, p2, q2);
       float overlap_predicate2 = ComputeDeterminant(p1, r1, r2, p2);
       if (overlap_predicate1 <= epsilon && overlap_predicate2 <= epsilon) {
            return true;
       }   
    }
}
