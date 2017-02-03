#ifndef OBJHANDLER_H
#define OBJHANDLER_H
#include <vector>
#include "Model.h"
#include<fstream>
#include<sstream>
static std::vector<std::string> modelFiles = {"test.obj","sphere.obj"};

class OBJHandler
{
public:
	static void read(Model &model);
	static void write(Model &model);
	static void readAll(std::vector<int> &vectorToFill);
	static void writeAll(std::vector<int> &vectorToWrite);
	virtual void OBJhandler() = 0;
};

#endif