#include "mesh.hpp"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) {
    m_vertices = vertices;
    m_indices = indices;
    m_textures = textures;

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

    glBindVertexArray(0);
}

void Mesh::draw(Shader& shader) {
    // unsigned int ambientNr = 1;
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    // unsigned int heightNr = 1;
    bool missingDiffuse = true;
    bool missingSpecular = true;
    // bool missingNormal = true;

    for (int i = 0; i < m_textures.size(); i++) {
        // printf("Texture Number: %d\n", i);
        glActiveTexture(GL_TEXTURE0 + i);

        std::string number;
        std::string name = m_textures[i].type;
        // if (name == "texture_ambient") number = std::to_string(ambientNr++);
        if (name == "texture_diffuse") { number = std::to_string(diffuseNr++); missingDiffuse = false; }
        else if (name == "texture_specular") { number = std::to_string(specularNr++); missingSpecular = false; }
        else if (name == "texture_normal") number = std::to_string(normalNr++);
        
        // else if (name == "textuer_height") number = std::to_string(heightNr++);
        // printf("Shader Name: %s, Number: %d\n", ("material." + name + number).c_str(), i);
        if (name == "texture_normal") {
            shader.setInt((name + number).c_str(), i);
        } 
        else shader.setInt(("material." + name + number).c_str(), i);
        glBindTexture(GL_TEXTURE_2D, m_textures[i].id);

        shader.setVec3("material.ambient", m_textures[i].diffuse);
        shader.setVec3("material.diffuse", m_textures[i].diffuse);
        shader.setVec3("material.specular", m_textures[i].specular);
        shader.setFloat("material.shininess", m_textures[i].shininess);
    }

    if (!missingDiffuse) shader.setBool("material.missingDiffuse", true);
    else shader.setBool("material.missingDiffuse", false);

    if (!missingSpecular) shader.setBool("Material.missingSpecular", true);
    else shader.setBool("material.missingSpecular", false);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
}