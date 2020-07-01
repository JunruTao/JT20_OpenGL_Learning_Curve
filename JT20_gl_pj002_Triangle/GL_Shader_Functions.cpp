#include <GL/glew.h>
#include <string.h>
#include <stdio.h>
#include "GL_Shader_Functions.h"

void AddShader(GLint theProgram, const char* shaderCode, GLenum shaderType) 
{
	/*Create the shader*/
	GLint iShader = glCreateShader(shaderType);
	
	//Create a char array to store the shader code;
	const GLchar* theCode[1];
	theCode[0] = shaderCode;
	
	//Create a int array store the length of the shader code;
	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);

	//Attach the shader code to iShader handle -> Compile Shader
	glShaderSource(iShader, 1, theCode, codeLength);
	glCompileShader(iShader);

	//[Debugging Stage] - test if this shader is compiled correctly.
	GLint result = 0;
	glGetShaderiv(iShader, GL_COMPILE_STATUS, &result);
	if (!result) 
	{
		GLchar eLog[1024] = { 0 };
		glGetShaderInfoLog(iShader, sizeof(eLog), NULL, eLog);
		fprintf(stderr, "Error compiling the %d shader: '%s' \n", shaderType, eLog);
		return;
	}

	//Final Step: Attraching the shader to the program
	glAttachShader(theProgram, iShader);
}



void CompileShaders(GLuint &_shader, const char* vertShader, const char* fragShader)
{
	//Assign the shader, creating a program
	_shader = glCreateProgram();

	if (!_shader) 
	{
		printf("failed to create shader..");
		return;
	}

	//Adding shader to the _shader handle
	AddShader(_shader, vertShader, GL_VERTEX_SHADER);
	AddShader(_shader, fragShader, GL_FRAGMENT_SHADER);

	//[Debug]Linking Shades to the program
	GLint result;
	GLchar eLog[1024];

	glLinkProgram(_shader);
	glGetProgramiv(_shader, GL_LINK_STATUS, &result);
	if (!result) 
	{
		glGetProgramInfoLog(_shader, sizeof(eLog), NULL, eLog);
		printf("Error Linking Program: '%s'\n", eLog);
		return;
	}


	//[Debug]Linking Shades to the program
	glValidateProgram(_shader);
	glGetProgramiv(_shader, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(_shader, sizeof(eLog), NULL, eLog);
		printf("Error Validating Program: '%s'\n", eLog);
		return;
	}

}