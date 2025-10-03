#include "image_loader.hpp"

ImageLoader* ImageLoader::m_instance = nullptr;

unsigned int ImageLoader::loadImage(const char *path, const std::string &directory) {
    std::string filepath = std::string(path);
    filepath = directory + '/' + filepath;

    unsigned int texture;
    glGenTextures(1, &texture);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, (nrChannels == 4 ? GL_RGBA : GL_RGB), GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glTextureParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTextureParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }
    else {
        Logger::Error("Textures failed to load path: %s", filepath.c_str());
        stbi_image_free(data);
    }

    return texture;
}