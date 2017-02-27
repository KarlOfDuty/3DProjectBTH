#include "Model.h"
#define BUFFER_OFFSET(i) ((char *)nullptr + (i))
//Returns the index of a material in the vector matching provided name, -1 if not found
int Material::findMaterial(std::string name, std::vector<Material> materials)
{
	int index = -1;
	for (int i = 0; i < materials.size() && index == -1; i++)
	{
		if (materials.at(i).name == name)
		{
			index = i;
		}
	}
	return index;
}
//Returns the index of a material in the vector matching this material's name, -1 if not found
int Material::findMaterial(std::vector<Material> materials)
{
	int index = -1;
	for (int i = 0; i < materials.size() && index == -1; i++)
	{
		if (materials.at(i).name == this->name)
		{
			index = i;
		}
	}
	return index;
}
//Getters
glm::mat4 Model::getModelMatrix() const
{
	return this->modelMatrix;
}
glm::mat4 Model::getRotationMatrix() const
{
	return this->rotationMatrix;
}
//Setters
void Model::setModelMatrix(glm::mat4 modelMat)
{
	this->modelMatrix = modelMat;
}
void Model::setRotationMatrix(glm::mat4 rotationMat)
{
	this->rotationMatrix = rotationMat;
}
//Multiplies the model matrix with the rotation matrix (used for constant rotation)
void Model::rotate()
{
	this->modelMatrix *= rotationMatrix;
}
//Reads a .obj file and creates a Model object from the data
void Model::read(std::string filename)
{
	//Removes any old properties
	//faces = std::vector<std::vector<Vertex>>();
	//Temporary containers
	std::ifstream file(filename);
	std::string str = "";
	//Vector of all vertex positions
	std::vector<glm::vec3> vertexPos = std::vector<glm::vec3>();
	vertexPos.push_back(glm::vec3(0, 0, 0));
	//All vertex normals
	std::vector<glm::vec3> vertexNormals = std::vector<glm::vec3>();
	vertexNormals.push_back(glm::vec3(0, 0, 0));
	//All texture coordinates
	std::vector<glm::vec2> vertexTex = std::vector<glm::vec2>();
	vertexTex.push_back(glm::vec3(0, 0, 0));
	//Current material file's materials
	std::vector<Material> materials = std::vector<Material>();
	Material currentMaterial = Material();
	Mesh mesh;
	std::vector<Vertex> allFace = std::vector<Vertex>();

	//Gets a single line of the file at a time
	while (std::getline(file, str))
	{
		std::stringstream line;
		double data;
		//Takes the first word of the line and compares it to variable names
		line << str;
		line >> str;
		if (str == "v")
		{
			//A vertex position
			glm::vec3 aVertexPos;
			if (modelDebug)std::cout << "Vertex (v): ";
			//X
			line >> data;
			aVertexPos.x = data;
			if (modelDebug)std::cout << data << " ";
			//Y
			line >> data;
			aVertexPos.y = data;
			if (modelDebug)std::cout << data << " ";
			//Z
			line >> data;
			aVertexPos.z = data;
			if (modelDebug)std::cout << data << " ";

			vertexPos.push_back(aVertexPos);
			if (modelDebug)std::cout << std::endl;
		}
		else if (str == "vt")
		{
			//A texture position
			glm::vec2 aVertexTex;
			if (modelDebug)std::cout << "Texture Position (vt): ";
			//U
			line >> data;
			aVertexTex.x = data;
			if (modelDebug)std::cout << data << " ";
			//V
			line >> data;
			aVertexTex.y = data;
			if (modelDebug)std::cout << data << " ";

			vertexTex.push_back(aVertexTex);
			if (modelDebug)std::cout << std::endl;
		}
		else if (str == "vn")
		{
			//A normal
			glm::vec3 normal;
			if (modelDebug)std::cout << "Normal (vn): ";
			//X
			line >> data;
			normal.x = data;
			if (modelDebug)std::cout << data << " ";
			//Y
			line >> data;
			normal.y = data;
			if (modelDebug)std::cout << data << " ";
			//Z
			line >> data;
			normal.z = data;
			if (modelDebug)std::cout << data << " ";

			vertexNormals.push_back(normal);
			if (modelDebug)std::cout << std::endl;
		}
		else if (str == "f")
		{
			//Faces
			if (modelDebug)std::cout << "Face (f): ";
			std::vector<Vertex> aFace = std::vector<Vertex>();
			//Split the rest of the line into seperate words
			while (line >> str)
			{
				std::stringstream strIndices;
				strIndices << str;
				if (modelDebug)std::cout << str << " ";
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
					aVertex.texPos = vertexTex.at(i);
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
				mesh.material = currentMaterial;
				//Adds the vertex to this face
				aFace.push_back(aVertex);
				allFace.push_back(aVertex);
			}
			//Adds the face to the model
			//std::cout << "NU PUSHAR JAG TILL MESH" << std::endl;
			//mesh.vertices = aFace;
			//this->meshes.push_back(mesh);
			if (modelDebug)std::cout << std::endl;
		}
		else if (str == "g")
		{
			//Groups
			if (modelDebug)std::cout << "Group name (g): ";
			while (line >> str)
			{
				if (modelDebug)std::cout << str << " ";
			}
			if (modelDebug)std::cout << std::endl;
		}
		else if (str == "s")
		{
			//Smoothing groups
			if (modelDebug)std::cout << "Smoothing group (s): ";
			while (line >> str)
			{
				if (modelDebug)std::cout << str << " ";
			}
			if (modelDebug)std::cout << std::endl;
		}
		else if (str == "o")
		{
			if (!allFace.empty())
			{
				mesh.vertices = allFace;
				mesh.material = currentMaterial;
				meshes.push_back(mesh);
				allFace = std::vector<Vertex>();
			}
		}
		else if (str == "mtllib")
		{
			//Material library
			line >> str;
			if (modelDebug)std::cout << std::endl << "Material Library (mtllib): " << str << std::endl;
			str = filename.substr(0, filename.find_last_of("\\/") + 1) + str;
			std::ifstream mtlFile(str);
			while (std::getline(mtlFile, str))
			{
				//Takes the first word of the line and compares it to variable names
				std::stringstream mtlWord;
				mtlWord << str;
				mtlWord >> str;
				if (str == "newmtl")
				{
					//Check if the material already exists to avoid duplicates
					mtlWord >> str;
					if (Material::findMaterial(str, materials) == -1)
					{
						//Create new material and enter material name
						Material newMaterial;
						newMaterial.name = str;
						if (matDebug)std::cout << "Material name: " << str << std::endl;
						materials.push_back(newMaterial);
					}
				}
				else if (str == "Ka")
				{
					if (matDebug)std::cout << "Ambient colour: ";
					float data = 0.0;
					//R
					mtlWord >> data;
					if (matDebug)std::cout << data << " ";
					materials.at(materials.size() - 1).ambientColour.x = data;
					//G
					mtlWord >> data;
					if (matDebug)std::cout << data << " ";
					materials.at(materials.size() - 1).ambientColour.y = data;
					//B
					mtlWord >> data;
					if (matDebug)std::cout << data << " ";
					materials.at(materials.size() - 1).ambientColour.z = data;
					if (matDebug)std::cout << std::endl;
				}
				else if (str == "Kd")
				{
					if (matDebug)std::cout << "Diffuse colour: ";
					float data = 0.0;
					//R
					mtlWord >> data;
					if (matDebug)std::cout << data << " ";
					materials.at(materials.size() - 1).diffuseColour.x = data;
					//G
					mtlWord >> data;
					if (matDebug)std::cout << data << " ";
					materials.at(materials.size() - 1).diffuseColour.y = data;
					//B
					mtlWord >> data;
					if (matDebug)std::cout << data << " ";
					materials.at(materials.size() - 1).diffuseColour.z = data;
					if (matDebug)std::cout << std::endl;
				}
				else if (str == "Ks")
				{
					if (matDebug)std::cout << "Specular colour: ";
					float data = 0.0;
					//R
					mtlWord >> data;
					if (matDebug)std::cout << data << " ";
					materials.at(materials.size() - 1).specularColour.x = data;
					//G
					mtlWord >> data;
					if (matDebug)std::cout << data << " ";
					materials.at(materials.size() - 1).specularColour.y = data;
					//B
					mtlWord >> data;
					if (matDebug)std::cout << data << " ";
					materials.at(materials.size() - 1).specularColour.z = data;
					if (matDebug)std::cout << std::endl;
				}
				else if (str == "Tr")
				{
					//Transpareny
					float data = 0.0;
					mtlWord >> data;
					if (matDebug)std::cout << "Transparency: " << data << std::endl;
					materials.at(materials.size() - 1).transparency = data;
				}
				else if (str == "illum")
				{
					//Illumination mode
					int data = 0;
					mtlWord >> data;
					if (matDebug)std::cout << "Illumination mode: " << data << std::endl;
					materials.at(materials.size() - 1).illuminationMode = data;
				}
				else if (str == "map_Ka")
				{
					//Name of the file containing the ambient texture map
					mtlWord >> str;
					str = filename.substr(0, filename.find_last_of("\\/") + 1) + str;
					if (matDebug)std::cout << "Ambient texture map: " << str << std::endl;
					materials.at(materials.size() - 1).textureMapAmbientFile = str;
				}
				else if (str == "map_Kd")
				{
					//Name of the file containing the diffuse texture map
					mtlWord >> str;
					str = filename.substr(0, filename.find_last_of("\\/") + 1) + str;
					if (matDebug)std::cout << "Diffuse texture map: " << str << std::endl;
					materials.at(materials.size() - 1).textureMapDiffuseFile = str;
				}
				else if (str == "map_Ks")
				{
					//Name of the file containing the specular texture map
					mtlWord >> str;
					str = filename.substr(0, filename.find_last_of("\\/") + 1) + str;
					if (matDebug)std::cout << "Specular texture map: " << str << std::endl;
					materials.at(materials.size() - 1).textureMapSpecularFile = str;
				}
			}
			if (matDebug)std::cout << std::endl;
		}
		else if (str == "usemtl")
		{
			//Material name
			if (modelDebug)std::cout << "Material name (usemtl): ";
			while (line >> str)
			{
				//Set the current material so it can be assigned to faces
				int index = Material::findMaterial(str, materials);
				if (index != -1)
				{
					currentMaterial = materials.at(index);
				}
				if (modelDebug)std::cout << str << " ";
			}
			if (modelDebug)std::cout << std::endl;
		}
	}

	if (!allFace.empty())
	{
		mesh.vertices = allFace;
		mesh.material = currentMaterial;
		meshes.push_back(mesh);
		allFace = std::vector<Vertex>();
	}
	//mesh.material = currentMaterial;
	//mesh.vertices = allFace;
	//meshes.push_back(mesh);
	/*
	while (std::getline(file, str))
	{
		std::stringstream line;
		double data;
		//Takes the first word of the line and compares it to variable names
		line << str;
		line >> str;
		if (str == "v")
		{
			//A vertex position
			glm::vec3 aVertexPos;
			if(modelDebug)std::cout << "Vertex (v): ";
			//X
			line >> data;
			aVertexPos.x = data;
			if (modelDebug)std::cout << data << " ";
			//Y
			line >> data;
			aVertexPos.y = data;
			if (modelDebug)std::cout << data << " ";
			//Z
			line >> data;
			aVertexPos.z = data;
			if (modelDebug)std::cout << data << " ";

			vertexPos.push_back(aVertexPos);
			if (modelDebug)std::cout << std::endl;
		}
		else if (str == "vt")
		{
			//A texture position
			glm::vec2 aVertexTex;
			if (modelDebug)std::cout << "Texture Position (vt): ";
			//U
			line >> data;
			aVertexTex.x = data;
			if (modelDebug)std::cout << data << " ";
			//V
			line >> data;
			aVertexTex.y = data;
			if (modelDebug)std::cout << data << " ";

			vertexTex.push_back(aVertexTex);
			if (modelDebug)std::cout << std::endl;
		}
		else if (str == "vn")
		{
			//A normal
			glm::vec3 normal;
			if (modelDebug)std::cout << "Normal (vn): ";
			//X
			line >> data;
			normal.x = data;
			if (modelDebug)std::cout << data << " ";
			//Y
			line >> data;
			normal.y = data;
			if (modelDebug)std::cout << data << " ";
			//Z
			line >> data;
			normal.z = data;
			if (modelDebug)std::cout << data << " ";

			vertexNormals.push_back(normal);
			if (modelDebug)std::cout << std::endl;
		}
		else if (str == "f")
		{
			//Faces
			if (modelDebug)std::cout << "Face (f): ";
			std::vector<Vertex> aFace = std::vector<Vertex>();
			//Split the rest of the line into seperate words
			while (line >> str)
			{
				std::stringstream strIndices;
				strIndices << str;
				if (modelDebug)std::cout << str << " ";
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
					aVertex.texPos = vertexTex.at(i);
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
				aVertex.material = currentMaterial;
				aVertex.colour = glm::vec4(aVertex.material.diffuseColour, 1.0);
				//Adds the vertex to this face
				aFace.push_back(aVertex);
			}
			//Adds the face to the model
			this->faces.push_back(aFace);
			if (modelDebug)std::cout << std::endl;
		}
		else if (str == "g")
		{
			//Groups
			if (modelDebug)std::cout << "Group name (g): ";
			while (line >> str)
			{
				if (modelDebug)std::cout << str << " ";
			}
			if (modelDebug)std::cout << std::endl;
		}
		else if (str == "s")
		{
			//Smoothing groups
			if (modelDebug)std::cout << "Smoothing group (s): ";
			while (line >> str)
			{
				if (modelDebug)std::cout << str << " ";
			}
			if (modelDebug)std::cout << std::endl;
		}
		else if (str == "mtllib")
		{
			//Material library
			line >> str;
			if (modelDebug)std::cout << std::endl << "Material Library (mtllib): " << str << std::endl;
			str = filename.substr(0, filename.find_last_of("\\/") + 1) + str;
			std::ifstream mtlFile(str);
			while (std::getline(mtlFile, str))
			{
				//Takes the first word of the line and compares it to variable names
				std::stringstream mtlWord;
				mtlWord << str;
				mtlWord >> str;
				if (str == "newmtl")
				{
					//Check if the material already exists to avoid duplicates
					mtlWord >> str;
					if (Material::findMaterial(str, materials) == -1)
					{
						//Create new material and enter material name
						Material newMaterial;
						newMaterial.name = str;
						if (matDebug)std::cout << "Material name: " << str << std::endl;
						materials.push_back(newMaterial);
					}
				}
				else if (str == "Ka")
				{
					if (matDebug)std::cout << "Ambient colour: ";
					float data = 0.0;
					//R
					mtlWord >> data;
					if (matDebug)std::cout << data << " ";
					materials.at(materials.size() - 1).ambientColour.x = data;
					//G
					mtlWord >> data;
					if (matDebug)std::cout << data << " ";
					materials.at(materials.size() - 1).ambientColour.y = data;
					//B
					mtlWord >> data;
					if (matDebug)std::cout << data << " ";
					materials.at(materials.size() - 1).ambientColour.z = data;
					if (matDebug)std::cout << std::endl;
				}
				else if (str == "Kd")
				{
					if (matDebug)std::cout << "Diffuse colour: ";
					float data = 0.0;
					//R
					mtlWord >> data;
					if (matDebug)std::cout << data << " ";
					materials.at(materials.size() - 1).diffuseColour.x = data;
					//G
					mtlWord >> data;
					if (matDebug)std::cout << data << " ";
					materials.at(materials.size() - 1).diffuseColour.y = data;
					//B
					mtlWord >> data;
					if (matDebug)std::cout << data << " ";
					materials.at(materials.size() - 1).diffuseColour.z = data;
					if (matDebug)std::cout << std::endl;
				}
				else if (str == "Ks")
				{
					if (matDebug)std::cout << "Specular colour: ";
					float data = 0.0;
					//R
					mtlWord >> data;
					if (matDebug)std::cout << data << " ";
					materials.at(materials.size() - 1).specularColour.x = data;
					//G
					mtlWord >> data;
					if (matDebug)std::cout << data << " ";
					materials.at(materials.size() - 1).specularColour.y = data;
					//B
					mtlWord >> data;
					if (matDebug)std::cout << data << " ";
					materials.at(materials.size() - 1).specularColour.z = data;
					if (matDebug)std::cout << std::endl;
				}
				else if (str == "Tr")
				{
					//Transpareny
					float data = 0.0;
					mtlWord >> data;
					if (matDebug)std::cout << "Transparency: " << data << std::endl;
					materials.at(materials.size() - 1).transparency = data;
				}
				else if (str == "illum")
				{
					//Illumination mode
					int data = 0;
					mtlWord >> data;
					if (matDebug)std::cout << "Illumination mode: " << data << std::endl;
					materials.at(materials.size() - 1).illuminationMode = data;
				}
				else if (str == "map_Ka")
				{
					//Name of the file containing the ambient texture map
					mtlWord >> str;
					if (matDebug)std::cout << "Ambient texture map: " << str << std::endl;
					materials.at(materials.size() - 1).textureMapAmbientFile = str;
				}
				else if (str == "map_Kd")
				{
					//Name of the file containing the diffuse texture map
					mtlWord >> str;
					if (matDebug)std::cout << "Diffuse texture map: " << str << std::endl;
					materials.at(materials.size() - 1).textureMapDiffuseFile = str;
				}
				else if (str == "map_Ks")
				{
					//Name of the file containing the specular texture map
					mtlWord >> str;
					if (matDebug)std::cout << "Specular texture map: " << str << std::endl;
					materials.at(materials.size() - 1).textureMapSpecularFile = str;
				}
			}
			if (matDebug)std::cout << std::endl;
		}
		else if (str == "usemtl")
		{
			//Material name
			if (modelDebug)std::cout << "Material name (usemtl): ";
			while (line >> str)
			{
				//Set the current material so it can be assigned to faces
				int index = Material::findMaterial(str, materials);
				if (index != -1)
				{
					currentMaterial = materials.at(index);
				}
				if (modelDebug)std::cout << str << " ";
			}
			if (modelDebug)std::cout << std::endl;
		}
	}
	*/
}
//Draws the model
void Model::draw(Shader shader)
{
	//Draw vertices
	glBindVertexArray(this->VAO);
	for (int i = 0; i < this->meshes.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, meshes.at(i).material.diffuseTexture);
		glUniform1i(glGetUniformLocation(shader.program, "diffuseTexture"), 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, meshes.at(i).material.diffuseTexture);
		glUniform1i(glGetUniformLocation(shader.program, "specularTexture"), 1);
		
		glDrawArrays(GL_TRIANGLES, 0, this->meshes[i].vertices.size()*3);
	}

	glBindVertexArray(0);
}

