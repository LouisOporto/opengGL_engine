#include "mesh.hpp"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, DefaultMaterials defaultMat) {
    m_vertices = vertices;
    m_indices = indices;
    m_textures = textures;
    m_meshMaterials = defaultMat;

    setupMesh();
}

void Mesh::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);
}

void Mesh::draw(Shader* shader) {
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;

    bool missingDiffuse = true;
    bool missingSpecular = true;
    bool missingHeight = true;
    bool missingNormal = true;

    for (int i = 0; i < m_textures.size(); i++) {
        // printf("Texture Number: %d\n", i);
        glActiveTexture(GL_TEXTURE0 + i);

        std::string number;
        std::string name = m_textures[i].type;
        if (name == "texture_diffuse") {
            number = std::to_string(diffuseNr++);
            missingDiffuse = false;
        } else if (name == "texture_specular") {
            number = std::to_string(specularNr++);
            missingSpecular = false;
        } else if (name == "texture_normal") {
            number = std::to_string(normalNr++);
            missingNormal = false;
        } else if (name == "texture_height")
            number = std::to_string(heightNr++);
        missingHeight = false;

        // Logger::Log("Shader Name: %s, Number: %d\n", ("material." + name +
        // number).c_str(), i);
        if (name == "texture_height" || name == "texture_normal") {
            shader->setInt(("materialVert." + name + number).c_str(), i);
            // shader.setInt(("material" + name + number).c_str(), i);
        } else {
            shader->setInt(("material." + name + number).c_str(), i);
        }
        // Logger::Log("Setting %s", (name + number).c_str());
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
    }

    shader->setBool("material.missingDiffuse", missingDiffuse);
    shader->setBool("material.missingSpecular", missingSpecular);
    // shader.setBool("material.missingNormal", missingNormal);
    shader->setBool("materialVert.missingNormal", missingNormal);
    shader->setBool("materialVert.missingHeight", missingHeight);

    shader->setVec3("material.ambient", m_meshMaterials.ambient);
    shader->setVec3("material.diffuse", m_meshMaterials.diffuse);
    shader->setVec3("material.specular", m_meshMaterials.specular);
    shader->setFloat("material.shininess", m_meshMaterials.shininess);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
}