#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include <GL/glew.h>


static GLuint program;


#define GLSL(src) "#version 440\n" #src
void shaderAttach(GLuint program, GLenum type, char* shaderSource = NULL);

const char* vertexShaderSrc = GLSL(
    in vec3 pos;
uniform float Scale;
void main() {
    gl_Position = vec4(pos, 1.0) + vec4(Scale, 0, 0, 0);
}
);

// Fragment shader
const char* fragmentShaderSrc = GLSL(
    uniform float Scale;
void main() {
    gl_FragData[0] = vec4(Scale * 2, 1.0 / (Scale * 3), 1.0, 1.0);
    //gl_FragData[0] = vec4(0,1, 1.0, 1.0);
});



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


//GLuint CreateCustomProgram(const char* VertexSource, const char* FragmentSource) 
//{



//}




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
        float x, y, z;        //Vertex
    };

    MyVertex pvertex[3] = { {0,0,0},{1,0,0},{0,1,0} };

    GLuint VertexVBOID = 0, IndexVBOID = 0;
    glGenBuffers(1, &VertexVBOID);
    glBindBuffer(GL_ARRAY_BUFFER, VertexVBOID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pvertex), &pvertex[0].x, GL_STATIC_DRAW);

    //create the indices(element buffer)
    unsigned short pindices[sizeof(pvertex) / sizeof(MyVertex)];
    for (int i = 0;i < sizeof(pindices) / sizeof(unsigned short);++i) 
        pindices[i] = i;

    glGenBuffers(1, &IndexVBOID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexVBOID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pindices), pindices, GL_STATIC_DRAW);




    bool running = true;

    /*[PART 2.3]----THE 'BIG WHILE LOOP'----*/
    while (running)
    {
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



        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexVBOID);
        GLint posAttrib = glGetAttribLocation(program, "pos");
        glEnableVertexAttribArray(posAttrib);

        glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        glUseProgram(program);

        GLint locScale = glGetUniformLocation(program, "Scale");

        static float Scale1 = -1;
        Scale1 += .001;
        if (1 < Scale1) Scale1 = -1;


        glUniform1f(locScale, Scale1);

        //glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, sizeof(pindices)/sizeof(unsigned short), GL_UNSIGNED_SHORT, 0);
        glUseProgram(0);


        //program = CreateCustomProgram(VertexSource,FragmentSource)

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
        /*[Part 4.3 -start]-------------Menu Creations in here---------------*/
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


        program = glCreateProgram();
        shaderAttach(program, GL_VERTEX_SHADER, vertexShaderSrc);
        shaderAttach(program, GL_FRAGMENT_SHADER, fragmentShaderSrc);
        glLinkProgram(program);

        GLint result = 0;

        glGetProgramiv(program, GL_LINK_STATUS, &result);
        if (result == GL_FALSE) {
            GLint length;
            char* log;
            /* get the program info log */
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
            log = (char*)malloc(length);
            glGetProgramInfoLog(program, length, &result, log);
            /* print an error message and the info log */
            MessageBoxA(hWnd, log, "sceneInit(): Program linking failed:", 0);
            free(log);
            /* delete the program */
            glDeleteProgram(program);
            MessageBoxA(hWnd, "failed", "Linking shader program", 0);
        }


        glViewport(0, 0, 800, 600);
        return 0;


    }break;


    case WM_DESTROY:
    {
        //_____________________________new for [GL]
        glDisable(GL_TEXTURE_2D);
        ReleaseDC(hWnd, dvcContext);
        wglDeleteContext(rdrContext);

        glDeleteProgram(program);
        PostQuitMessage(0);
        return 0;

    }break;

    }

    return DefWindowProc(hWnd, userMessage, wParam, lParam);
}

