#ifndef MODEL_H
#define MODEL_H
#include <vector>
#include<fstream>
#include<sstream>
#include<iostream>
#include <glm\glm.hpp>
//A single vertex (corner) in a face.
struct Vertex
{
	glm::vec3 pos;
	glm::vec2 tex;
	//glm::vec4 colour;
	glm::vec3 normal;
};
class Model
{
private:
	std::vector<std::vector<Vertex>> faces;
public:
	std::vector<std::vector<Vertex>> getFaces() const;
	void Model::read(std::string filename);
	Model();
	~Model();
};
#endif