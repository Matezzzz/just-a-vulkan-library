#include "image_load.h"
#include "../04_memory_objects/image_info.h"
#include "../04_memory_objects/local_object_creator.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
 
ImageData::ImageData(const string& filename, uint32_t comp_c){
    //flip images vertically on load - images were inverted otherwise
    stbi_set_flip_vertically_on_load(true);
    //load image with given filename using stb_image.h, save width, height and component count
    uint8_t* im_data = stbi_load(filename.c_str(), &width, &height, &comp, comp_c);
    if (im_data == nullptr){
        PRINT_ERROR("Image " << filename << " could not be loaded.")
        return;
    }
    //store image data as a vector to prevent bothering myself with having to delete pointer responsibly
    data = vector<uint8_t>(im_data, im_data+width*height*comp);
    //free image data
    stbi_image_free(im_data);
}
uint32_t ImageData::size(){
    return width*height*comp;
}



ImageSetOptions::ImageSetOptions(const string& filename_base, ImageState state) :
    m_base_name(filename_base), m_state(state)
{}
ImageSetOptions& ImageSetOptions::addDiffuse(const string& filename){
    return add(IMAGE_TYPE_DIFFUSE, filename);
}
ImageSetOptions& ImageSetOptions::addAO(const string& filename){
    return add(IMAGE_TYPE_AMBIENT_OCCLUSION, filename);
}
ImageSetOptions& ImageSetOptions::addNormalMap(const string& filename){
    return add(IMAGE_TYPE_NORMAL_MAP, filename);
}
ImageSetOptions& ImageSetOptions::addRoughness(const string& filename){
    return add(IMAGE_TYPE_ROUGHNESS, filename);
}
ImageSetOptions& ImageSetOptions::addHeightMap(const string& filename){
    return add(IMAGE_TYPE_HEIGHT_MAP, filename);
}
ImageSetOptions& ImageSetOptions::addMetallic(const string& filename){
    return add(IMAGE_TYPE_METALLIC, filename);
}
ImageState ImageSetOptions::getState() const{
    return m_state;
}
ImageSetOptions& ImageSetOptions::add(ImageType type, const string& filename){
    //add given image info to the list. Filename is derived from (base name = directory + prefix)_(image name)
    push_back(SetImageInfo{type, m_base_name + "/" + m_base_name + "_" + filename});
    return *this;
}




ImageSet::ImageSet(LocalObjectCreator& object_creator, const ImageSetOptions& options){
    reserve(options.size());
    createImages(object_creator, options);
}
const VkFormat image_formats[]{VK_FORMAT_UNDEFINED, VK_FORMAT_R8_UNORM, VK_FORMAT_R8G8_UNORM, VK_FORMAT_R8G8B8_UNORM, VK_FORMAT_R8G8B8A8_UNORM};
void ImageSet::createImages(LocalObjectCreator& object_creator, const ImageSetOptions& options){
    //how many images to load
    uint32_t image_count = options.size();
    //reserve a vector for them
    vector<ImageData> image_data;
    image_data.reserve(image_count);

    //load image from each info
    for (const SetImageInfo& info : options){
        image_data.push_back(ImageData{info.filename, getComponentCount(info.type)});
    }
    
    //create vulkan image for each loaded image data
    for (uint32_t i = 0; i < image_count; i++){
        push_back(ImageInfo(image_data[i].width, image_data[i].height, image_formats[image_data[i].comp], VK_IMAGE_USAGE_SAMPLED_BIT).create());
    }
    //allocate memory for all images
    m_memory = ImageMemoryObject(vectorOfImages(), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    //copy loaded image data to each image
    for (uint32_t i = 0; i < image_count; i++){
        object_creator.copyToLocal(image_data[i].data.data(), image_data[i].size(), (*this)[i], ImageState{IMAGE_NEWLY_CREATED}, options.getState());
    }
}
vector<const Image*> ImageSet::vectorOfImages(){
    //allocate vector of pointers
    vector<const Image*> v(size());
    //go through all images and set a pointer for each one
    for (uint32_t i = 0; i < size(); i++){
        v[i] = &(*this)[i];
    }
    return v;
}