
#include <windows.h>
#include <stdio.h>
#include <GL/glew.h>
#include <SOIL/SOIL.h>
#include <thread>
#include <chrono>

static GLuint program1 = 0;


#define GLSL(src) "#version 440\n" #src
void shaderAttach(GLuint program, GLenum type, char* shaderSource = NULL);

const char* vertexShaderSrc = GLSL(
	in vec3 pos;
in vec2 tex;

out vec2 texOut;
out vec4 position;

uniform float Scale;

void main() {
	position = vec4(pos.xyz, 1.0);
	texOut = tex;
}
);


const char* domainSrc = GLSL(
	layout(triangles, equal_spacing, ccw) in;

// equal_spacing, fractional_even_spacing, or fractional_odd_spacing
uniform sampler2D colorMap0;

in vec2 texOut1[];
in vec4 position1[];

out vec2 texOut;
out vec4 position;
void main()
{
	
	position.xyzw = position1[0].xyzw * gl_TessCoord.x +
		position1[1].xyzw * gl_TessCoord.y +
		position1[2].xyzw * gl_TessCoord.z;

	texOut = texOut1[0].xy * +gl_TessCoord.x +
		texOut1[1].xy * +gl_TessCoord.y +
		texOut1[2].xy * +gl_TessCoord.z;

	vec4 color = texture(colorMap0, texOut);
	position = position + color * 0.03;
}
);

const char* HullConstantSrc = GLSL(
layout(vertices = 3) out;
in vec4 position[];
in vec2 texOut[];

uniform float Scale;

out vec4 position1[];
out vec2 texOut1[];

void main(void)
{
	gl_TessLevelOuter[0] = 40.0 + sin(Scale) * 4;
	gl_TessLevelOuter[1] = 40.0 + sin(Scale) * 4;
	gl_TessLevelOuter[2] = 40.0 + sin(Scale) * 4;

	gl_TessLevelInner[0] = 40.0 + sin(Scale) * 4;

	position1[gl_InvocationID] = position[gl_InvocationID];
	texOut1[gl_InvocationID] = texOut[gl_InvocationID];
}
);


// Geometry shader
const char* GemoetryShaderSrc = GLSL(
	layout(triangles) in;
	layout(triangle_strip, max_vertices = 300) out;
	in vec4 position[];
	in vec2 texOut[];

out vec2 tex;
void main() {

	vec4 vertex[3];

	vertex[0] = position[0];//gl_in[0].gl_Position;
	vertex[1] = position[1];//gl_in[1].gl_Position;
	vertex[2] = position[2];//gl_in[2].gl_Position;

	gl_Position = vertex[0];
	tex = texOut[0];
	EmitVertex();

	gl_Position = vertex[1];
	tex = texOut[1];
	EmitVertex();

	gl_Position = vertex[2];
	tex = texOut[2];
	EmitVertex();

	EndPrimitive();
	

}
);


// Fragment shader
const char* fragmentShaderSrc = GLSL(
	in vec2 tex;
	uniform sampler2D colorMap0;

void main() {
	gl_FragData[0] = texture(colorMap0, tex);
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


GLuint CreateCustomProgram(const char* VertexSource, const char* FragmentSource = NULL)
{

	GLuint nprogram = glCreateProgram();
	shaderAttach(nprogram, GL_VERTEX_SHADER,vertexShaderSrc);

	shaderAttach(nprogram,GL_TESS_CONTROL_SHADER,HullConstantSrc);
	shaderAttach(nprogram,GL_TESS_EVALUATION_SHADER,domainSrc);

    //Geometry Shader
	shaderAttach(nprogram, GL_GEOMETRY_SHADER,GemoetryShaderSrc);
	shaderAttach(nprogram, GL_FRAGMENT_SHADER, fragmentShaderSrc);


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

	static float bsize = 0.8f;
	MyVertex pvertex[3] = 
	{ 
		{-bsize,bsize,0,0,0},{bsize,bsize,0,1,1},{bsize,-bsize,0,0,1}
	};

	GLuint VertexVBOID = 0, IndexVBOID = 0;
	glGenBuffers(1, &VertexVBOID);
	glBindBuffer(GL_ARRAY_BUFFER, VertexVBOID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pvertex), &pvertex[0].x, GL_STATIC_DRAW);

	//create the indices(element buffer)
	//unsigned short point_indices[sizeof(pvertex) / sizeof(MyVertex)];
	//for (int i = 0;i < sizeof(point_indices) / sizeof(unsigned short);++i)
	//	point_indices[i] = i;

	//glGenBuffers(1, &IndexVBOID);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexVBOID);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(point_indices), point_indices, GL_STATIC_DRAW);

	GLuint texture = -1;
	//if return 0, means not found
	texture	= SOIL_load_OGL_texture(
		"C:/Users/1/Cpp_Tutorials/OpenGL_Projects/JT20_OpenGL_Learning_Curve/JT20_Win32_GL_test_14_Tessallation/hello.png", 
		SOIL_LOAD_AUTO, 
		SOIL_CREATE_NEW_ID, 
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);


	bool running = true;
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


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
		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(program1);
		glBindBuffer(GL_ARRAY_BUFFER, VertexVBOID);

		GLint posAttrib = glGetAttribLocation(program1, "pos");
		glEnableVertexAttribArray(posAttrib);
		glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);


		GLint texAttrib = glGetAttribLocation(program1, "tex");
		glEnableVertexAttribArray(texAttrib);
		glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (const void*)(3 * sizeof(float)));

		
			//get the uniform id from shader program
		static GLint locScale = glGetUniformLocation(program1, "Scale");
			//currently using this scale to move around
			static float Scale1 = 1;
			Scale1 += 0.04;

			glUniform1f(locScale, Scale1);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture);
			GLint gli = glGetUniformLocation(program1, "colorMap0");
			glUniform1i(gli, 0);//set colormap value 0

			glPatchParameteri(GL_PATCH_VERTICES, 3);
			glDrawArrays(GL_PATCHES, 0, 3);


		glUseProgram(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		SwapBuffers(dvcContext);
	}




	glDeleteBuffers(1, &VertexVBOID);
	glDeleteBuffers(1, &IndexVBOID);

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


		program1 = CreateCustomProgram(vertexShaderSrc);

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
		PostQuitMessage(0);
		return 0;

	}break;

	}

	return DefWindowProc(hWnd, userMessage, wParam, lParam);
}

