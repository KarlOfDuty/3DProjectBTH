#ifndef MODEL_H
#define MODEL_H
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <GL\glew.h>
#include <GL\GL.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <SOIL.h>
#include "Shader.h"
//A material specifying how shading, coloring and texturing works
struct Material
{
	std::string name;
	glm::vec3 ambientColour;
	glm::vec3 diffuseColour;
	glm::vec3 specularColour;
	GLuint diffuseTexture;
	std::string textureMapAmbientFile;
	std::string textureMapDiffuseFile;
	std::string textureMapSpecularFile;
	std::string normalMapFile;
	float transparency;
	int illuminationMode;
	bool hasTextures = false;
	//Searching Functions
	static int findMaterial(std::string name, std::vector<Material> materials);
	int findMaterial(std::vector<Material> materials);
};
//A single vertex (corner) in a face.
struct Vertex
{
	glm::vec3 pos;
	glm::vec2 texPos;
	glm::vec4 colour;
	glm::vec3 normal;
	Material material;
};
//Turns on console feedback for reading of model files
static bool modelDebug = false;
//Turns on console feedback for reading of material files
static bool matDebug = true;
class Model
{
private:
	std::vector<std::vector<Vertex>> faces;
	glm::mat4 modelMatrix;
	glm::mat4 rotationMatrix;
public:
	GLuint VAO; //Vertex Array Object
	GLuint VBO; //Vertex Buffer Object
	Material getMaterial(int index);
	glm::mat4 getModelMatrix() const;
	glm::mat4 getRotationMatrix() const;
	void setModelMatrix(glm::mat4 modelMat);
	void setRotationMatrix(glm::mat4 rotationMat);
	void rotate();
	void read(std::string filename);
	void draw(Shader shader);
	void setupMesh();
	Model(std::string filename);
	Model(std::string filename, glm::mat4 modelMat);
	Model(std::string filename, glm::mat4 modelMat, glm::mat4 rotation);
	Model();
	~Model();
};
#endif