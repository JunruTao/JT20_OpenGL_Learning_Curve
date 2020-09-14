
#include <windows.h>
#include <stdio.h>
#include <GL/glew.h>
#include <SOIL/SOIL.h>

#include "glm/glm.hpp"
#include "glm/common.hpp"
#include "glm/ext.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "glm/matrix.hpp"

#include <vector>
#include <thread>
#include <chrono>
#include <cmath>

#include "ShaderProgram.h"
#include "Camera.h"
#include "Mesh.h"


const char* vertShader_PATH = 
"C:/Users/1/Cpp_Tutorials/OpenGL_Projects/JT20_OpenGL_Learning_Curve/JT20_Win32_GL_test_12_Camera/shaders/vShader_01.glsl";
const char* fragShader_PATH =
"C:/Users/1/Cpp_Tutorials/OpenGL_Projects/JT20_OpenGL_Learning_Curve/JT20_Win32_GL_test_12_Camera/shaders/fShader_01.glsl";
const char* geomShader_PATH =
"C:/Users/1/Cpp_Tutorials/OpenGL_Projects/JT20_OpenGL_Learning_Curve/JT20_Win32_GL_test_12_Camera/shaders/gShader_01.glsl";

static GLuint program1 = 0;
static GLuint program2 = 0;
static bool running = true;

HDC dvcContext; //where to store pixel data
HGLRC rdrContext; //handle to OpenGL context


HWND hWnd;
LPCWSTR JT_MAIN_WIN_NAME = L"JT20 OpenGL-Win32 x64";
LPCWSTR parentCapTitle = L"JT-2020 GLWin";
LPCWSTR Error01 = L"Error 01: RegisterClassW issue tend to ";
LPCWSTR Error02 = L"Error 02: ParentWindowCreateW issue to tend to";
static int wndWidth = 800;
static int wndHeight = 600;

static glm::vec3 campos = { -1.5f,0.5f,0.0f };
static glm::vec3 mView = { 0.0f,0.0f,0.0f };
static float speed = 0.05f;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

/*--------------[Windows Main function]--------------*/
/*[Part1.4]---WinMain Inputs*/
int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	PSTR szCmdLine,
	int iCmdShow)
{
	MSG msg = {};
	WNDCLASS winClass = {};
	winClass.lpfnWndProc = WndProc;
	winClass.hInstance = hInstance;
	winClass.lpszClassName = JT_MAIN_WIN_NAME;
	if (!RegisterClass(&winClass))
	{
		MessageBoxW(NULL, Error01, JT_MAIN_WIN_NAME, MB_ICONERROR);
	}
	hWnd = CreateWindowEx(
		0,                          //Optional Window style 
		JT_MAIN_WIN_NAME,           //Window Class name
		parentCapTitle,             //Window Caption text
		WS_OVERLAPPEDWINDOW,        //Window Style(WS_EX_OVER...) - will show no closing buttons
		CW_USEDEFAULT,              //Window Pos.Start X,
		CW_USEDEFAULT,              //Window Pos.Start Y,
		wndWidth,              //Window Width,
		wndHeight,              //Window Height
		NULL,             //Parent Window
		NULL,            //Menu
		hInstance,        //Instance handle
		NULL              //Additional application data
	);
	if (hWnd == NULL){ MessageBoxW(NULL, Error02, JT_MAIN_WIN_NAME, 0); return 0; }
	ShowWindow(hWnd, iCmdShow);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);


	GameCamera* cam1 = new GameCamera();
	Mesh* mesh1 = new Mesh();
	Mesh* mesh2 = new Mesh();

	mesh1->LoadFromObjFile("C:/Users/1/Desktop/test.obj");
	mesh2->LoadFromObjFile("C:/Users/1/Desktop/ground.obj");

	GLuint texture = SOIL_load_OGL_texture(
		"C:/Users/1/Cpp_Tutorials/OpenGL_Projects/JT20_OpenGL_Learning_Curve/JT20_Win32_GL_test_12_Camera/people.png", 
		SOIL_LOAD_AUTO, 
		SOIL_CREATE_NEW_ID, 
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);


	GLuint texture2 = SOIL_load_OGL_texture(
		"C:/Users/1/Cpp_Tutorials/OpenGL_Projects/JT20_OpenGL_Learning_Curve/JT20_Win32_GL_test_12_Camera/hello2.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);

	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);





    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	/*[PART 2.3]----THE 'BIG WHILE LOOP'----*/
	while (running)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// Translate the message and dispatch it to WindowProc()
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			// If the message is WM_QUIT, exit the while loop
			if (msg.message == WM_QUIT || msg.message == WM_CLOSE) {
				running = false;
			}

		}

		cam1->ProcessInput(&wndWidth, &wndHeight, true);

		//drawing the background
		glClearDepth(1.0f);
		glClearColor(1.0f, 0.2f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glUseProgram(program1);

			GLint uniform_model1 = glGetUniformLocation(program1, "model");
			GLint uniform_perspective = glGetUniformLocation(program1, "perspective");
			GLint attrib_colormap1 = glGetUniformLocation(program1, "colorMap0");
			glUniform1i(attrib_colormap1, 0);//set colormap value 0c
			glm::mat4 mat_pers = cam1->GetCameraMatrix(&wndWidth, &wndHeight);
			glUniformMatrix4fv(uniform_perspective, 1, GL_FALSE, glm::value_ptr(mat_pers));


			static float Scale1 = 0;
			static float Dir = 0.02f;
			Scale1 += Dir;

				mesh1->AttributeBinding(program1);

					glm::mat4 mat = glm::mat4(1.0f);
					mat = glm::rotate<float>(mat, Scale1,glm::vec3(0.0f,1.0f,0));
					glUniformMatrix4fv(uniform_model1, 1, GL_FALSE, glm::value_ptr(mat));

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texture);

				mesh1->Draw();

				
				glBindTexture(GL_TEXTURE_2D, 0);


					mesh2->AttributeBinding(program1);
					glm::mat4 mat2 = glm::mat4(1.0f);
					glUniformMatrix4fv(uniform_model1, 1, GL_FALSE, glm::value_ptr(mat2));

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texture2);

				mesh2->Draw();

			glUseProgram(0);


		SwapBuffers(dvcContext);
	}



	delete cam1;

	return 0;
}
/*------------------------------------------------- */






