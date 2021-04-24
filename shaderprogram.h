#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <GL/glew.h>
#include "stdio.h"

class ShaderProgram {
private:
	GLuint vertexShader;
	GLuint geometryShader; 
	GLuint fragmentShader;
	char* readFile(const char* fileName);
	GLuint loadShader(GLenum shaderType, const char* fileName);
public:
	GLuint shaderProgram;
	ShaderProgram(const char* vertexShaderFile, const char* geometryShaderFile, const char* fragmentShaderFile);
	~ShaderProgram();
	void use();
	GLuint u(const char* variableName);
	GLuint a(const char* variableName);
};




#endif
