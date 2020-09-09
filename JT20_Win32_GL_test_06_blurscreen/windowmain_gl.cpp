
#include <windows.h>
#include <stdio.h>
#include <GL/glew.h>
#include <SOIL/SOIL.h>
#include <thread>
#include <chrono>

static GLuint program1 = 0;
static GLuint program2 = 0;


#define GLSL(src) "#version 440\n" #src
void shaderAttach(GLuint program, GLenum type, char* shaderSource = NULL);

const char* vertexShaderSrc = GLSL(
	in vec3 pos;
    in vec2 tex;
	out vec2 texOut;
	uniform float Scale;

	void main()
	{
		gl_Position = vec4(pos, 1.0) - vec4(0, 0, Scale, 0);
		texOut = tex;
	}
);

// Fragment shader 1
const char* fragmentShaderSrc_N1 = GLSL(
	uniform float Scale;
	in vec2 texOut;
	uniform sampler2D colorMap0;
	out vec4 FragColor;
	

	void main()
	{
		vec2 temp = texOut.yx;
		vec4 tempColor0 = texture(colorMap0, temp);
		FragColor = tempColor0;
		
	}
);



// Fragment shader 2
const char* fragmentShaderSrc_N2 = GLSL(
	uniform float Scale;
	uniform float Blur;
	uniform float Xsize;
	uniform float Ysize;
	in vec2 texOut;
	uniform sampler2D colorMap0;
	uniform sampler2D depthMap1;
	out vec4 FragColor;

void main()
{
	vec2 temp = texOut.yx;
	vec2 temp1 = temp;
	vec4 tempColor0 = texture(colorMap0, temp1);
	vec4 tempColor2 = tempColor0;

	float pBlur = Blur;
	for (int xBlur = 1; xBlur <= pBlur; ++xBlur) {
		for (int yBlur = 1; yBlur <= pBlur; ++yBlur) {
			temp1.x = temp.x + (xBlur / Xsize);
			temp1.y = temp.y + (yBlur / Ysize);
			vec4 tempColor1 = texture(colorMap0, temp1.xy);
			tempColor0 = tempColor0 + tempColor1;
		}
	}

	vec4 tempColor3 = texture(depthMap1, texOut.xy);
	tempColor0 = tempColor2 * (1- tempColor3.x) + (tempColor3.x * (tempColor0 / (pBlur * pBlur)));
	//if ((tempColor0[0] + tempColor0[1] + tempColor0[2]) / 3 < 0.5) { tempColor0 = vec4(0,0,0,0); }
	FragColor = tempColor0;

}
);


