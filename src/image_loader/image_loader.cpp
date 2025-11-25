#include "image_loader.hpp"

ImageLoader* ImageLoader::m_instance = nullptr;

unsigned int ImageLoader::loadImage(const char* path,
                                    const std::string& directory) {
    std::string filepath = std::string(path);
    filepath = directory + '/' + filepath;

    unsigned int texture;
    glGenTextures(1, &texture);

    int width, height, nrChannels;
    unsigned char* data =
        stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, (nrChannels == 4 ? GL_RGBA : GL_RGB),
                     width, height, 0, (nrChannels == 4 ? GL_RGBA : GL_RGB),
                     GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                            GL_LINEAR_MIPMAP_LINEAR);
        glTextureParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    } else {
        Logger::Error("Textures failed to load path: %s", filepath.c_str());
        stbi_image_free(data);
    }

    return texture;
}

unsigned int ImageLoader::loadCubemap(std::vector<std::string> faces,
                                      const std::string& directory) {
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    int width, height, nrChannels;
    if (faces.size() == 1) {
        // Figure out how to process all sides of the image similar to the previous process. // Predefined width and height to dissect a cubemap (6 sides)
        std::string path = directory + "/" + faces[0];
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0); // Load entire image once
        if (data) {
            int offsetW, offsetH;
            int newWidth = width / 4;
            int newHeight = height / 3;
            Logger::Log("Width: %d and Height: %d (SubWidth %d : SubHeight %d)", width, height, newWidth, newHeight);
            for (int i = 0; i < 6; i++) {
                switch (i) {
                    case 0: offsetW = newWidth * 2; offsetH = newHeight; break; // right
                    case 1: offsetW = 0; offsetH = newHeight; break; // left
                    case 2: offsetW = newWidth; offsetH = 0; break; // top
                    case 3: offsetW = newWidth; offsetH = newHeight * 2; break; // bottom
                    case 4: offsetW = newWidth; offsetH = newHeight; break; // front
                    case 5: offsetW = newWidth * 3; offsetH = newHeight; break; // bottom
                }
                Logger::Log("Offset for position: X=%d Y=%d", offsetW, offsetH);
                Logger::Log("Format%d", nrChannels);
                unsigned char* sideData = subImageGet(data, width, height, nrChannels, offsetW, offsetH, newWidth, newHeight);
                if (sideData != NULL) {
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, (nrChannels == 4 ? GL_RGBA : GL_RGB), newWidth, 
                        newHeight, 0, (nrChannels == 4 ? GL_RGBA : GL_RGB), GL_UNSIGNED_BYTE, sideData);
                }
                else {
                    Logger::Error("Failed to retrieve sub image");
                }
                // Logger::Warn("Cleaning sideData");
                free(sideData);
                // Logger::Warn("Error Number:%d", glGetError());
            }
            stbi_image_free(data);
        }
        else {
            Logger::Error("Texture failed to load path: %s", path.c_str());
            stbi_image_free(data);
        }
    }
    else {
        for (int i = 0; i < faces.size(); i++) {
            std::string path = directory + "/" + faces[i];
            unsigned char* data =
            stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
            if (data) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, (nrChannels == 4 ? GL_RGBA : GL_RGB), width,
                    height, 0, (nrChannels == 4 ? GL_RGBA : GL_RGB), GL_UNSIGNED_BYTE, data);

                    stbi_image_free(data);
                } else {
                    Logger::Error("Textures failed to load path: %s", path.c_str());
                    stbi_image_free(data);
            }
        }
    }
            
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    return texture;
}

unsigned char* ImageLoader::subImageGet(unsigned char* originalData, int originalWidth, int originalHeight, int channels, int xOffset, int yOffset, int newWidth, int newHeight) {
    if (xOffset + newWidth > originalWidth || yOffset + newHeight > originalHeight || xOffset < 0 || yOffset < 0 || newWidth <= 0 || newHeight <= 0) {
        return NULL; // Error handling for invalid crop dimensions
    }

    // Calculate the size of new image data
    size_t newDataSize = newWidth * newHeight * channels;
    // Logger::Warn("Allocating newData");
    unsigned char* newData = (unsigned char*)malloc(newDataSize);
    if (newData == NULL) return NULL;

    for (int y = 0; y < newHeight; y++) {
        int originalRowStartIndex = ((yOffset + y) * originalWidth + xOffset) * channels;
        int newRowStartIndex = (y * newWidth) * channels;
        size_t bytesToCopy = newWidth * channels;

        // Logger::Warn("Memcopying line: %d", y);
        memcpy(newData + newRowStartIndex, originalData + originalRowStartIndex, bytesToCopy);
    }

    return newData;
}