#ifndef TERRAIN_H
#define TERRAIN_H
#include <GL\glew.h>
#include <GL\GL.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <SOIL.h>
#include "Shader.h"

class Terrain
{
private:
	int width;
	int length;
	float** heights;
	float scaleFactor;
	glm::vec3** normals;
	bool computedNormals; //Check if normal is up-to-date
	GLuint VAO; //Vertex Array Object
	GLuint colorTexture;
	glm::mat4 modelMatrix;
	void setupTexture();
public:
	Terrain(int w, int l, float scale);
	~Terrain();
	int getWidth();
	int getLength();
	float getScale();
	float heightAt(float x, float z); //Returns the approximate height of the terrain at the specified (x, z) position
	void setHeight(int x, int z, float y); //Set the height at (x, z) to y
	float getHeight(int x, int z); //Returns the height at (x, z)
	void computeNormals(); //Compute the normals, if they haven't been computed yet
	glm::vec3 getNormal(int x, int z); //Returns the normal at (x, z)
	void loadTerrain(std::string fileName, float height); //Loads a terrain from a heightmap.
	void draw(Shader shader);
};
#endif