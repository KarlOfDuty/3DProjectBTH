#include "Cannon.h"
const double PI = 3.14159265358979323846;
Cannon::Cannon()
{
	this->allCannonBalls = std::vector<CannonBall>();
	this->gravity = -9.82;
}
Cannon::~Cannon()
{

}
void Cannon::update(float dt)
{
	for (int i = 0; i < allCannonBalls.size(); i++)
	{
		//std::cout << allCannonBalls[i].speedVector.y << std::endl;
		allCannonBalls[i].speedVector.z = allCannonBalls[i].speedVector.z - allCannonBalls[i].accelVector.z*dt;
		allCannonBalls[i].speedVector.y = allCannonBalls[i].speedVector.y + allCannonBalls[i].accelVector.y*dt;
		
		allCannonBalls[i].velocity = sqrt(pow(allCannonBalls[i].speedVector.z, 2) + pow(allCannonBalls[i].speedVector.y, 2));

		allCannonBalls[i].angle = atan(allCannonBalls[i].speedVector.y / allCannonBalls[i].speedVector.z);

		allCannonBalls[i].accelVector.z = -0.00158*pow(allCannonBalls[i].velocity, 2)*cos(allCannonBalls[i].angle);
		allCannonBalls[i].accelVector.y = -0.00158*pow(allCannonBalls[i].velocity, 2)*sin(allCannonBalls[i].angle) + gravity;

		allCannonBalls[i].ballModel.setModelMatrix(
			glm::translate(allCannonBalls[i].ballModel.getModelMatrix(), allCannonBalls[i].speedVector*dt)
		);
	}
}
void Cannon::draw(Shader shader)
{
	for (int i = 0; i < allCannonBalls.size(); i++)
	{
		glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, &allCannonBalls[i].ballModel.getModelMatrix()[0][0]);
		allCannonBalls[i].ballModel.draw(shader);
	}
}
void Cannon::shoot(glm::vec3 originPos)
{
	glm::mat4 modelMat = glm::mat4(
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0
	);
	CannonBall newBall;
	newBall.ballModel = Model("models/sphere/sphere.obj", modelMat);
	
	newBall.velocity = 10;
	std::cout << "velocity = " << newBall.velocity << std::endl;
	newBall.angle = 30 * PI / 180;
	std::cout << "angle = " << newBall.angle << std::endl;
	newBall.speedVector.x = 0;
	std::cout << "SPEEDVECTOR X = " << newBall.speedVector.x << std::endl;
	newBall.speedVector.y = 10 * -sin(newBall.angle*180/PI);
	std::cout << "sin : " << -sin(newBall.angle * 180 / PI) << std::endl;
	std::cout << "SPEEDVECTOR Y = " << newBall.speedVector.y << std::endl;
	newBall.speedVector.z = -20 * cos(newBall.angle * 180 / PI);
	std::cout << "SPEEDVECTOR Z = " << newBall.speedVector.z << std::endl;
	newBall.accelVector.x = 0;
	newBall.accelVector.y = gravity;
	newBall.accelVector.z = 0;
	
	newBall.mass = 1;
	
	newBall.radius = 100;
	
	allCannonBalls.push_back(newBall);
}
glm::vec3 Cannon::getMovementVector(int cannonBallNr)
{
	if(cannonBallNr >= 0 && cannonBallNr <= this->allCannonBalls.size() && !this->allCannonBalls.empty())
	{
		return this->allCannonBalls[cannonBallNr].speedVector;
	}
	else
	{
		return glm::vec3(-1, -1, -1);
	}
}