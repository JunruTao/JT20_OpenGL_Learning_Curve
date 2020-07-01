#ifndef GL_SHADER_FUNCTIONS_H
#define GL_SHADER_FUNCTIONS_H

//All Shader related functions defined in this Header
void AddShader(GLint theProgram,const char* ShaderCode, GLenum shaderType);
void CompileShaders(GLuint &_shader, const char* vertShader, const char* fragShader);

#endif
