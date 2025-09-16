#ifndef _IMAGE_LOADER_H_
#define _IMAGE_LOADER_H_
#include "stb_image.h"
#include <GL/glew.h>
#include <string>

class ImageLoader {
    public:
        static ImageLoader* getInstance() { return m_instance = m_instance != nullptr ? m_instance : new ImageLoader(); }
        
        unsigned int loadImage(const char* filepath, const std::string &directory);
    private:
        ImageLoader() {}
        static ImageLoader* m_instance;
};  

#endif // _IMAGE_LOADER_H_