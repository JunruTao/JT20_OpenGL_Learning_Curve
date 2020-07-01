#include <vector>
#include <GL/glew.h>
#include <stdexcept>
#include "ObjectsCore.h"



void ObjectGeo::AddVertices(float x, float y, float z)
{
	vertices.push_back(std::vector<float>{x, y, z});

}

std::vector<float> ObjectGeo::GetGlVertices()
{
	std::vector<float> data;
	for (std::vector<float> point : vertices)
	{
		data.push_back(point[0]);
		data.push_back(point[1]);
		data.push_back(point[2]);
	}
	return data;
}

std::vector<int> ObjectGeo::GetGlVerticesIndex()
{

	std::vector<GLint> index;
	for (size_t i = 0; i < vertices.size();++i)
	{
		index.push_back(i);
	}
	return index;

}

int ObjectGeo::GetVerticesCount()
{
	return vertices.size();
}