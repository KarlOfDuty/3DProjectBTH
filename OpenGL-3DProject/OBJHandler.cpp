#include "OBJHandler.h"

void OBJHandler::read(Model &model)
{
	std::ifstream file("Read.txt");
	std::string str;
	while (std::getline(file, str))
	{
		std::stringstream ss;
		ss << str;
		ss >> str;
		if (str == "v")
		{
			//Vertices
		}
		else if (str == "vf")
		{
			//Faces
		}
		else if (str == "vn")
		{
			//Normals
		}
	}
}
void OBJHandler::write(Model &model)
{

}
void OBJHandler::readAll(std::vector<int>& vectorToFill)
{

}
void OBJHandler::writeAll(std::vector<int>& vectorToWrite)
{

}