void Model::setupMesh()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	std::vector<Vertex> vertices = std::vector<Vertex>();
	for (int i = 0; i < meshes.size(); i++)
	{
		//Iterate through vertices in the face
		for (int j = 0; j < meshes[i].vertices.size(); j++)
		{
			vertices.push_back(meshes.at(i).vertices.at(j));
		}
		std::cout << "Name = " << meshes.at(i).material.name << std::endl;
		std::cout << "Diffuse = " << meshes.at(i).material.textureMapDiffuseFile << std::endl;
		std::cout << "Specular = " << meshes.at(i).material.textureMapSpecularFile << std::endl;
		loadTextures(i);
	}

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices.front(), GL_STATIC_DRAW);
	//Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(0));
	//Texture Coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(sizeof(float) * 3));
	//Normal
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(sizeof(float) * 5));

	glBindVertexArray(0);
}

void Model::loadTextures(int meshNr)
{
	glGenTextures(1, &meshes.at(meshNr).material.diffuseTexture);
	glBindTexture(GL_TEXTURE_2D, meshes.at(meshNr).material.diffuseTexture); // All upcoming GL_TEXTURE_2D operations now have effect on this texture object
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load image, create texture and generate mipmaps
	int width, height;
	unsigned char* image = SOIL_load_image(meshes.at(meshNr).material.textureMapDiffuseFile.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

	glGenTextures(1, &meshes.at(meshNr).material.specularTexture);
	glBindTexture(GL_TEXTURE_2D, meshes.at(meshNr).material.specularTexture); // All upcoming GL_TEXTURE_2D operations now have effect on this texture object
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load image, create texture and generate mipmaps
	image = SOIL_load_image(meshes.at(meshNr).material.textureMapSpecularFile.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
}
//Constructors
Model::Model(std::string filename)
{
	//Initializes the model without a rotation or model matrix
	this->modelMatrix = glm::mat4(1.0);
	this->rotationMatrix = glm::mat4(1.0);
	read(filename);
}
Model::Model(std::string filename, glm::mat4 modelMat)
{
	//Initializes the model without a rotation
	this->modelMatrix = modelMat;
	this->rotationMatrix = glm::mat4(1.0);
	read(filename);
	setupMesh();
}
Model::Model(std::string filename, glm::mat4 modelMat, glm::mat4 rotation)
{
	//Initializes the model
	this->modelMatrix = modelMat;
	this->rotationMatrix = rotation;
	read(filename);
}
Model::Model()
{
	//Initializes the model with no data
	this->modelMatrix = glm::mat4(1.0);
	this->rotationMatrix = glm::mat4(1.0);
	//this->faces = std::vector<std::vector<Vertex>>();
}
//Destructor
Model::~Model()
{

}

