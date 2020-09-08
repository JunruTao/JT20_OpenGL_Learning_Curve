#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "ObjectsCore.h"
#include "GL_Create_Geometries.h"
#include "GL_Shader_Functions.h"


//#############################################/
/*.............ANOUNCEMENTS start..............*/
/*
[6/272020]J.TAO openGL practicing
	1. create a gl context using glfw
	2. initialisation of glfw/glew

[7/1/2020]J.TAO Update - previous GLFW template
	1. create triangles,
		VAO, VBO, Shader(Vertex, Fragment)
	2. create shader programs
	3. compile shaders
	4. running
*/
/*.............ANOUNCEMENTS end................*/
//#############################################/


/*(1)-------------------[Global Variables]--------------------*/
/*Window Dimensions*/
const GLint Width = 800;
const GLint Height = 600;

GLuint VAO, VBO, shader;


// Vertex Shader code
static const char* vShader = "                                                \n\
#version 330                                                                  \n\
                                                                              \n\
layout (location = 0) in vec3 pos;											  \n\
                                                                              \n\
void main()                                                                   \n\
{                                                                             \n\
    gl_Position = vec4(0.4*pos.x, 0.4*pos.y, pos.z, 1.0);				  \n\
}";

// Fragment Shader
static const char* fShader = "                                                \n\
#version 330                                                                  \n\
                                                                              \n\
out vec4 colour;                                                               \n\
                                                                              \n\
void main()                                                                   \n\
{                                                                             \n\
    colour = vec4(1.0, 0.0, 0.0, 1.0);                                         \n\
}";


int main()
{
	/*(2)-------------------[Setting Glfw Context]--------------------*/

	//[Initialise GLFW]:
	if (!glfwInit())
	{
		std::cout << "[Failed!]@__##[GLFW]-Framework Initialisation Error" << std::endl;
		glfwTerminate();
		return 1;
	}

	//[Set-up Version Properties]: version 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//[Version Compatiblity]: backward forbidden [X]
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//[Version Compatiblity]: forward allowed [V]
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);


	/*(2)-------------------[Create Glfw Window]--------------------*/

	//[Create main Window]:
	GLFWwindow* mainWindow = glfwCreateWindow(Width, Height, "GL Window Test", NULL, NULL);
	//[Window Creatation Test]:
	if (!mainWindow)
	{
		std::cout << "[Failed!]@__##[GLFW] - Create Glfw-Window Error" << std::endl;
		glfwTerminate();
		return 0;
	}
	//[Get Buffer Size]:
	int bufferWidth; int bufferHeight;
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);
	//[Set current context to main window]:
	glfwMakeContextCurrent(mainWindow);


	/*(3)-------------------[Setting Up GLEW]--------------------*/

	//[Allowing Glew using Modern features]:
	glewExperimental = GL_TRUE;
	//[Initialise GLEW]:
	if (glewInit() != GLEW_OK)
	{
		std::cout << "[Failed!]@__##[OpenGL Extension Wrangle -GLEW] - Initialisation Error" << std::endl;
		glfwDestroyWindow(mainWindow); //Due to a ready window creation
		glfwTerminate();
		return 1;
	}

	/*(4)-------------------[Create OpenGL viewport]--------------------*/

	glViewport(0, 0, bufferWidth, bufferHeight);

	/*-[creating objects here]-*/
	ObjectGeo geo;
	geo.AddVertices(-1.0f, -1.0f, 0.0f);
	geo.AddVertices(1.0f, -1.0f, 0.0f);
	geo.AddVertices(0.0f, 1.0f, 0.0f);
	geo.AddVertices(-1.0f,0.4f, 0.0f); //Extra Points Added
	CreateGLGeometry(geo, &VAO, &VBO);

	/*-[Compile Shader here]-*/
	CompileShaders(shader,vShader,fShader);
	



	/*(5)-------------------[Running Display While-Loop]--------------------*/
	while (!glfwWindowShouldClose(mainWindow))
	{
		//[Get User Events -i.g. mouse, keyboard...]:
		glfwPollEvents();


		//[GL: Clear Window] color & buffer:
		glClearColor(0.1f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLE_FAN, 0, geo.GetVerticesCount());
		//glDrawArrays(GL_TRIANGLES, 0, geo.GetVerticesCount()); - Old
		
		glBindVertexArray(0);
		glUseProgram(0);


		//[GLFW - Swap Buffer to dispay]
		glfwSwapBuffers(mainWindow);
	}

	glfwTerminate();
	return 0;
}
