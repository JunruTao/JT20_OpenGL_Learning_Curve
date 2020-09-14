#ifndef MESH_H
#define MESH_H

#include <vector>
#include <Windows.h>
#include <string>
#include <stdio.h>
#include "glm/glm.hpp"
#include "glm/common.hpp"
#include "glm/ext.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "glm/matrix.hpp"
#include "GL/glew.h"


class Mesh
{
public:
	Mesh():VertVBO_id(0), UVTexVBO_id(0) {};
	~Mesh();

	bool LoadFromObjFile(const char* FilePath);
	void SendDataToGPU();
	void AttributeBinding(GLuint ProgramID, const char* pos = "pos", const char* tex = "tex");
	void Draw();

private:
	GLuint VertVBO_id;
	GLuint UVTexVBO_id;

	std::vector<glm::vec3> _vertices;
	std::vector<glm::vec2> _uvs;
	std::vector<glm::vec3> _normals;

};



#endif