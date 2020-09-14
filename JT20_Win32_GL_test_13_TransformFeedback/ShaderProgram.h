#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H


#include <fstream>
#include <iostream>
#include <sstream>
#include <exception>
#include <string>
#include <GL/glew.h>


std::string ReadShader(const char* filePath)
{
	std::string content;
	std::ifstream fileStream(filePath, std::ios::in);

	if (!fileStream.is_open()) {
		MessageBoxA(NULL, "Could not read file ", "file reading error", 0);
		return "";
	}

	std::string line = "";
	while (!fileStream.eof()) {
		std::getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();
	return content;
}





GLuint ShaderCompile(const GLenum shader_type, const char* filename)
{
	/* create shader object, set the source, and compile */
	std::string scode = ReadShader(filename);
	const char* source = scode.c_str();

	GLuint shader = glCreateShader(shader_type);
	GLint length = strlen(source);
	glShaderSource(shader, 1, (const char**)&source, NULL);
	glCompileShader(shader);

	GLint result = 0;
	/* make sure the compilation was successful */
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		char* log;

		/* get the shader info log */
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		log = (char*)malloc(length);
		glGetShaderInfoLog(shader, length, &result, log);

		/* print an error message and the info log */
		//printf("shaderCompileFromFile(): Unable to compile %s\n", log);
		MessageBoxA(NULL, log, "shader error", 0);
		free(log);

		glDeleteShader(shader);
		return 0;
	}
	return shader;
}


/*
 * Compiles and attaches a shader of the
 * given type to the given program object.
 */
void shaderAttach(GLuint program, GLenum type, const char* shaderSource = NULL)
{
	/* compile the shader */
	GLuint shader = ShaderCompile(type, shaderSource);
	if (shader != 0) {
		/* attach the shader to the program */
		glAttachShader(program, shader);

		/* delete the shader - it won't actually be
		 * destroyed until the program that it's attached
		 * to has been destroyed */
		glDeleteShader(shader);
	}
}


GLuint CreateCustomProgram(const char* VertexPath, const char* FragmentPath, const char* GeoPath)
{

	GLuint nprogram = glCreateProgram();
	shaderAttach(nprogram, GL_VERTEX_SHADER, VertexPath);
	shaderAttach(nprogram, GL_FRAGMENT_SHADER, FragmentPath);
	shaderAttach(nprogram, GL_GEOMETRY_SHADER, GeoPath);

	const GLchar* feedbackVaryings[] = { "X","Y","Z","TX","TY" };
	glTransformFeedbackVaryings(nprogram, 5, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);

	glLinkProgram(nprogram);

	GLint result = 0;

	glGetProgramiv(nprogram, GL_LINK_STATUS, &result);
	if (result == GL_FALSE) {
		GLint length;
		char* log;
		/* get the program info log */
		glGetProgramiv(nprogram, GL_INFO_LOG_LENGTH, &length);
		log = (char*)malloc(length);
		glGetProgramInfoLog(nprogram, length, &result, log);
		/* print an error message and the info log */
		MessageBoxA(NULL, log, "sceneInit(): Program linking failed:", 0);
		free(log);
		/* delete the program */
		glDeleteProgram(nprogram);
		MessageBoxA(NULL, "failed", "Linking shader program", 0);
	}
	return nprogram;

}

#endif