// Fragment shader 3
const char* fragmentShaderSrc_N3 = GLSL(
	uniform float Scale;
uniform float Blur;
uniform float Xsize;
uniform float Ysize;
in vec2 texOut;
uniform sampler2D colorMap1;
out vec4 FragColor;

void main()
{
	vec2 temp = texOut.yx;
	vec2 temp1 = temp;
	vec4 tempColor0 = texture(colorMap1, temp);
	vec4 tempColor1 = texture(colorMap1, temp);


	temp1.x = temp.x + (Blur / Xsize);
	temp1.y = temp.y + (Blur / Ysize);
	vec4 tempColor1 = texture(colorMap1, temp1);
	temp1.x = temp.x - (Blur / Xsize);
	temp1.y = temp.y + (Blur / Ysize);
	vec4 tempColor2 = texture(colorMap1, temp1);
	temp1.x = temp.x - (Blur / Xsize);
	temp1.y = temp.y - (Blur / Ysize);
	vec4 tempColor3 = texture(colorMap1, temp1);
	temp1.x = temp.x + (Blur / Xsize);
	temp1.y = temp.y - (Blur / Ysize);
	vec4 tempColor4 = texture(colorMap1, temp1);

	temp1.x = temp.x + (Blur / Xsize);
	temp1.y = temp.y;
	vec4 tempColor5 = texture(colorMap1, temp1);
	temp1.x = temp.x - (Blur / Xsize);
	temp1.y = temp.y;
	vec4 tempColor6 = texture(colorMap1, temp1);
	temp1.x = temp.x;
	temp1.y = temp.y - (Blur / Ysize);
	vec4 tempColor7 = texture(colorMap1, temp1);
	temp1.x = temp.x;
	temp1.y = temp.y - (Blur / Ysize);
	vec4 tempColor8 = texture(colorMap1, temp1);


	tempColor0 = (tempColor0 + tempColor1 + tempColor2 + tempColor3 + tempColor4 + tempColor5 + tempColor6 + tempColor7 + tempColor8) / 9.0f;
	if ((tempColor0[0] + tempColor0[1] + tempColor0[2]) / 3 < 0.8) { tempColor0 = 0; }

	temp.x += sin(Scale);
	temp.y += cos(Scale);
	FragColor = tempColor0;

}
);


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

	static float bsize = 0.7f;
	MyVertex pvertex[8] =
	{
		{-bsize,bsize,1,0,0},{bsize,bsize,1,0,1},{bsize,-bsize,-1,1,1},{-bsize,-bsize,-1,1,0},
		{ -0.9f,0.9f,0,0,0 },
		{ 0.9f,0.9f,0,0,1.0f },
		{ 0.9f,-0.9f,0,1.0f,1.0f },
		{ -0.9f,-0.9f,0,1.0f,0 }
	};

	GLuint VertexVBOID = 0;
	glGenBuffers(1, &VertexVBOID);
	glBindBuffer(GL_ARRAY_BUFFER, VertexVBOID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(pvertex), &pvertex[0].x, GL_STATIC_DRAW);

	//GLuint screenVBOID = 1;
	//glGenBuffers(1, &screenVBOID);
	//glBindBuffer(GL_ARRAY_BUFFER, screenVBOID);
	//	glBufferData(GL_ARRAY_BUFFER, sizeof(screen), &screen[0].x, GL_STATIC_DRAW);


	GLuint texture = SOIL_load_OGL_texture(
		"C:/Users/1/Cpp_Tutorials/OpenGL_Projects/JT20_OpenGL_Learning_Curve/JT20_Win32_GL_test_06_blurscreen/hello.png", 
		SOIL_LOAD_AUTO, 
		SOIL_CREATE_NEW_ID, 
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

	//generating a frame buffer
	GLuint color_tex, depth_tex, framebuffer;
	glGenTextures(1, &color_tex);
	glBindTexture(GL_TEXTURE_2D, color_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 800, 600, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);

	glGenTextures(1, &depth_tex);
	glBindTexture(GL_TEXTURE_2D, depth_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	//NULL means reserve texture memory, but texels are undefined
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, 800, 600, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);


	//glGenFramebuffers(1, &framebuffer);
	//	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebuffer);
	//	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, color_tex,0);
	//	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, depth_tex, 0);
	//glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	glGenFramebuffersEXT(1, &framebuffer);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebuffer);
	//Attach 2D texture to this FBO
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, color_tex, 0/*mipmap level*/);
	//-------------------------
	//Attach depth texture to FBO
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, depth_tex, 0/*mipmap level*/);

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

		GLint posAttrib = glGetAttribLocation(program1, "pos");
		glEnableVertexAttribArray(posAttrib);
		glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);

		GLint texAttrib = glGetAttribLocation(program1, "tex");
		glEnableVertexAttribArray(texAttrib);
		glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (const void*)(3 * sizeof(float)));

		
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebuffer);
			glEnable(GL_DEPTH_TEST);
				glClearDepth(1.0f);
				glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				glUseProgram(program1);

				static GLint locScale = glGetUniformLocation(program1, "Scale");

				static float Scale1 =  -1;
				static float Dir = 1;
				if (1 < Scale1 || Scale1 < -1) Dir *= -1;
				Scale1 += Dir * 0.02f;
				glUniform1f(locScale, Scale1);

				//finally drawing the geometry

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texture);
				GLint gli = glGetUniformLocation(program1, "colorMap0");
				glUniform1i(gli, 0);//set colormap value 0

				glDrawArrays(GL_QUADS, 0, 4);
				glBindTexture(GL_TEXTURE_2D, 0);
				glUseProgram(0);

			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		

		glUseProgram(program2);

		static GLint locScale2 = glGetUniformLocation(program2, "Scale");
		static float Scale2 = 0;
		glUniform1f(locScale2, Scale2);

		static GLint locBlur = glGetUniformLocation(program2, "Blur");
		glUniform1f(locBlur, 30.0f);

		static GLint locX = glGetUniformLocation(program2, "Xsize");
		glUniform1f(locX, 800.0f);
		static GLint locY = glGetUniformLocation(program2, "Ysize");
		glUniform1f(locY, 600.0f);

		//finally drawing the geometry

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depth_tex);
		GLint gli2 = glGetUniformLocation(program2, "depthMap1");
		glUniform1i(gli2, 0);//set colormap value 0

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, color_tex);
		GLint gli3 = glGetUniformLocation(program2, "colorMap0");
		glUniform1i(gli3, 1);//set colormap value 0

		glDrawArrays(GL_QUADS, 4, 4);
		glBindTexture(GL_TEXTURE_2D, 0);
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

