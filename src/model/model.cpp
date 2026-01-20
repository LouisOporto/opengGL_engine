#include "model.hpp"

void Model::draw(Shader &shader) {
    for (int i = 0; i < m_meshes.size(); i++) {
        m_meshes[i].draw(shader);
    }
}

void Model::loadModel(std::string path) {
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        Logger::Error("ERROR::ASSIMP::%s", import.GetErrorString());
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene) {
    for (int i = 0; i < node->mNumMeshes; i++) {
        // Logger::Warn("Processing Mesh: %d", i);
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        m_meshes.push_back(processMesh(mesh, scene));
    }

    // Logger::Warn("Process Node Childern");
    for (int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    DefaultMaterials materialColor;

    Vertex vertex;
    // Logger::Log("Processing mesh: %s", mesh->mName.C_Str());

    // process vertex
    for (int i = 0; i < mesh->mNumVertices; i++) {
        glm::vec3 vector;
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.position = vector;

        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.normal = vector;

        vector.x = mesh->mTangents[i].x;
        vector.y = mesh->mTangents[i].y;
        vector.z = mesh->mTangents[i].z;
        vertex.tangent = vector;

        if (mesh->mTextureCoords[0]) {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.texCoords = vec;
        } else {
            vertex.texCoords = glm::vec2(0.0f);
        }

        vertices.push_back(vertex);
    }

    // Logger::Log("Process Faces");
    // process indices
    for (int i = 0; i < mesh->mNumFaces; i++) {
        // Get faces from mesh->mFaces[index]
        aiFace face = mesh->mFaces[i];
        for (int j = 0; j < face.mNumIndices; j++) {
            // Get indices of each face from mesh->mIndices[index];
            indices.push_back(face.mIndices[j]);
        }
    }

    // Logger::Log("Process Materials");
    // process materials
    // NOTE: Focus on loading abedo (diffuse) and specular(metallic) (Add more
    // texture material once we can figure that out) ie. shininess, roughness,
    // normal map etc...
    if (mesh->mMaterialIndex >= 0) {
        // Get material from scene->mMaterials[index] using the meshs material
        // index; Logger::Log("Mesh size: %d", mesh->mMaterialIndex);
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        // Logger::Log("After material retrieval");
        bool noMaterials = true;

        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");  // From abledo
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");  // From Metallic
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_height");  // Should be the roughness map
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        aiColor3D ambient, diffuse, specular;
        float shininess;

        material->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
        material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
        material->Get(AI_MATKEY_COLOR_SPECULAR, specular);

        material->Get(AI_MATKEY_SHININESS, shininess);
        shininess = glm::max(glm::min(512.f, shininess), 0.f);

        materialColor.ambient = {ambient.r, ambient.g, ambient.b};
        materialColor.diffuse = {diffuse.r, diffuse.g, diffuse.b};
        materialColor.specular = {specular.r, specular.g, specular.b};
        materialColor.shininess = shininess;

        // Logger::Warn("Ka: %f, %f, %f\n"
        //     "Kd: %f, %f, %f\n"
        //     "Ks: %f, %f, %f\n"
        //     "Shininess: %f",
        //     ambient.r, ambient.g, ambient.b,
        //     diffuse.r, diffuse.g, diffuse.b,
        //     specular.r, specular.g, specular.b, shininess);
    }
    return Mesh(vertices, indices, textures, materialColor);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName) {
    std::vector<Texture> textures;
    int count = mat->GetTextureCount(type);

    // Logger::Log("Texture count for type: %s, Count: %d", typeName.c_str(),
    // count);

    for (int i = 0; i < count; i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        for (int j = 0; j < m_texturesLoaded.size(); j++) {
            if (std::strcmp(m_texturesLoaded[j].path.data(), str.C_Str()) == 0) {
                textures.push_back(m_texturesLoaded[j]);
                skip = true;
                break;
            }
        }
        if (!skip) {
            Texture texture;
            // Logger::Log("Loading image file: %s", str.C_Str());

            if (typeName == "texture_diffuse") {
                texture.id = ImageLoader::getInstance()->loadImage(str.C_Str(), directory, 1);
            }
            else {
                texture.id = ImageLoader::getInstance()->loadImage(str.C_Str(), directory);
            }
            // printf("texture.id = %d, Typename: %s\n", texture.id,
            // typeName.c_str());
            texture.type = typeName;
            texture.path = str.C_Str();

            textures.push_back(texture);
            m_texturesLoaded.push_back(texture);
        }
    }
    return textures;
}