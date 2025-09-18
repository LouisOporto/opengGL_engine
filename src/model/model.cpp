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
        printf("ERROR::ASSIMP::%s\n", import.GetErrorString());
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene) {
    for(int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        m_meshes.push_back(processMesh(mesh, scene));
    }

    for(int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    // ColorInformation colorInfo;
    std::vector<Texture> textures;
    printf("Processing mesh: %s\n", mesh->mName.C_Str());

    for (int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        // process vertex position, normals and texture coords
        glm::vec3 vector;
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.position = vector;

        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.normal = vector;

        if (mesh->mTextureCoords[0]) {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.texCoords = vec;
        } 
        else {
            vertex.texCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    // process indices
    for (int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // process materials
    if (mesh->mMaterialIndex >= 0) {
        // printf("Mesh size: %d\n", mesh->mMaterialIndex);
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SHININESS, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        
        // std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        // textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        
        // colorInfo = {textures, {div   vffuse.r, diffuse.g, diffuse.b}, {specular.r, specular.g, specular.b}, shininess.r};
    }
    
    // printf("Number of textures: %d\n", textures.size());
    
    return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName) {
    std::vector<Texture> textures;
    printf("Texture count for type: %s, Count: %d\n", typeName.c_str(), mat->GetTextureCount(type));
    for (int i = 0; i < mat->GetTextureCount(type); i++) {
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
            aiColor3D ambient, diffuse, specular;
            float shininess;
            printf("Loading image file: %s\n", str.C_Str());

            mat->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
            mat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
            mat->Get(AI_MATKEY_COLOR_SPECULAR, specular);
            mat->Get(AI_MATKEY_SHININESS, shininess);

            texture.id = ImageLoader::getInstance()->loadImage(str.C_Str(), directory);
            // printf("texture.id = %d, Typename: %s\n", texture.id, typeName.c_str());
            texture.type = typeName;
            texture.path = str.C_Str();
            texture.ambient = {ambient.r, ambient.g, ambient.b};
            texture.diffuse = {diffuse.r, diffuse.g, diffuse.b};
            texture.specular = {specular.r, diffuse.g, diffuse.b};
            texture.shininess = shininess;

            textures.push_back(texture);
            m_texturesLoaded.push_back(texture);
        }
    }
    return textures;
}