#include "Cannon.h"
///https://en.wikipedia.org/wiki/Drag_coefficient
///https://en.wikipedia.org/wiki/Dynamic_pressure
///https://en.wikipedia.org/wiki/Drag_equation
const double PI = 3.14159265358979323846;
bool checkDistance = true;
Cannon::Cannon()
{
	this->allCannonBalls = std::vector<CannonBall>();
	this->gravity = 9.82; // m/s^2
	this->tests = 0;
	this->windDirection = glm::vec3(1,0,0); // Used for direction only, thus should always be normalized
	this->windDirection = glm::normalize(windDirection);
	this->windVelocity = 0; // m/s
	this->airDensity = 1.293; // kg/m^3
	this->dragCoefficientSphere = 0.29;
}
Cannon::~Cannon()
{

}
void Cannon::update(float dt)
{

	//if (tests < 100)
	//{
		for (int i = 0; i < allCannonBalls.size(); i++)
		{
			if (allCannonBalls[i].loading > 10)
			{
				//Calculates the area the sphere would have if it would be orthagonally projected (as in making it a 2D circle)
				double area = pow(allCannonBalls[i].radius, 2) * PI;
				//Calculates drag to be used in wind and air resistance
				float drag = -dragCoefficientSphere*(airDensity*area / 2);
				double totalDrag = drag / allCannonBalls[i].mass;
				//Calculate the force of the wind
				glm::vec3 wind;
				wind.x = -drag * pow((windDirection.x * windVelocity), 2);
				wind.y = -drag * pow((windDirection.y * windVelocity), 2);
				wind.z = -drag * pow((windDirection.z * windVelocity), 2);

				//Update speed for the ball
				allCannonBalls[i].speedVector.x = allCannonBalls[i].speedVector.x + (allCannonBalls[i].accelVector.x) * dt;
				allCannonBalls[i].speedVector.z = allCannonBalls[i].speedVector.z + (allCannonBalls[i].accelVector.z) * dt;
				allCannonBalls[i].speedVector.y = allCannonBalls[i].speedVector.y + (allCannonBalls[i].accelVector.y) * dt;

				//Update the total speed from both directions
				allCannonBalls[i].velocity = sqrt(pow(allCannonBalls[i].speedVector.z, 2) + pow(allCannonBalls[i].speedVector.y, 2) + pow(allCannonBalls[i].speedVector.z, 2));

				//Update the angle
				allCannonBalls[i].direction = allCannonBalls[i].speedVector;
				allCannonBalls[i].direction = glm::normalize(allCannonBalls[i].direction);

				//Update the acceleration for the ball
				//allCannonBalls[i].accelVector.x = (wind.x / allCannonBalls[i].mass) + (totalDrag*pow(allCannonBalls[i].velocity, 2)*allCannonBalls[i].direction.x);
				//allCannonBalls[i].accelVector.y = (wind.y / allCannonBalls[i].mass) + (totalDrag*pow(allCannonBalls[i].velocity, 2)*allCannonBalls[i].direction.y) - gravity;
				//allCannonBalls[i].accelVector.z = (wind.z / allCannonBalls[i].mass) + (totalDrag*pow(allCannonBalls[i].velocity, 2)*allCannonBalls[i].direction.z);

				//Wind and drag added to all axies accelerations
				allCannonBalls[i].accelVector = (wind / (float)allCannonBalls[i].mass) + ((float)totalDrag*(float)pow(allCannonBalls[i].velocity, 2)*allCannonBalls[i].direction);
				//Gravitation only on the Y-axis
				allCannonBalls[i].accelVector.y = -gravity;

				//Translate to new position
				allCannonBalls[i].ballModel.setModelMatrix(
					glm::translate(allCannonBalls[i].ballModel.getModelMatrix(), allCannonBalls[i].speedVector*dt)
				);
				//allCannonBalls[i].ballModel.rotate();
				tests++;
				if (tests == 100)
				{
					std::cout << allCannonBalls[i].ballModel.getModelMatrix()[3][2] << std::endl;
				}
			}
			else
			{
				allCannonBalls[i].loading++;
			}
		}
	//}
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
		originPos.x, originPos.y+3, originPos.z, 1.0
	);
	CannonBall newBall;
	newBall.ballModel = Model("models/sphere/sphere.obj", modelMat);

	newBall.velocity = 20; // m/s
	
	//newBall.angle = 30 * PI / 180; // rad for z-axis

	//Angle which the ball starts from
	newBall.direction = glm::vec3(0,1,-2);
	newBall.direction = glm::normalize(newBall.direction);
	newBall.initialVelocity = 10; // m/s

	//A vector with the velocity in each direction
	newBall.speedVector = newBall.direction * newBall.initialVelocity;

	//A vector with the acceleration in each direction
	newBall.accelVector.x = 0;
	newBall.accelVector.y = 1;
	newBall.accelVector.z = -2;
	
	newBall.density = 7870; // kg/m^3
	newBall.radius = 0.1; // m
	double volume = (4 / 3)*PI*pow(newBall.radius, 3); // m^3
	newBall.mass = volume * newBall.density; // kg

	//newBall.spin = 1;
	//newBall.omega = newBall.spin/9.5493;
	
	newBall.loading = 0;
	newBall.time = 0.0f;
	//newBall.ballModel.setRotationMatrix(glm::rotate(glm::mat4(), glm::radians(newBall.omega), glm::vec3(1.0f, 0.0f, 0.0f)));
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