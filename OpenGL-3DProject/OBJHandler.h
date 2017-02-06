#ifndef OBJHANDLER_H
#define OBJHANDLER_H
#include <vector>
#include "Model.h"
#include<fstream>
#include<sstream>
#include<iostream>

class OBJHandler
{
public:
	static void read(Model &model);
	virtual void OBJhandler() = 0;
};

#endif