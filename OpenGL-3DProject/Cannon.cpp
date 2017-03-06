#include "Cannon.h"
const double PI = 3.14159265358979323846;
Cannon::Cannon()
{
	this->allCannonBalls = std::vector<CannonBall>();
	this->gravity = 9.82;
}
Cannon::~Cannon()
{

}
void Cannon::update(float dt)
{
	for (int i = 0; i < allCannonBalls.size(); i++)
	{
		float test = -0.29*(1.293*PI*pow(allCannonBalls[i].radius, 2) / 2);
		float test2 = test / allCannonBalls[i].mass;

		//Update speed for the ball
		allCannonBalls[i].speedVector.z = allCannonBalls[i].speedVector.z - allCannonBalls[i].accelVector.z*dt;
		allCannonBalls[i].speedVector.y = allCannonBalls[i].speedVector.y + allCannonBalls[i].accelVector.y*dt;
		//Update the total speed from both directions
		allCannonBalls[i].velocity = sqrt(pow(allCannonBalls[i].speedVector.z, 2) + pow(allCannonBalls[i].speedVector.y, 2));
		//Update the angle
		allCannonBalls[i].angle = atan(allCannonBalls[i].speedVector.y / allCannonBalls[i].speedVector.z);
		//Update the acceleration for the ball
		allCannonBalls[i].accelVector.z = -test2*pow(allCannonBalls[i].velocity, 2)*cos(allCannonBalls[i].angle);
		allCannonBalls[i].accelVector.y = -test2*pow(allCannonBalls[i].velocity, 2)*sin(allCannonBalls[i].angle) - gravity;
		//std::cout << allCannonBalls[i].accelVector.y << std::endl;
		//allCannonBalls[i].accelVector.z = -0.00158*pow(allCannonBalls[i].velocity, 2)*cos(allCannonBalls[i].angle);
		//allCannonBalls[i].accelVector.y = -0.00158*pow(allCannonBalls[i].velocity, 2)*sin(allCannonBalls[i].angle) - gravity;
		//Translate to new position
		allCannonBalls[i].ballModel.setModelMatrix(
			glm::translate(allCannonBalls[i].ballModel.getModelMatrix(), allCannonBalls[i].speedVector*dt)
		);
		//allCannonBalls[i].ballModel.rotate();
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
		0.0, 0.01, 0.0, 1.0
	);
	CannonBall newBall;
	newBall.ballModel = Model("models/sphere/sphere.obj", modelMat);

	newBall.velocity = 10;
	
	newBall.angle = 30 * PI / 180;

	newBall.speedVector.y = 10 * -sin(newBall.angle*180/PI);
	newBall.speedVector.z = -20 * cos(newBall.angle * 180 / PI);
	
	newBall.accelVector.x = 0;
	newBall.accelVector.y = -gravity;
	newBall.accelVector.z = 0;
	
	newBall.density = 7870;
	newBall.radius = 0.5;
	newBall.mass = 4.0f/3.0f * PI * pow(newBall.radius,3)*newBall.density;

	newBall.spin = 60;
	newBall.omega = newBall.spin/9.5493;
	
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