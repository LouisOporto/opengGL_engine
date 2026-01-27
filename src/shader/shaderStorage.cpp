#include "shaderStorage.hpp"

bool ShaderStorage::addShader(const char* name, const char* vert, const char* frag, const char* geom) {
    if (m_shaders.count(name)) return false;

    Shader* shader = new Shader();
    if (!shader->initShader(vert, frag, geom)) {
        Logger::Error("Failed to create shader");
        return false;
    }
    m_shaders[name] = shader;

    return true;
}

Shader* ShaderStorage::getShader(const char* name) {
    // Retrieve shader for public use
    if (m_shaders.count(name)) {
        return m_shaders[name];
    }
    // Better method to catch none errors
    Logger::Error("Could not find shader name: %s", name);
    return nullptr;
}

void ShaderStorage::clean() {
    for (auto& v : m_shaders) {
        delete v.second;
    }
    m_shaders.clear();
}