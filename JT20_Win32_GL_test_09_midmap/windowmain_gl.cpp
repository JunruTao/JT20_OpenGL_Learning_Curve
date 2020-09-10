
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

#include <thread>
#include <chrono>
#include <cmath>




#define GLSL(src) "#version 440\n" #src

const char* vertexShaderSrc = GLSL(
	in vec3 pos;
    in vec2 tex;
	out vec2 texOut;
	uniform float Scale;
	uniform mat4 MVP;

	void main()
	{
		gl_Position = vec4(pos.xyz, 1.0)*MVP;
		texOut = tex;
	}
);

//light example(hard coded)
const char* fragmentShaderSrc2 = GLSL(
	uniform float Scale;
	uniform sampler2D colorMap0;

	in vec2 texOut;
	in vec3 normal;
	in vec3 pos;

	layout(location = 0) out vec4 FragColor0;

void main() {
	vec3 eye = vec3(0, 0, 1);
	vec3 viewDir = normalize(eye - pos);

	vec3 lightpos = vec3(1, 1, 1);//som oblique rays
	vec3 lightDir = normalize(pos - lightpos);
	vec3 reflectDir = reflect(-lightDir, normal);  //notice that we find the reflection along plane (plane normal is provided)

	float intensity = pow(max(dot(viewDir, reflectDir), 0.0), 5);

	gl_FragData[0] = texture(colorMap0, texOut) + vec4(intensity, intensity, intensity, 0);
});




// Fragment shader 1
const char* fragmentShaderSrc_N1 = GLSL(
	uniform float Scale;
	in vec2 texOut;
	uniform sampler2D colorMap0;

	layout(location = 0) out vec4 FragColor0;
	layout(location = 1) out vec4 FragColor1;
	
	void main()
	{
		vec4 tempColor0 = texture(colorMap0, texOut.yx);
		FragColor0 = tempColor0;

		if (tempColor0.r > 0.2 && tempColor0.g > 0.2 && tempColor0.b > 0.2) 
		{
			FragColor1.xyz = tempColor0.xyz;
		}
		else 
		{
			FragColor1 = vec4(0.0f,0.0f,0.0f,0.0f);
		}

	}
);



// Fragment shader 2
const char* fragmentShaderSrc_N2 = GLSL(
	uniform float Scale;
	uniform float Xsize;
	uniform float Ysize;
	in vec2 texOut;

	uniform sampler2D colorMap1;
	uniform sampler2D colorMap2;
	uniform sampler2D depthMap1;

	out vec4 FragColor;

void main()
{
	vec4 nomalcolor = texture(colorMap1,texOut.yx);
	FragColor = texture(colorMap2, texOut.yx, 5);
}
);


static GLuint program1 = 0;
static GLuint program2 = 0;

HDC dvcContext; //where to store pixel data
HGLRC rdrContext; //handle to OpenGL context

/*[Part1.1][Create a Handle to window]*/
HWND hWnd; //  <---- This is the *Handle* of the {_parent_window_}
LPCWSTR JT_MAIN_WIN_NAME = L"JT20 Test Window"; //  <---- Used in the *Creation* of {_parent_window_}
LPCWSTR parentCapTitle = L"JT-2020 First Window"; //  <---- Used in the *CAPTION*(Text on the window) of {_parent_window_}
LPCWSTR Error01 = L"Error 01: RegisterClassW issue tend to ";
LPCWSTR Error02 = L"Error 02: ParentWindowCreateW issue to tend to"; //  <---- *Error handling message* of {_parent_window_}


GLuint ShaderCompile(const GLenum shader_type, const char* source)
{
	/* create shader object, set the source, and compile */
	GLuint shader = glCreateShader(shader_type);
	GLint length = strlen(source);
	glShaderSource(shader, 1, (const char**)&source, &length);
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
		MessageBoxA(hWnd, log, "shader error", 0);
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


GLuint CreateCustomProgram(const char* VertexSource, const char* FragmentSource)
{

	GLuint nprogram = glCreateProgram();
	shaderAttach(nprogram, GL_VERTEX_SHADER, VertexSource);
	shaderAttach(nprogram, GL_FRAGMENT_SHADER, FragmentSource);
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
		MessageBoxA(hWnd, log, "sceneInit(): Program linking failed:", 0);
		free(log);
		/* delete the program */
		glDeleteProgram(nprogram);
		MessageBoxA(hWnd, "failed", "Linking shader program", 0);
	}
	return nprogram;

}


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


