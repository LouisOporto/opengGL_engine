#pragma once

#include "shader.hpp"
#include <map>
#include <string>

class ShaderStorage {
    public:
        ShaderStorage() {}
        bool addShader(const char* name, const char* vert, const char* frag, const char* geom = nullptr);
        Shader* getShader(const char* name);
        void clean();

    private:
        std::map<std::string, Shader*> m_shaders;
};