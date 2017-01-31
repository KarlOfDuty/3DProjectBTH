#ifndef OBJHANDLER_H
#define OBJHANDLER_H
#include <vector>
class OBJHandler
{
public:
	void read(std::vector<int> &vectorToFill);
	void write(std::vector<int> &vectorToWrite);
	void OBJhandler();
};

#endif