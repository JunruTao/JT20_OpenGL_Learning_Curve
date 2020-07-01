#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
//#############################################/
/*.............ANOUNCEMENTS start..............*/
/*
[6/272020]J.TAO openGL practicing
	1. create a gl context using glfw
	2. initialisation of glfw/glew

[]
*/
/*.............ANOUNCEMENTS end................*/
//#############################################/


/*(1)-------------------[Global Variables]--------------------*/
/*Window Dimensions*/
const GLint Width = 800;
const GLint Height = 600;


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
		std::cout<< "[Failed!]@__##[OpenGL Extension Wrangle -GLEW] - Initialisation Error" << std::endl;
		glfwDestroyWindow(mainWindow); //Due to a ready window creation
		glfwTerminate();
		return 1;
	}

	/*(4)-------------------[Create OpenGL viewport]--------------------*/

	glViewport(0,0,bufferWidth,bufferHeight);

	/*(5)-------------------[Running Display While-Loop]--------------------*/
	while (!glfwWindowShouldClose(mainWindow)) 
	{
		//[Get User Events -i.g. mouse, keyboard...]:
		glfwPollEvents();

		/*.............USER FUNCTIONS start.............*/




		/*.............USER FUNCTIONS end.............*/

		//[GL: Clear Window] color & buffer:
		glClearColor(0.1f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		//[GLFW - Swap Buffer to dispay]
		glfwSwapBuffers(mainWindow);
	}

	return 0;
}
