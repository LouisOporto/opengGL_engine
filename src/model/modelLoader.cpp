#include "modelLoader.hpp"

bool ModelLoader::addModel(const char* modelName, const char* filePath) {
    if (m_models.count(modelName)) {
        Logger::Error("File with name: %s already exists inside modelLoader", modelName);
        return false;
    }

    m_models[modelName] = new Model(filePath);
    return true;
}

Model* ModelLoader::operator[] (const char* modelName) {
    if (!m_models.count(modelName)) {
        Logger::Error("Failed to retrieve model with name: %s", modelName);
        return nullptr;
    }
    return m_models[modelName];
}

void ModelLoader::clean() {
    for (auto iter = m_models.begin(); iter != m_models.end(); iter++) {
        delete iter->second;
    }
    m_models.clear();
}