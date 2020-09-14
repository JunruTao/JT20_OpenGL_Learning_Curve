#include "Mesh.h"

bool Mesh::LoadFromObjFile(const char* FilePath)
{
	std::vector<unsigned int> m_vertexIndices;
	std::vector<unsigned int> m_uvIndices;
	std::vector<unsigned int> m_normalIndices;

	std::vector<glm::vec3> m_vertices;
	std::vector<glm::vec2> m_uvs;
	std::vector<glm::vec3> m_normals;

	FILE* file = fopen(FilePath, "r");
	if (file == NULL) {
		MessageBoxA(NULL, "Impossible to open the file !", "OBJ_Loading", 0);
		return false;
	}
	else {
		while (1)
		{
			char lineHeader[128];
			// read the first word of the line
			int res = fscanf(file, "%s", lineHeader);
			if (res == EOF)
				break; // EOF = End Of File. Quit the loop.

			// else : parse lineHeader
			if (strcmp(lineHeader, "v") == 0) {
				glm::vec3 vertex;
				fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
				m_vertices.push_back(vertex);

			}
			else if (strcmp(lineHeader, "vt") == 0) {
				glm::vec2 uv;
				fscanf(file, "%f %f\n", &uv.x, &uv.y);
				m_uvs.push_back(uv);
			}
			else if (strcmp(lineHeader, "vn") == 0) {
				glm::vec3 normal;
				fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
				m_normals.push_back(normal);
			}
			else if (strcmp(lineHeader, "f") == 0) {

				std::string vertex1, vertex2, vertex3;
				unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];

				//Wavefront OBJ: face:
				//f 5/1/1 1/2/1 4/3/1
				int matches = fscanf(
					file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
					&vertexIndex[0],
					&uvIndex[0],
					&normalIndex[0],
					&vertexIndex[1],
					&uvIndex[1],
					&normalIndex[1],
					&vertexIndex[2],
					&uvIndex[2],
					&normalIndex[2]);


				if (matches != 9) {
					MessageBoxA(NULL, "File can't be read by our simple parser : ( Try exporting with other options\n", "OBJLoaderError", 0);
					return false;
				}
				else {
					m_vertexIndices.push_back(vertexIndex[0]);
					m_vertexIndices.push_back(vertexIndex[1]);
					m_vertexIndices.push_back(vertexIndex[2]);

					m_uvIndices.push_back(uvIndex[0]);
					m_uvIndices.push_back(uvIndex[1]);
					m_uvIndices.push_back(uvIndex[2]);

					m_normalIndices.push_back(normalIndex[0]);
					m_normalIndices.push_back(normalIndex[1]);
					m_normalIndices.push_back(normalIndex[2]);
				}
			}
		}//end while
		for (unsigned int i = 0; i < m_vertexIndices.size(); i++) 
		{
			unsigned int vertexIndex = m_vertexIndices[i];
			glm::vec3 vertex = m_vertices[vertexIndex - 1];
			_vertices.emplace_back(std::move(vertex));
		}
		for (unsigned int i = 0; i < m_uvIndices.size(); i++)
		{
			unsigned int uvIndex = m_uvIndices[i];
			glm::vec2 uv = m_uvs[uvIndex - 1];
			_uvs.emplace_back(std::move(uv));
		}

		for (unsigned int i = 0; i < m_normalIndices.size(); i++)
		{
			unsigned int normalIndex = m_normalIndices[i];
			glm::vec3 normal = m_normals[normalIndex - 1];
			_normals.emplace_back(std::move(normal));
		}
		SendDataToGPU();

		return true;
	}//end elif
}


void Mesh::SendDataToGPU() 
{
	//Send Vertex Array Buffer data To GPU;
	glGenBuffers(1, &VertVBO_id);
	glBindBuffer(GL_ARRAY_BUFFER, VertVBO_id);
	glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(glm::vec3), &_vertices[0].x, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &UVTexVBO_id);
	glBindBuffer(GL_ARRAY_BUFFER, UVTexVBO_id);
	glBufferData(GL_ARRAY_BUFFER, _uvs.size() * sizeof(glm::vec2), &_uvs[0].x, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::AttributeBinding(GLuint ProgramID, const char* pos, const char* tex )
{
	glBindBuffer(GL_ARRAY_BUFFER, VertVBO_id);
		GLint posAttrib = glGetAttribLocation(ProgramID, pos);
		glEnableVertexAttribArray(posAttrib);
		glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, UVTexVBO_id);
		GLint texAttrib = glGetAttribLocation(ProgramID, tex);
		glEnableVertexAttribArray(texAttrib);
		glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::Draw() 
{
	glBindBuffer(GL_ARRAY_BUFFER, VertVBO_id);
		glDrawArrays(GL_TRIANGLES, 0, _vertices.size());
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Mesh::~Mesh() 
{
	glDeleteBuffers(1, &VertVBO_id);
	glDeleteBuffers(1, &UVTexVBO_id);
}