/*[WindowProc] - Window Procedure*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT userMessage, WPARAM wParam, LPARAM lParam)
{

	switch (userMessage)
	{
		/*--------------ON--CREATION----------------*/
	case WM_CREATE:
	{

		//_______________________GL_______________________
		PIXELFORMATDESCRIPTOR pfd = {
			sizeof(PIXELFORMATDESCRIPTOR),  //  size of this pfd  
				1,                     // version number  
				PFD_DRAW_TO_WINDOW |   // support window  
				PFD_SUPPORT_OPENGL |   // support OpenGL  
				PFD_DOUBLEBUFFER,      // double buffered  
				PFD_TYPE_RGBA,         // RGBA type  
				24,                    // 24-bit color depth  
				0, 0, 0, 0, 0, 0,      // color bits ignored  
				0,                     // no alpha buffer  
				0,                     // shift bit ignored  
				0,                     // no accumulation buffer  
				0, 0, 0, 0,            // accum bits ignored  
				32,                    // 32-bit z-buffer      
				0,                     // no stencil buffer  
				0,                     // no auxiliary buffer  
				PFD_MAIN_PLANE,        // main layer  
				0,                     // reserved  
				0, 0, 0                // layer masks ignored  
		};

		dvcContext = GetDC(hWnd);//get handle from HWND
		int pixelFormat = ChoosePixelFormat(dvcContext, &pfd);
		SetPixelFormat(dvcContext, pixelFormat, &pfd);
		rdrContext = wglCreateContext(dvcContext);
		wglMakeCurrent(dvcContext, rdrContext);

		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK)
		{
			DestroyWindow(hWnd);
		}

		program1 = CreateCustomProgram(vertShader_PATH, fragShader_PATH, geomShader_PATH);
		glViewport(0, 0, wndWidth, wndHeight);
		ShowCursor(FALSE);
		return 0;


	}break;

	case WM_KEYDOWN:
	{
		if (wParam == VK_ESCAPE) 
		{
			running = false;
		}
	}break;


	case WM_LBUTTONDOWN:
	{

	}
	break;

	case WM_DESTROY:
	{
		//_____________________________new for [GL]
		glDisable(GL_TEXTURE_2D);
		ReleaseDC(hWnd, dvcContext);
		wglDeleteContext(rdrContext);

		glDeleteProgram(program1);
		PostQuitMessage(0);
		return 0;

	}break;

	}

	return DefWindowProc(hWnd, userMessage, wParam, lParam);
}