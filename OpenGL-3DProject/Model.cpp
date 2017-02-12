#include "Model.h"

std::vector<std::vector<Vertex>> Model::getFaces()
{
	return this->faces;
}

glm::mat4 Model::getModelMatrix()
{
	return this->modelMatrix;
}

void Model::rotate(glm::mat4 rotationMatrix)
{
	this->modelMatrix *= rotationMatrix;
}

void Model::read(std::string filename)
{
	//Removes any old properties
	faces = std::vector<std::vector<Vertex>>();
	//Reads a .obj file and creates a Model object from the data
	std::ifstream file(filename);
	std::string str;
	std::vector<glm::vec3> vertexPos = std::vector<glm::vec3>();
	vertexPos.push_back(glm::vec3(0, 0, 0));
	std::vector<glm::vec3> vertexNormals = std::vector<glm::vec3>();
	vertexNormals.push_back(glm::vec3(0, 0, 0));
	std::vector<glm::vec2> vertexTex = std::vector<glm::vec2>();
	vertexTex.push_back(glm::vec3(0, 0, 0));
	//Gets a single line of the file at a time
	while (std::getline(file, str))
	{
		std::stringstream line;
		double data;
		//Read words of the line one by one
		line << str;
		line >> str;
		if (str == "v")
		{
			//A vertex position
			glm::vec3 aVertexPos;
			//std::cout << "Vertex (v): ";
			//X
			line >> data;
			aVertexPos.x = data;
			//std::cout << data << " ";
			//Y
			line >> data;
			aVertexPos.y = data;
			//std::cout << data << " ";
			//Z
			line >> data;
			aVertexPos.z = data;
			//std::cout << data << " ";

			vertexPos.push_back(aVertexPos);
			//std::cout << std::endl;
		}
		else if (str == "vt")
		{
			//A texture position
			glm::vec2 aVertexTex;
			//std::cout << "Texture Position (vt): ";
			//U
			line >> data;
			aVertexTex.x = data;
			//std::cout << data << " ";
			//V
			line >> data;
			aVertexTex.y = data;
			//std::cout << data << " ";

			vertexTex.push_back(aVertexTex);
			//std::cout << std::endl;
		}
		else if (str == "vn")
		{
			//A normal
			glm::vec3 normal;
			//std::cout << "Normal (vn): ";
			//X
			line >> data;
			normal.x = data;
			//std::cout << data << " ";
			//Y
			line >> data;
			normal.y = data;
			//std::cout << data << " ";
			//Z
			line >> data;
			normal.z = data;
			//std::cout << data << " ";

			vertexNormals.push_back(normal);
			//std::cout << std::endl;
		}
		else if (str == "f")
		{
			//Faces
			//std::cout << "Face (f): ";
			std::vector<Vertex> aFace = std::vector<Vertex>();
			//Split the rest of the line into seperate words
			while (line >> str)
			{
				std::stringstream strIndices;
				strIndices << str;
				int i = 0;
				std::stringstream intIndices;
				//Splits up the indices to be seperated by spaces instead of slashes.
				while (std::getline(strIndices, str, '/'))
				{
					intIndices << str;
					intIndices << " ";
					//Count number of indices to identify either v/vt/vn or v//vn format
					i++;
				};
				Vertex aVertex;
				//Creates a vertex from the data pointed to by the indices
				if (i == 3)
				{
					intIndices >> i;
					aVertex.pos = vertexPos.at(i);
					intIndices >> i;
					aVertex.tex = vertexTex.at(i);
					intIndices >> i;
					aVertex.normal = vertexNormals.at(i);
				}
				else if (i == 2)
				{
					strIndices >> i;
					aVertex.pos = vertexPos.at(i);
					strIndices >> i;
					aVertex.normal = vertexNormals.at(i);
				}
				aVertex.colour = glm::vec4(0,0,1,1);
				aVertex.modelX = glm::vec4(1, 0, 0, 0);
				aVertex.modelY = glm::vec4(0, 1, 0, 0);
				aVertex.modelZ = glm::vec4(0, 0, 1, 0);
				//Adds the vertex to this face
				aFace.push_back(aVertex);
			}
			//Adds the face to the model
			this->faces.push_back(aFace);
			//std::cout << std::endl;
		}
		else if (str == "g")
		{
			//Groups
			//std::cout << "Group name (g): ";
			while (line >> str)
			{
				//std::cout << str << " ";
			}
			//std::cout << std::endl;
		}
		else if (str == "s")
		{
			//Groups
			//std::cout << "Smoothing group (s): ";
			while (line >> str)
			{
				//std::cout << str << " ";
			}
			//std::cout << std::endl;
		}
		else if (str == "mtllib")
		{
			//Material library
			//std::cout << "Material Library (mtllib): ";
			while (line >> str)
			{
				//std::cout << str << " ";
			}
			//std::cout << std::endl;
		}
		else if (str == "usemtl")
		{
			//Material name
			//std::cout << "Material name (usemtl): ";
			while (line >> str)
			{
				//std::cout << str << " ";
			}
			//std::cout << std::endl;
		}
	}
}

Model::Model(std::string filename)
{
	modelMatrix =
	{
		glm::vec4(1,0,0,0),
		glm::vec4(0,1,0,0),
		glm::vec4(0,0,1,0),
		glm::vec4(0,0,0,1)
	};
	read(filename);
}

Model::Model()
{
	modelMatrix =
	{
		glm::vec4(1,0,0,0),
		glm::vec4(0,1,0,0),
		glm::vec4(0,0,1,0),
		glm::vec4(0,0,0,1)
	};
	this->faces = std::vector<std::vector<Vertex>>();
}

Model::~Model()
{

}
