#ifndef IMAGE_LOAD_H
#define IMAGE_LOAD_H


#include "../00_base/vulkan_base.h"
#include "../04_memory_objects/image.h"

/**
 * ImageData
 *  - Class for loading a texture from a file. stb_image.h is used to load images from various file formats.
 */
class ImageData{
public:
    //pointer to image bytes
    vector<uint8_t> data;
    //width, height, number of components
    int width, height, comp;
    //load image from given file with expected number of components
    ImageData(const string& filename, uint32_t comp_c);
    //pixel count
    uint32_t size();
    //deallocate image data
};


//all PBR image types
enum ImageType{
    IMAGE_TYPE_DIFFUSE,
    IMAGE_TYPE_AMBIENT_OCCLUSION,
    IMAGE_TYPE_NORMAL_MAP,
    IMAGE_TYPE_ROUGHNESS,
    IMAGE_TYPE_HEIGHT_MAP,
    IMAGE_TYPE_METALLIC
};

//component counts for every image type
constexpr uint32_t image_type_sizes[]{4, 1, 4, 1, 1, 1};
constexpr uint32_t getComponentCount(ImageType type){
    return image_type_sizes[type];
}
//holds type of image and filename
struct SetImageInfo{
    ImageType type;
    string filename;
};

/**
 * ImageSetOptions
 *  - Used to set information about images to be loaded inside a vector, so they can be loaded later.
 */
class ImageSetOptions : public vector<SetImageInfo>{
    //name of directory, in which images are
    string m_base_name;
    //state in which images should be after being loaded
    ImageState m_state;
public:
    /**
     * Initialize image set options.
     * @param filename_base the directory to find images in
     * @param state state, in which images should be after being loaded
     */
    ImageSetOptions(const string& filename_base, ImageState state);
    ImageSetOptions& addDiffuse(const string& filename = "BaseColor.png");
    ImageSetOptions& addAO(const string& filename = "AmbientOcclusion.png");
    ImageSetOptions& addNormalMap(const string& filename = "Normal.png");
    ImageSetOptions& addRoughness(const string& filename = "Roughness.png");
    ImageSetOptions& addHeightMap(const string& filename = "Height.png");
    ImageSetOptions& addMetallic(const string& filename = "Metallic.png");
    ImageState getState() const;
private:
    ImageSetOptions& add(ImageType type, const string& filename);
};


/**
 * Holds an image and a view for it's whole area.
 */
struct ImageTexture{
    Image image;
    ImageView image_view;
};


class LocalObjectCreator;

/**
 * ImageSet
 *  - Load all images from ImageSetOptions given to constructor, then copy them to the GPU
 */
class ImageSet : public vector<ImageTexture>{
    ImageMemoryObject m_memory;
public:
    //load images from given options to the GPU
    ImageSet(LocalObjectCreator& object_creator, const ImageSetOptions& options);\
    //return image reference
    Image& getImage(uint32_t i);
    //return reference to image view, and create it, if it doesn't already exist
    ImageView& getImageView(uint32_t i);  
private:
    //create images on the GPU from given options
    void createImages(LocalObjectCreator& object_creator, const ImageSetOptions& options);
    //return a vector of image pointers
    vector<const Image*> vectorOfImages();
};


#endif