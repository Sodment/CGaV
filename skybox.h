#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>
#include "model.h"

const char* faces[6]
{
    "res/skybox/right.png",
    "res/skybox/left.png",
    "res/skybox/top.png",
    "res/skybox/bottom.png",
    "res/skybox/front.png",
    "res/skybox/back.png"
};

float skyboxVertices[] = {        
  -100.0f,  100.0f, -100.0f,
  -100.0f, -100.0f, -100.0f,
   100.0f, -100.0f, -100.0f,
   100.0f, -100.0f, -100.0f,
   100.0f,  100.0f, -100.0f,
  -100.0f,  100.0f, -100.0f,

  -100.0f, -100.0f,  100.0f,
  -100.0f, -100.0f, -100.0f,
  -100.0f,  100.0f, -100.0f,
  -100.0f,  100.0f, -100.0f,
  -100.0f,  100.0f,  100.0f,
  -100.0f, -100.0f,  100.0f,

   100.0f, -100.0f, -100.0f,
   100.0f, -100.0f,  100.0f,
   100.0f,  100.0f,  100.0f,
   100.0f,  100.0f,  100.0f,
   100.0f,  100.0f, -100.0f,
   100.0f, -100.0f, -100.0f,

  -100.0f, -100.0f,  100.0f,
  -100.0f,  100.0f,  100.0f,
   100.0f,  100.0f,  100.0f,
   100.0f,  100.0f,  100.0f,
   100.0f, -100.0f,  100.0f,
  -100.0f, -100.0f,  100.0f,

  -100.0f,  100.0f, -100.0f,
   100.0f,  100.0f, -100.0f,
   100.0f,  100.0f,  100.0f,
   100.0f,  100.0f,  100.0f,
  -100.0f,  100.0f,  100.0f,
  -100.0f,  100.0f, -100.0f,

  -100.0f, -100.0f, -100.0f,
  -100.0f, -100.0f,  100.0f,
   100.0f, -100.0f, -100.0f,
   100.0f, -100.0f, -100.0f,
  -100.0f, -100.0f,  100.0f,
   100.0f, -100.0f,  100.0f
};

class SkyBox 
{
public:
    GLuint VAO;

    SkyBox() 
    {
        setupSkybox();
    }

    void Draw(ShaderProgram& shader) 
    {
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(shader.shaderProgram, "skybox"), 0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0);
    }

private:
    GLuint VBO, skyboxTexture;

    unsigned int loadCubemap(const char* faces[6])
    {
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
        int width, height, nrComponents;
        for (unsigned int i = 0; i < 6; i++)
        {
            unsigned char* data = stbi_load(faces[i], &width, &height, &nrComponents, 0);
            if (data)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                std::cout << "Cubemap texture good to load at path: " << faces[i] << std::endl;
                stbi_image_free(data);
            }
            else
            {
                std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
                stbi_image_free(data);
            }
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return textureID;
    }

    void setupSkybox()
    {
        skyboxTexture = loadCubemap(faces);
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        glBindVertexArray(0);
    }
};
