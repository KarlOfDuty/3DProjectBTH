#include "Model.h"

std::vector<glm::vec3> Model::getNormals() const
{
	return normals;
}

std::vector<std::vector<Vertex>> Model::getFaces() const
{
	return this->faces;
}

void Model::addFace(std::vector<Vertex> face)
{
	this->faces.push_back(face);
}

Model::Model()
{
	this->normals = std::vector<glm::vec3>();
	this->faces = std::vector<std::vector<Vertex>>();
}

Model::~Model()
{

}
