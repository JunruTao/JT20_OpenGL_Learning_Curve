#ifndef OBJECTSCORE_H
#define OBJECTSCORE_H
#include <vector>


class ObjectGeo 
{
protected:
	std::vector<std::vector<float>> vertices;
public:
	//Constructor
	ObjectGeo() {};


	//setters and getters;
	void AddVertices(float x, float y, float z);
	std::vector<float> GetGlVertices();
	std::vector<int> GetGlVerticesIndex();
	int GetVerticesCount();
};

#endif
