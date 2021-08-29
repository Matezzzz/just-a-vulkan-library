#include "data_creator.h"
#include <cmath>


constexpr float PI = 3.1415926;

Vertices& Vertices::add(const Vertices& v2){
    //add all vertices from v2 into current vertices object
    insert(end(), v2.begin(), v2.end());
    return *this;
}
Vertices& Vertices::add(const glm::vec3& v){
    push_back(v.x); push_back(v.y); push_back(v.z);
    return *this;
}

Vertices VertexCreator::createPlane(uint32_t x_count, uint32_t y_count, float x_offset, float y_offset, float x_scale, float y_scale){
    Vertices vertices;
    //reserve space for all plane vertices - 6 for each plane, each holds 2 coords
    vertices.reserve(12 * x_count * y_count);
    //subdivision x and y size
    float s_x = x_scale / x_count;
    float s_y = y_scale / y_count;
    //go through all y coords
    for (uint32_t y = 0; y < y_count; y++){
        //current plane y start coord
        float y_ = y * s_y + y_offset;
        //current plane y end coord
        float y__ = y_ + s_y;
        for (uint32_t x = 0; x < x_count; x++){
            //current plane x start coord
            float x_ = x * s_x + x_offset;
            //current plane x end coord
            float x__ = x_ + s_x;
            //form two triangles out of vertices and add them to the array
            vertices.push_back(x_);  vertices.push_back(y_);
            vertices.push_back(x__); vertices.push_back(y_);
            vertices.push_back(x_);  vertices.push_back(y__);

            vertices.push_back(x__); vertices.push_back(y_);
            vertices.push_back(x_);  vertices.push_back(y__);
            vertices.push_back(x__); vertices.push_back(y__);
        }
    }
    //return created vertices
    return vertices;
}
Vertices VertexCreator::screenQuad(){
    //two triangles, one square from [-1, -1] to [1, 1]
    Vertices vertices{
        -1.f, -1.f,
         1.f, -1.f,
        -1.f,  1.f,
         1.f, -1.f,
        -1.f,  1.f,
         1.f,  1.f};
    return vertices;
}
Vertices VertexCreator::screenQuadTexCoords(){
    //two triangles, one square from [-1, -1] to [1, 1]. Now with texture coords!
    Vertices vertices{
        -1.f, -1.f, 0.f, 0.f,
         1.f, -1.f, 1.f, 0.f,
        -1.f,  1.f, 0.f, 1.f,
         1.f, -1.f, 1.f, 0.f,
        -1.f,  1.f, 0.f, 1.f,
         1.f,  1.f, 1.f, 1.f
    };
    return vertices;
}
Vertices VertexCreator::unitSphere(uint32_t angle_subdivisions, uint32_t height_subidivions){
    Vertices vertices;
    vertices.reserve(angle_subdivisions * (2 * height_subidivions - 2) * 3);
    
    for (uint32_t a = 0; a < angle_subdivisions; a++){
        vertices.add(glm::vec3(0, -1, 0));
        float w = -1 + 2.f / height_subidivions;
        vertices.add(getUnitSphereVertex(w, 2 * PI * a / angle_subdivisions));
        vertices.add(getUnitSphereVertex(w, 2 * PI * (a+1) / angle_subdivisions));
    }
    for (uint32_t h = 1; h < height_subidivions - 1; h++){
        for (uint32_t a = 0; a < angle_subdivisions; a++){
            float w1 = -1 + 2.f * h / height_subidivions;
            float w2 = -1 + 2.f * (h+1) / height_subidivions;
            float a1 = 2 * PI * a / angle_subdivisions;
            float a2 = 2 * PI * (a+1) / angle_subdivisions;
            vertices.add(getUnitSphereVertex(w1, a1)).add(getUnitSphereVertex(w1, a2)).add(getUnitSphereVertex(w2, a1))
                    .add(getUnitSphereVertex(w1, a2)).add(getUnitSphereVertex(w1, a2)).add(getUnitSphereVertex(w2, a2)); 
        }
    }
    for (uint32_t a = 0; a < angle_subdivisions; a++){
        vertices.add(glm::vec3(0, 1, 0));
        float w = 1 - 2.f / height_subidivions;
        vertices.add(getUnitSphereVertex(w, 2 * PI * a / angle_subdivisions));
        vertices.add(getUnitSphereVertex(w, 2 * PI * (a+1) / angle_subdivisions));
    }
    return vertices;
}
glm::vec3 VertexCreator::getUnitSphereVertex(float w, float a){
    float r = 1 - w * w;
    return glm::vec3(sinf(a)*r, w, cosf(a)*r);
}