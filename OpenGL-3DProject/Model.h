#ifndef MODEL_H
#define MODEL_H
#include <vector>
#include <glm\glm.hpp>
class Model
{
private:
	std::vector<glm::vec3> verticies;
	std::vector<glm::vec3> colours;
	std::vector<glm::vec3> normals;
public:
	std::vector<glm::vec3> getVerticies() const;
	std::vector<glm::vec3> getColours() const;
	std::vector<glm::vec3> getNormals() const;
	Model();
	~Model();
};

#endif