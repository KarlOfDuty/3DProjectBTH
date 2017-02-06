#ifndef MODEL_H
#define MODEL_H
#include <vector>
#include <glm\glm.hpp>
//A single vertex (corner) in a face.
struct Vertex
{
	double x, y, z;
	double u, v;
	//double r, g, b, a;
};
class Model
{
private:
	std::vector<std::vector<Vertex>> faces;
	std::vector<glm::vec3> normals;
public:
	std::vector<glm::vec3> getNormals() const;
	std::vector<std::vector<Vertex>> getFaces() const;
	void addFace(std::vector<Vertex> face);
	Model();
	~Model();
};
#endif