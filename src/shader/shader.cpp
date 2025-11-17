#include "shader.hpp"

bool Shader::initShader(const char* vertexFile, const char* fragmentFile) {
    std::fstream vertFile;
    std::fstream fragFile;
    std::string vertexCode;
    std::string fragmentCode;

    vertFile.exceptions(std::fstream::badbit | std::fstream::failbit);
    fragFile.exceptions(std::fstream::badbit | std::fstream::failbit);

    try {
        vertFile.open(vertexFile);
        fragFile.open(fragmentFile);

        std::stringstream vertStream, fragStream;
        vertStream << vertFile.rdbuf();
        fragStream << fragFile.rdbuf();

        vertexCode = vertStream.str();
        fragmentCode = fragStream.str();

        vertFile.close();
        fragFile.close();
    } catch (std::fstream::failure) {
        Logger::Error("SHADER::ERROR::FILE_LOADING");
        return false;
    }

    const char* vCode = vertexCode.c_str();
    const char* fCode = fragmentCode.c_str();

    unsigned int vertex, fragment;
    int status;
    char log[512];

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vCode, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &status);
    if (!status) {
        glGetShaderInfoLog(vertex, 512, NULL, log);
        Logger::Error("SHADER::ERROR::VERTEX::COMPILE::%s", log);
        return false;
    }

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fCode, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &status);
    if (!status) {
        glGetShaderInfoLog(fragment, 512, NULL, log);
        Logger::Error("SHADER::ERROR::FRAGMENT::COMPILE::%s\n", log);
        return false;
    }

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    glGetProgramiv(ID, GL_LINK_STATUS, &status);
    if (!status) {
        glGetProgramInfoLog(ID, 512, NULL, log);
        Logger::Error("SHADER::ERROR::PROGRAM::LINKING::%s\n", log);
        return false;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return true;
}

void Shader::setInt(const std::string name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setBool(const std::string name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setFloat(const std::string name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec3(const std::string name, glm::vec3 value) const {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setMat4(const std::string name, glm::mat4 value) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                       &value[0][0]);
}

void Shader::setPointLight(const std::string name, glm::vec3 pos,
                           glm::vec3 ambient, glm::vec3 diffuse,
                           glm::vec3 specular, float constant, float linear,
                           float quadratic) {
    setVec3(name + ".position", pos);
    setVec3(name + ".ambient", ambient);
    setVec3(name + ".diffuse", diffuse);
    setVec3(name + ".specular", specular);
    setFloat(name + ".constant", constant);
    setFloat(name + ".linear", linear);
    setFloat(name + ".quadratic", quadratic);
}

void Shader::setSpotLight(const std::string name, glm::vec3 pos,
                          glm::vec3 direction, glm::vec3 ambient,
                          glm::vec3 diffuse, glm::vec3 specular, float cutOff,
                          float outerCutOff, float constant, float linear,
                          float quadratic) {
    setVec3(name + ".position", pos);
    setVec3(name + ".direction", direction);
    setVec3(name + ".ambient", ambient);
    setVec3(name + ".diffuse", diffuse);
    setVec3(name + ".specular", specular);
    setFloat(name + ".constant", constant);
    setFloat(name + ".linear", linear);
    setFloat(name + ".quadratic", quadratic);
    setFloat(name + ".cutOff", cutOff);
    setFloat(name + ".outerCutOff", outerCutOff);
}

void Shader::setDirLight(const std::string name, glm::vec3 direction,
                         glm::vec3 ambient, glm::vec3 diffuse,
                         glm::vec3 specular) {
    setVec3(name + ".direction", direction);
    setVec3(name + ".ambient", ambient);
    setVec3(name + ".diffuse", diffuse);
    setVec3(name + ".specular", specular);
}

void Shader::use() { glUseProgram(ID); }