# JT20_OpenGL_Learning_Curve
A Learning Curve creating openGL_tests. As a beginner for modern OpenGL, this repository is created fullfilling the purpose of learning OpenGL and computer graphics.
And also at the same time, engaging the practice of C++ language in play organise proper gl program with interfaces. This will be a process of documenting.
****
## 6/27/2020 First Commit: Project-JT20_gl_pj001_win
  * First Gl window in the context of GLFW
  * Initialise GLFW and GLEW
  * Create the _'Big While Loop'_ for running program.
  * Using `glClearColor()`function to fill the gl-viewport with a certain colour.
***
## 7/1/2020 Update: Project-JT20_gl_pj002_Triangles
  * Drawing the First geo(Triangle/Polygon) -> Switch between `GL_TRIANGLES` and `GL_TRIANGLE_FAN`
  * Setting up a few header files seperating these functions/classes:
    * **ObjectsCore** -> An class stores geometry information, vertices are stored in `std::verctor<std::vector<float>>`, and then using a few setters and getter functions to establish interface to bridge other contexts in this case the openGL;
    * **GL_Create_Geometrys** -> here stores a function taking the object and VAO VBO as reference, establish the data and send to buffer
    * **GL_ShaderFunctions** -> here basically relates to functions create/compile shaders.
 
 Therefore using this structure, it will be possible to work further clearly rather than putting all the functions in one cpp file. I can seperate the shaders, shader compiling/validating, openGL geometry buffer and attribute inputs, geometry classes and methods, and context-main(glfw/win32).
