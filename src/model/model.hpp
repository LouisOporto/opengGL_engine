#ifndef _MODEL_H_
#define _MODEL_H_

#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../shader/shader.hpp"
#include "../mesh/mesh.hpp"
#include "../image_loader/image_loader.hpp"
#include "../engine/logger.hpp"

class Model {
    public:
        Model(const char *path) {
            // Model begins as a Assimp Scene (This is where the file gets loaded into)
            // It contains the important root nodes and childern nodes
            // If the scene is NULL, not complete or missing a root node then and error has occured
            loadModel(path);
        }
        void draw(Shader &shader);
    private:
        std::vector<Mesh> m_meshes;
        std::vector<Texture> m_texturesLoaded;
        std::string directory;

        void loadModel(std::string path);
        void processNode(aiNode *node, const aiScene *scene);
        Mesh processMesh(aiMesh *mesh, const aiScene *scene);
        std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};

#endif // _MODEL_H_