#ifndef _IMAGE_LOADER_H_
#define _IMAGE_LOADER_H_
#include <GL/glew.h>

#include <string>
#include <vector>

#include "../engine/logger.hpp"
#include "stb_image.h"

class ImageLoader {
   public:
    static ImageLoader* getInstance() {
        return m_instance = m_instance != nullptr ? m_instance : new ImageLoader();
    }

    unsigned int loadImage(const char* filepath, const std::string& directory, int diffuse = 0);
    unsigned int loadCubemap(std::vector<std::string> faces, const std::string& directory);

   private:
    ImageLoader() {}
    unsigned char* subImageGet(unsigned char* originalData, int originalWidth, int originalHeight, int channels, int xOffset, int yOffset, int newWidth, int newHeight);
    static ImageLoader* m_instance;
};

#endif  // _IMAGE_LOADER_H_