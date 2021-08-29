#ifndef VERTEX_CREATOR_H
#define VERTEX_CREATOR_H

#include <vector>
#include <stdint.h>
#include <glm/glm.hpp>


using std::vector;


/**
 * Vertices
 *  - holds array of float information for vertices
 */
class Vertices : public vector<float>{
public:
    using vector::vector;
    //add together two vectors of vertex data
    Vertices& add(const Vertices& v2);

    Vertices& add(const glm::vec3& v);
};


/**
 * VertexCreator
 *  - Class with static functions for automatically creating vertices for primitives
 */
class VertexCreator{
public:
    /**
     * Create a horizontal plane. Each vertex contains only X and Y coordinates.
     * @param x_count count of subdivisions in the x direction
     * @param y_count ^
     * @param x_offset starting x position of plane
     * @param y_offset ^
     * @param x_scale plane length in the x direction
     * @param y_scale ^
     */
    static Vertices createPlane(uint32_t x_count = 1, uint32_t y_count = 1, float x_offset = 1.f, float y_offset = 1.f, float x_scale = 1.f, float y_scale = 1.f);
    /**
     * Create screen quad composed of two triangles - rectangle from (-1, -1) to (1, 1). It has only XY coords.
     */
    static Vertices screenQuad();
    /**
     * Create screen quad composed of two triangles - rectangle from (-1, -1) to (1, 1). It has XY coords and texture coords from 0 to 1.
     */
    static Vertices screenQuadTexCoords();

    static Vertices unitSphere(uint32_t angle_subdivisions, uint32_t height_subidivions);
private:
    static glm::vec3 getUnitSphereVertex(float w, float a);
};



/**
 * Texture3D
 *  - class to quickly create and edit a 3D texture
 */
template<typename T>
class Texture3D : public vector<T>{
    //iterate over texture size in all dimensions
    #define ITER_XYZ(from_x, from_y, from_z, to_x, to_y, to_z, stuff_to_do)\
    for (uint32_t x = from_x; x < to_x; x++){\
        for (uint32_t y = from_y; y < to_y; y++){\
            for (uint32_t z = from_z; z < to_z; z++){\
                stuff_to_do\
            }\
        }\
    }
public:
    uint32_t width, height, depth;
    //width, height, depth, the value to fill the whole texture with
    Texture3D(uint32_t width_, uint32_t height_, uint32_t depth_, T default_value = 0) : vector<T>(width_*height_*depth_, default_value), width(width_), height(height_), depth(depth_)
    {}
    //fill volume with given start and end coords
    Texture3D& fillWith(uint32_t from_x, uint32_t from_y, uint32_t from_z, uint32_t to_x, uint32_t to_y, uint32_t to_z, T value){
        ITER_XYZ(from_x, from_y, from_z, to_x, to_y, to_z,
            (*this)[getIndex(x, y, z)] = value;
        )
        return *this;
    }
    //iterate over all texels and set value in those where the condition holds
    Texture3D& fillBasedOnRule(bool (*rule)(uint32_t posx, uint32_t posy, uint32_t posz, T current_val), T value){
        ITER_XYZ(0, 0, 0, width, height, depth,
            if (rule(x, y, z, (*this)[getIndex(x, y, z)]))
            {
                (*this)[getIndex(x, y, z)] = value;
            }
        )
        return *this;
    }
    //get index of texel in internal array
    uint32_t getIndex(uint32_t x, uint32_t y, uint32_t z){
        return ((z * height) + y) * width + x;
    }
    uint32_t sizeBytes() const{
        return vector<T>::size() * sizeof(T);
    }
};


#endif