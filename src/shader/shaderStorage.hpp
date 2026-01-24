#pragma once

#include "shader.hpp"
#include <map>
#include <string>

class ShaderStorage {
    public:
        static ShaderStorage* getInstance() { return m_instance = m_instance != nullptr ? m_instance : new ShaderStorage(); }
        bool addShader(const char* name, const char* vert, const char* frag, const char* geom = nullptr);
        // void removeShader(const char* name);
        Shader* getShader(const char* name);
        void clean();

    private:
        ShaderStorage() {}
        static ShaderStorage* m_instance;
        std::map<std::string, Shader*> m_shaders;
};