/*--------------[Windows Main function]--------------*/
/*[Part1.4]---WinMain Inputs*/
int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	PSTR szCmdLine,
	int iCmdShow)
{

	/*[PART 2.1]----CREATE WINDOW CLASS STRUCTURE----*/
	MSG msg = {}; // Messsage

	/*NEW*/WNDCLASS winClass = {};
	/*NEW*/winClass.lpfnWndProc = WndProc;
	/*NEW*/winClass.hInstance = hInstance;
	/*NEW*/winClass.lpszClassName = JT_MAIN_WIN_NAME;


	/*[Register Class window]*/
	if (!RegisterClass(&winClass))
	{
		MessageBoxW(NULL, Error01, JT_MAIN_WIN_NAME, MB_ICONERROR);
	}



	/*[PART 2.2]----CREATING NEW WINDOW----*/
	/*NEW - Updates According to Win32 website -- Using CreateWindowEx*/
	hWnd = CreateWindowEx(
		0,                          //Optional Window style 
		JT_MAIN_WIN_NAME,           //Window Class name
		parentCapTitle,             //Window Caption text
		WS_OVERLAPPEDWINDOW,        //Window Style(WS_EX_OVER...) - will show no closing buttons
		CW_USEDEFAULT,              //Window Pos.Start X,
		CW_USEDEFAULT,              //Window Pos.Start Y,
		800,              //Window Width,
		600,              //Window Height
		NULL,             //Parent Window
		NULL,            //Menu
		hInstance,        //Instance handle
		NULL              //Additional application data
	);

	if (hWnd == NULL)
	{
		MessageBoxW(NULL, Error02, JT_MAIN_WIN_NAME, 0); //The [0] here uType Default for Okay push button
		return 0;
	}

	ShowWindow(hWnd, iCmdShow);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);


	struct MyVertex
	{
		float x, y, z, u, v;        //Vertex
	};

	static float bsize = 0.5f;

	MyVertex pvertex[24] =
	{
		//front
		{-bsize,bsize,bsize,0.0f,0.0f},{bsize,bsize,bsize,0.0f,1.0f},
		{bsize,-bsize,bsize,1.0f,1.0f},{-bsize,-bsize,bsize,1.0f,0.0f},
		//back
		{-bsize,bsize,-bsize,0.0f,0.0f},{bsize,bsize,-bsize,0.0f,1.0f},
		{bsize,-bsize,-bsize,1.0f,1.0f},{-bsize,-bsize,-bsize,1.0f,0.0f},
		//top
		{-bsize,bsize,bsize,0.0f,0.0f},{bsize,bsize,bsize,0.0f,1.0f},
		{bsize,bsize,-bsize,1.0f,1.0f},{-bsize,bsize,-bsize,1.0f,0.0f},
		//bottom
		{-bsize,-bsize,bsize,0.0f,0.0f},{bsize,-bsize,bsize,0.0f,1.0f},
		{bsize,-bsize,-bsize,1.0f,1.0f},{-bsize,-bsize,-bsize,1.0f,0.0f},
		//left
		{-bsize,bsize,-bsize,0.0f,0.0f},{-bsize,bsize,bsize,0.0f,1.0f},
		{-bsize,-bsize,bsize,1.0f,1.0f},{-bsize,-bsize,-bsize,1.0f,0.0f},
		//right
		{bsize,bsize,-bsize,0.0f,0.0f},{bsize,bsize,bsize,0.0f,1.0f},
		{bsize,-bsize,bsize,1.0f,1.0f},{bsize,-bsize,-bsize,1.0f,0.0f},
	};

	GLuint VertexVBOID = 0;
	glGenBuffers(1, &VertexVBOID);
	glBindBuffer(GL_ARRAY_BUFFER, VertexVBOID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pvertex), &pvertex[0].x, GL_STATIC_DRAW);


	MyVertex screen[4] =
	{
		{ -1.0f,1.0f,0,0,0 },
		{ 1.0f,1.0f,0,0,1.0f },
		{ 1.0f,-1.0f,0,1.0f,1.0f },
		{ -1.0f,-1.0f,0,1.0f,0 }
	};
	GLuint screenVBOID = 1;
	glGenBuffers(1, &screenVBOID);
	glBindBuffer(GL_ARRAY_BUFFER, screenVBOID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(screen), &screen[0].x, GL_STATIC_DRAW);


	MyVertex shade_GEO[6] =
	{
		{ -1.0f,0.0f,-1.0f,0,0 },
		{ 1.0f,0.3f,-1.0f,1,1.0f },
		{ 1.0f,-0.4f,-1.0f,0.0f,1.0f },

		{ -1.0f,1.0f,-1.0f,0,0 },
		{ 1.0f,1.0f,-1.0f,1,1.0f },
		{ 0.2f,-0.1f,-1.0f,0.0f,1.0f },
	};
	GLuint shadeVBOID = 1;
	glGenBuffers(1, &shadeVBOID);
	glBindBuffer(GL_ARRAY_BUFFER, shadeVBOID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(shade_GEO), &shade_GEO[0].x, GL_STATIC_DRAW);

	GLuint texture = SOIL_load_OGL_texture(
		"C:/Users/1/Cpp_Tutorials/OpenGL_Projects/JT20_OpenGL_Learning_Curve/JT20_Win32_GL_test_06_blurscreen/hello.png", 
		SOIL_LOAD_AUTO, 
		SOIL_CREATE_NEW_ID, 
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

	GLuint texture2 = SOIL_load_OGL_texture(
		"C:/Users/1/Cpp_Tutorials/OpenGL_Projects/JT20_OpenGL_Learning_Curve/JT20_Win32_GL_test_07_glm - Copy/hello2.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);

	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

	//generating a frame buffer
	
	GLuint color_tex, boom_tex, depth_tex, framebuffer;
	glGenFramebuffersEXT(1, &framebuffer);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebuffer);

	glGenTextures(1, &color_tex);
	glBindTexture(GL_TEXTURE_2D, color_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 800, 600, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, color_tex, 0/*mipmap level*/);

	glGenTextures(1, &boom_tex);
	glBindTexture(GL_TEXTURE_2D, boom_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//1.1 create
	glTexStorage2D(GL_TEXTURE_2D, 10, GL_RGBA8, 800, 600);  //this creates the require mipmaps
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_2D, boom_tex, 0/*mipmap level*/);
	
	glGenTextures(1, &depth_tex);
	glBindTexture(GL_TEXTURE_2D, depth_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, 800, 600, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, depth_tex, 0/*mipmap level*/);

	//if drawing 2 color buffer, you need to use the method bellow
	GLenum buffers[] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT };
	glDrawBuffers(2, buffers);
	
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	
	bool running = true;

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

		//drawing the background
		glClearDepth(1.0f);
		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebuffer);

				glEnable(GL_DEPTH_TEST);
				glClearDepth(1.0f);
				
				glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glUseProgram(program1);


				glBindBuffer(GL_ARRAY_BUFFER, VertexVBOID);

				GLint posAttrib = glGetAttribLocation(program1, "pos");
				glEnableVertexAttribArray(posAttrib);
				glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);

				GLint texAttrib = glGetAttribLocation(program1, "tex");
				glEnableVertexAttribArray(texAttrib);
				glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (const void*)(3 * sizeof(float)));


				static GLint locScale = glGetUniformLocation(program1, "Scale");

				static float Scale1 =  0;
				static float Dir = 1;
				Scale1 += Dir;
				glUniform1f(locScale, Scale1);


				glm::mat4 mat = glm::mat4(1.0f);
				mat = glm::rotate<float>(mat, glm::radians(Scale1*2.0f), glm::vec3(1, 1, 0));
				//mat = glm::scale(mat, glm::vec3(sinf(Scale1*0.08f)*0.4+0.8f, 0.9f, sinf(Scale1 * 0.18f) * 0.4 + 0.8f));
				glUniformMatrix4fv(posAttrib, 1, GL_FALSE, glm::value_ptr(mat));

				//finally drawing the geometry

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texture);
				GLint gli = glGetUniformLocation(program1, "colorMap0");
				glUniform1i(gli, 0);//set colormap value 0

				glDrawArrays(GL_QUADS, 0, 24);
				glBindTexture(GL_TEXTURE_2D, 0);
				glBindBuffer(GL_ARRAY_BUFFER, 0);

				glBindBuffer(GL_ARRAY_BUFFER, shadeVBOID);
					GLint posAttrib_shade = glGetAttribLocation(program1, "pos");
					glEnableVertexAttribArray(posAttrib_shade);
					glVertexAttribPointer(posAttrib_shade, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);

					glm::mat4 mat_x = glm::mat4(1.0f);
					glUniformMatrix4fv(posAttrib, 1, GL_FALSE, glm::value_ptr(mat_x));

					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, texture2);
					GLint gliw = glGetUniformLocation(program1, "colorMap0");
					glUniform1i(gliw, 0);//set colormap value 0


				glDrawArrays(GL_TRIANGLES, 0, 6);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glUseProgram(0);


			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
			
			//1.2 glGenerateMipmap this is needed to update all mipmaps of lower detail
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, boom_tex);
			glGenerateMipmap(GL_TEXTURE_2D);




		glUseProgram(program2);

		glBindBuffer(GL_ARRAY_BUFFER, screenVBOID);
		GLint posAttrib2 = glGetAttribLocation(program2, "pos");
		glEnableVertexAttribArray(posAttrib2);
		glVertexAttribPointer(posAttrib2, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);

		GLint texAttrib2 = glGetAttribLocation(program2, "tex");
		glEnableVertexAttribArray(texAttrib2);
		glVertexAttribPointer(texAttrib2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (const void*)(3 * sizeof(float)));


			static GLint locScale2 = glGetUniformLocation(program2, "Scale");
			static float Scale2 = 0;
			glUniform1f(locScale2, Scale2);

			static GLint locX = glGetUniformLocation(program2, "Xsize");
			glUniform1f(locX, 800.0f);
			static GLint locY = glGetUniformLocation(program2, "Ysize");
			glUniform1f(locY, 600.0f);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, color_tex);
			GLint gli2 = glGetUniformLocation(program2, "colorMap1");
			glUniform1i(gli2, 0);//set colormap value 0

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, boom_tex);
			GLint gli3 = glGetUniformLocation(program2, "colorMap2");
			glUniform1i(gli3, 1);//set colormap value 1

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D,depth_tex);
			GLint gli4 = glGetUniformLocation(program2, "depthMap1");
			glUniform1i(gli4, 2);//set colormap value 2

			glm::mat4 mat_2 = glm::mat4(1.0f);
			glUniformMatrix4fv(posAttrib2, 1, GL_FALSE, glm::value_ptr(mat_2));

			
			glDrawArrays(GL_QUADS, 0, 4);
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

		glUseProgram(0);

		SwapBuffers(dvcContext);
	}



	glDeleteBuffers(1,&VertexVBOID);
	glDeleteFramebuffers(1, &framebuffer);

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

		program1 = CreateCustomProgram(vertexShaderSrc, fragmentShaderSrc_N1);
		program2 = CreateCustomProgram(vertexShaderSrc, fragmentShaderSrc_N2);

		glViewport(0, 0, 800, 600);
		return 0;


	}break;


	case WM_DESTROY:
	{
		//_____________________________new for [GL]
		glDisable(GL_TEXTURE_2D);
		ReleaseDC(hWnd, dvcContext);
		wglDeleteContext(rdrContext);

		glDeleteProgram(program1);
		glDeleteProgram(program2);
		PostQuitMessage(0);
		return 0;

	}break;

	}

	return DefWindowProc(hWnd, userMessage, wParam, lParam);
}

//old pipeline lighting

/*

float a[4] = {1,1,1,0};
glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,a);
float b=1;
glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,&b);
glEnable(GL_COLOR_MATERIAL );

glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
{
	float a[4]={0,0,0,1};
	glLightfv(GL_LIGHT0,GL_POSITION ,a);
}


glEnable(GL_LIGHTING);
glEnable(GL_LIGHT0 );


////////////
{
	float a[4]={sin((angle+4.5)*3.1415926/180),cos((angle+4.5)*3.1415926/180),sin(2*(angle+4.5)*3.1415926/180), 0};
	glLightfv(GL_LIGHT0 ,GL_DIFFUSE ,a);
}


{
	float a[4]={0,0,0,0};
	angle=angle-5;
	a[0]=1*sin(angle*3.1415926/180);
	a[1]=1*cos(angle*3.1415926/180);
	a[2]=0;

	glLightfv(GL_LIGHT0,GL_SPOT_DIRECTION,a);
}

{
	float a=.4;
	glLightfv(GL_LIGHT0,GL_SPOT_EXPONENT,&a);
}
*/

//glVertex3f(...)



