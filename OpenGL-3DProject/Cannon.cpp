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
	if (tests < 23)
	for (int i = 0; i < allCannonBalls.size(); i++)
	{
		if(allCannonBalls[i].loading > 10)
		{
			float test = -0.29*(1.293*PI*pow(allCannonBalls[i].radius, 2) / 2);
			float test2 = test / allCannonBalls[i].mass;

			//double C = pow(PI,2) * 1.293 * pow(allCannonBalls[i].radius, 3) * allCannonBalls[i].omega;
			//allCannonBalls[i].time += dt;
			//double t = allCannonBalls[i].time;
			//allCannonBalls[i].speedVector.z = 1.0f / (1.0f - pow((t-allCannonBalls[i].mass),2)*pow(C,2)) * (allCannonBalls[i].initialVelocity.z + C * allCannonBalls[i].initialVelocity.y * (t/allCannonBalls[i].mass) - C * gravity * pow(t,2)/allCannonBalls[i].mass);
			//std::cout << allCannonBalls[i].speedVector.z << std::endl;
			
			//Update the acceleration for the ball
			allCannonBalls[i].accelVector.z = -test2*pow(allCannonBalls[i].velocity, 2)*cos(allCannonBalls[i].angle);
			allCannonBalls[i].accelVector.y = -test2*pow(allCannonBalls[i].velocity, 2)*sin(allCannonBalls[i].angle) - gravity;

			//Update speed for the ball
			allCannonBalls[i].speedVector.z = allCannonBalls[i].speedVector.z + allCannonBalls[i].accelVector.z*0.1;
			allCannonBalls[i].speedVector.y = allCannonBalls[i].speedVector.y + allCannonBalls[i].accelVector.y*0.1;
			
			//Update the total speed from both directions
			allCannonBalls[i].velocity = sqrt(pow(allCannonBalls[i].speedVector.z, 2) + pow(allCannonBalls[i].speedVector.y, 2));
			
			//Update the angle
			allCannonBalls[i].angle = atan(allCannonBalls[i].speedVector.y / allCannonBalls[i].speedVector.z);
			
			//std::cout << allCannonBalls[i].accelVector.y << std::endl;
			//allCannonBalls[i].accelVector.z = -0.00158*pow(allCannonBalls[i].velocity, 2)*cos(allCannonBalls[i].angle);
			//allCannonBalls[i].accelVector.y = -0.00158*pow(allCannonBalls[i].velocity, 2)*sin(allCannonBalls[i].angle) - gravity;
			
			//std::cout << "vx : " << allCannonBalls[i].speedVector.z <<  std::endl;
			//std::cout << "vy : " << allCannonBalls[i].speedVector.y << std::endl;
			//std::cout << "v : " << allCannonBalls[i].velocity << std::endl;
			//std::cout << "ay : " << allCannonBalls[i].accelVector.y << std::endl;
			std::cout << "ax : " << allCannonBalls[i].accelVector.z << std::endl;
			//Translate to new position
			allCannonBalls[i].ballModel.setModelMatrix(
				glm::translate(allCannonBalls[i].ballModel.getModelMatrix(), allCannonBalls[i].speedVector*dt)
			);
			//allCannonBalls[i].ballModel.rotate();
			tests++;
		}
		else
		{
			allCannonBalls[i].loading++;
		}
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
	originPos = glm::vec3(0, 0, 0);
	glm::mat4 modelMat = glm::mat4(
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		originPos.x, originPos.y, originPos.z, 1.0
	);
	CannonBall newBall;
	newBall.ballModel = Model("models/sphere/sphere.obj", modelMat);

	newBall.velocity = 10;
	
	newBall.angle = 30 * PI / 180;

	newBall.initialVelocity.y = 10;
	newBall.initialVelocity.z = -20;

	newBall.speedVector.y = newBall.initialVelocity.y;
	newBall.speedVector.z = newBall.initialVelocity.z;
	newBall.speedVector.y = newBall.initialVelocity.y * -sin(newBall.angle*180/PI);
	newBall.speedVector.z = newBall.initialVelocity.z * cos(newBall.angle * 180 / PI);

	newBall.accelVector.x = 0;
	newBall.accelVector.y = -gravity;
	newBall.accelVector.z = 0;
	
	newBall.density = 7870;
	newBall.radius = 0.010;
	newBall.mass = 0.0329;//4.0f/3.0f * PI * pow(newBall.radius,3)*newBall.density;

	newBall.spin = 1;
	newBall.omega = newBall.spin/9.5493;
	
	newBall.loading = 0;
	newBall.time = 0.0f;
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