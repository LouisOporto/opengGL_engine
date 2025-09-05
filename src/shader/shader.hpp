#ifndef _SHADER_H_
#define _SHADER_H_

#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <fstream>
#include <sstream>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtc/type_ptr.hpp>

class Shader {
    public:
        Shader() {}

        bool initShader(const char* vertexFile, const char* fragmentFIle);
        
        void use();

        void setInt(const std::string name, int value) const;
        void setBool(const std::string name, bool value ) const;
        void setFloat(const std::string name, float value) const;
        void setVec3(const std::string name, glm::vec3 value) const;
        void setMat4(const std::string name, glm::mat4 value) const;
       
        void setSpotLight(const std::string name, glm::vec3 pos, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float cutOff, float outerCutOff, float constant, float linear, float quadratic);
        void setPointLight(const std::string name, glm::vec3 pos, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant, float linear, float quadratic);
        void setDirLight(const std::string name, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
    private:
        unsigned int ID;
};

#endif // _SHADER_H_