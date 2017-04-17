#include "Terrain.h"
//Constructor
Terrain::Terrain(int w, int l, float scale)
{
	width = w;
	length = l;
	//Initialize heights array
	heights = new float*[length];
	for (int i = 0; i < length; i++)
	{
		heights[i] = new float[width];
	}
	//Initialize normals arrray
	normals = new glm::vec3*[length];
	for (int i = 0; i < length; i++)
	{
		normals[i] = new glm::vec3[width];
	}

	computedNormals = false;

	//Scale and positioning for terrain
	scaleFactor = scale;
	modelMatrix = glm::mat4({
		scaleFactor, 0.0, 0.0, 0.0,
		0.0, scaleFactor, 0.0, 0.0,
		0.0, 0.0, scaleFactor, 0.0,
		0.0, 0.0, 0.0, 1.0 });
	setupTexture();
}
//Destructor
Terrain::~Terrain()
{
	for (int i = 0; i < length; i++)
	{
		delete[] heights[i];
	}
	delete[] heights;

	for (int i = 0; i < length; i++)
	{
		delete[] normals[i];
	}
	delete[] normals;
}
//Sets up the color of the height map
void Terrain::setupTexture()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	
	glGenTextures(1, &colorTexture);
	glBindTexture(GL_TEXTURE_2D, colorTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	float colour[3] = {0,0.5,0};

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_FLOAT, colour);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}
//Getters
int Terrain::getWidth()
{
	return width;
}
int Terrain::getLength()
{
	return length;
}
float Terrain::getScale()
{
	return scaleFactor;
}
glm::vec3 Terrain::getNormal(int x, int z)
{
	if (!computedNormals)
	{
		computeNormals();
	}
	return normals[z][x];
}
//Setters
void Terrain::setHeight(int x, int z, float y)
{
	heights[z][x] = y;
	computedNormals = false;
}
float Terrain::getHeight(int x, int z)
{
	return heights[z][x];
}
//Returns the height at a specific position
float Terrain::heightAt(float x, float z)
{
	//Make (x, z) lie within the bounds of the terrain
	x /= scaleFactor;
	z /= scaleFactor;
	if (x < 0)
	{
		x = 0;
	}
	else if (x > getWidth() - 1)
	{
		x = getWidth() - 1;
	}

	if (z < 0)
	{
		z = 0;
	}
	else if (z > getLength() - 1)
	{
		z = getLength() - 1;
	}

	//Compute the grid cell in which (x, z) lies and how close we are to the
	//left and outward edges
	int leftX = (int)x;
	if (leftX == getWidth() - 1) {
		leftX--;
	}
	//Distance from leftX
	float fracX = x - leftX;

	int outZ = (int)z;
	if (outZ == getLength() - 1) {
		outZ--;
	}
	//Distance from outZ
	float fracZ = z - outZ;

	//Compute the four heights for the grid cell
	float h11 = getHeight(leftX, outZ);
	float h12 = getHeight(leftX, outZ + 1);
	float h21 = getHeight(leftX + 1, outZ);
	float h22 = getHeight(leftX + 1, outZ + 1);

	//Take a weighted average of the four heights
	return (1 - fracX) * ((1 - fracZ) * h11 + fracZ * h12) +
		fracX * ((1 - fracZ) * h21 + fracZ * h22);
}
//Computes normals
void Terrain::computeNormals()
{
	if (computedNormals) 
	{
		return;
	}

	//Vector for rough normals - before smoothing (approximate normals)
	glm::vec3** normals2 = new glm::vec3*[length];
	for (int i = 0; i < length; i++) 
	{
		normals2[i] = new glm::vec3[width];
	}

	for (int z = 0; z < length; z++) 
	{
		for (int x = 0; x < width; x++) 
		{
			glm::vec3 sum(0.0f, 0.0f, 0.0f);

			//Computing four edges for each point
			glm::vec3 out;
			if (z > 0) {
				out = glm::vec3(0.0f, heights[z - 1][x] - heights[z][x], -1.0f);
			}
			glm::vec3 in;
			if (z < length - 1) {
				in = glm::vec3(0.0f, heights[z + 1][x] - heights[z][x], 1.0f);
			}
			glm::vec3 left;
			if (x > 0) {
				left = glm::vec3(-1.0f, heights[z][x - 1] - heights[z][x], 0.0f);
			}
			glm::vec3 right;
			if (x < width - 1) {
				right = glm::vec3(1.0f, heights[z][x + 1] - heights[z][x], 0.0f);
			}

			//Cross product of a pair of edges to determine the vector perpendicular to a triangle
			if (x > 0 && z > 0) {
				sum += glm::normalize(glm::cross(out, left));
			}
			if (x > 0 && z < length - 1) {
				sum += glm::normalize(glm::cross(left, in));
			}
			if (x < width - 1 && z < length - 1) {
				sum += glm::normalize(glm::cross(in, right));
			}
			if (x < width - 1 && z > 0) {
				sum += glm::normalize(glm::cross(right, out));
			}

			normals2[z][x] = sum;
		}
	}

	//Smooth out the normals
	//For each normal, we average in a little bit of the surrounding normals.
	//Each adjacent normal gets a weight of 0.5, normal at the point gets a weight of 1
	const float FALLOUT_RATIO = 0.5f;
	for (int z = 0; z < length; z++) {
		for (int x = 0; x < width; x++) {
			glm::vec3 sum = normals2[z][x];

			if (x > 0) {
				sum += normals2[z][x - 1] * FALLOUT_RATIO;
			}
			if (x < width - 1) {
				sum += normals2[z][x + 1] * FALLOUT_RATIO;
			}
			if (z > 0) {
				sum += normals2[z - 1][x] * FALLOUT_RATIO;
			}
			if (z < length - 1) {
				sum += normals2[z + 1][x] * FALLOUT_RATIO;
			}


			float magnitude = glm::sqrt(sum[0] * sum[0] + sum[1] * sum[1] + sum[2] * sum[2]);

			//If the average turns out to be zero vector we just set a random vector since a zero vector can not be normalized
			if (magnitude == 0) {
				sum = glm::vec3(0.0f, 1.0f, 0.0f);
			}
			normals[z][x] = sum;
		}
	}

	for (int i = 0; i < length; i++) 
	{
		delete[] normals2[i];
	}
	delete[] normals2;

	computedNormals = true;
}
//Loads the heightmap texture
void Terrain::loadTerrain(std::string fileName, float height)
{	
	int imageWidth, imageHeight, channels;
	unsigned char* image;
	image = SOIL_load_image(fileName.c_str(), &imageWidth, &imageHeight, &channels, SOIL_LOAD_L);
	for (int y = 0; y < imageHeight; y++) 
	{
		for (int x = 0; x < imageWidth; x++) 
		{
			unsigned char color = (unsigned char)image[imageWidth* y + x];
			// Set height - whiter color means higher height, darker color means lower height
			float h = height * ((color / 255.0f) - 0.5f);
			setHeight(x, y, h);
		}
	}
	// compute the normals
	this->computeNormals();
	SOIL_free_image_data(image);
}
//Draws the terrain
void Terrain::draw(Shader shader)
{
	glBindVertexArray(this->VAO);
	
	//Color texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorTexture);
	glUniform1i(glGetUniformLocation(shader.program, "diffuseTexture"), 0);
	
	glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, &modelMatrix[0][0]);
	glUniform1i(glGetUniformLocation(shader.program, "isMouseOvered"), 0);
	
	for (int z = 0; z < getLength() - 1; z++)
	{
		//Makes OpenGL draw a triangle at every three consecutive vertices
		glBegin(GL_TRIANGLE_STRIP);
		for (int x = 0; x < getWidth(); x++)
		{
			glm::vec3 normal = getNormal(x, z);
			glNormal3f(normal[0], normal[1], normal[2]);
			glVertex3f(x, getHeight(x, z), z);
			normal = getNormal(x, z + 1);
			glNormal3f(normal[0], normal[1], normal[2]);
			glVertex3f(x, getHeight(x, z + 1), z + 1);
		}
		glEnd();
	}
	glBindVertexArray(0);
}