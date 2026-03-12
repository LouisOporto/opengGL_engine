#ifndef MODEL_LOADER_H_
#define MODEL_LOADER_H_
#include <map>

#include "model.hpp"
#include "../engine/logger.hpp"

class ModelLoader {
 public:
    ModelLoader() {}
    bool addModel(const char* modelName, const char* filePath);
    Model* operator[] (const char* modelName);

    void clean();

 private:
    std::map<std::string, Model*> m_models;
};

#endif // MODEL_LOADER_H_