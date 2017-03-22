#ifndef TERRAIN_H
#define TERRAIN_H
#include <GL\glew.h>
#include <GL\GL.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <SFML\Window.hpp>
#include <fstream>
#include <vector>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <SOIL.h>
#include "Shader.h"

class Terrain
{
private:
	int w; //width
	int l; //length
	float** hs; // heights
	float scaleFactor;
	glm::vec3** normals;
	bool computedNormals; //check if normal is up-to-date
	GLuint VAO; //Vertex Array Object
	GLuint colorTexture;
	glm::mat4 modelMatrix;
	void setupTexture();
public:
	Terrain(int w2, int l2, float scale);
	~Terrain();
	int getWidth();
	int getLength();
	float getScale();
	float heightAt(float x, float z); //returns the approximate height of the terrain at the specified (x, z) position
	void setHeight(int x, int z, float y); //set the height at (x, z) to y
	float getHeight(int x, int z); //returns the height at (x, z)
	void computeNormals(); //compute the normals, if they haven't been computed yet
	glm::vec3 getNormal(int x, int z);
	void loadTerrain(std::string fileName, float height);
	void draw(Shader shader);
};
#endif // ! 'TERRAIN.H'