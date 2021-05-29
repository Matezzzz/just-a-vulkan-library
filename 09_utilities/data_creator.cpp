#include "data_creator.h"


Vertices& Vertices::add(const Vertices& v2){
    //add all vertices from v2 into current vertices object
    insert(end(), v2.begin(), v2.end());
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