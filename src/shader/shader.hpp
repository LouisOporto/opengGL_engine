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

        void setInt(const char* name, int value) const;
        void setBool(const char* name, bool value ) const;
        void setFloat(const char* name, float value) const;
        void setVec3(const char* name, glm::vec3 value) const;
        void setMat3(const char* name, glm::mat3 value) const;
        
    private:
        unsigned int ID;
};

#endif // _SHADER_H_