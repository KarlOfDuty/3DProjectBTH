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
	glm::vec4 colour;
	glm::vec3 normal;
	glm::vec4 modelX;
	glm::vec4 modelY;
	glm::vec4 modelZ;
};
class Model
{
private:
	std::vector<std::vector<Vertex>> faces;
	glm::mat4 modelMatrix;
public:
	std::vector<std::vector<Vertex>> getFaces();
	glm::mat4 getModelMatrix();
	void rotate(glm::mat4 rotationMatrix);
	void read(std::string filename);
	Model(std::string filename);
	Model();
	~Model();
};
#endif