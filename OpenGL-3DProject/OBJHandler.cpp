#include "OBJHandler.h"

void OBJHandler::read(Model &model)
{
	std::ifstream file("cubetest.obj");
	std::string str;
	std::vector<glm::vec3> vertexPos = std::vector<glm::vec3>();
	std::vector<glm::vec3> vertexNormals = std::vector<glm::vec3>();
	std::vector<glm::vec2> texturePos = std::vector<glm::vec2>();
	while (std::getline(file, str))
	{
		std::stringstream line;
		double data;
		line << str;
		line >> str;
		if (str == "v")
		{
			//A vertex position
			glm::vec3 aVertexPos;
			std::cout << "Vertex (v): ";
			//X
			line >> data;
			aVertexPos.x = data;
			std::cout << data << " ";
			//Y
			line >> data;
			aVertexPos.y = data;
			std::cout << data << " ";
			//Z
			line >> data;
			aVertexPos.z = data;
			std::cout << data << " ";

			vertexPos.push_back(aVertexPos);
			std::cout << std::endl;
		}
		else if (str == "f")
		{
			std::cout << "Face (f): ";
			
			while (line >> str)
			{
				std::cout << str << " ";
				std::stringstream word;
				word << str;
				std::getline(word, str, '/');
				//word.
			}
			std::cout << std::endl;
		}
		else if (str == "vn")
		{
			//A normal
			glm::vec3 normal;
			std::cout << "Normal (vn): ";
			//X
			line >> data;
			normal.x = data;
			std::cout << data << " ";
			//Y
			line >> data;
			normal.y = data;
			std::cout << data << " ";
			//Z
			line >> data;
			normal.z = data;
			std::cout << data << " ";

			vertexNormals.push_back(normal);
			std::cout << std::endl;
		}
		else if (str == "vt")
		{
			//A texture position
			glm::vec2 aTexturePos;
			std::cout << "Texture Position (vt): ";
			//U
			line >> data;
			aTexturePos.x = data;
			std::cout << data << " ";
			//V
			line >> data;
			aTexturePos.y = data;
			std::cout << data << " ";

			texturePos.push_back(aTexturePos);
			std::cout << std::endl;
		}
		else if (str == "g")
		{
			//Groups
			std::cout << "Group name (g): ";
			while (line >> str)
			{
				std::cout << str << " ";
			}
			std::cout << std::endl;
		}
		else if (str == "s")
		{
			//Groups
			std::cout << "Smoothing group (s): ";
			while (line >> str)
			{
				std::cout << str << " ";
			}
			std::cout << std::endl;
		}
		else if (str == "mtllib")
		{
			//Material library
			std::cout << "Material Library (mtllib): ";
			while (line >> str)
			{
				std::cout << str << " ";
			}
			std::cout << std::endl;
		}
		else if (str == "usemtl")
		{
			//Material name
			std::cout << "Material name (usemtl): ";
			while (line >> str)
			{
				std::cout << str << " ";
			}
			std::cout << std::endl;
		}
	}
}