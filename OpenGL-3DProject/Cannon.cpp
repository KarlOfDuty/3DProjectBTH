#include "Cannon.h"
///https://en.wikipedia.org/wiki/Drag_coefficient
///https://en.wikipedia.org/wiki/Dynamic_pressure
///https://en.wikipedia.org/wiki/Drag_equation
const double PI = 3.14159265358979323846;
bool checkDistance = true;
Cannon::Cannon()
{
	this->allCannonBalls = std::vector<CannonBall*>();
	this->gravity = 9.82; // m/s^2
	this->tests = 0;
	this->windDirection = glm::vec3(1,0,0); // Used for direction only, thus should always be normalized
	this->windDirection = glm::normalize(windDirection);
	this->windVelocity = 10; // m/s
	this->airDensity = 1.293; // kg/m^3
	this->dragCoefficientSphere = 0.29;
}
Cannon::~Cannon()
{

}
void Cannon::update(float dt)
{
	for (int i = 0; i < allCannonBalls.size(); i++)
	{
		if (allCannonBalls[i]->ballModel->getModelMatrix()[3][1] < -10)
		{
			delete allCannonBalls[i];
			allCannonBalls.erase(allCannonBalls.begin()+i);
		}
		else
		{
			//Calculates the area the sphere would have if it would be orthagonally projected (as in making it a 2D circle)
			float area = pow(allCannonBalls[i]->radius, 2) * PI;
			//Calculates drag to be used in wind and air resistance
			float drag = dragCoefficientSphere*(airDensity*area / 2);
			//Calculate the force of the wind
			glm::vec3 wind;
			wind.x = drag * pow((windDirection.x * windVelocity), 2);
			wind.y = drag * pow((windDirection.y * windVelocity), 2);
			wind.z = drag * pow((windDirection.z * windVelocity), 2);
			//Update speed for the ball
			allCannonBalls[i]->speedVector = allCannonBalls[i]->speedVector + (allCannonBalls[i]->accelVector * dt);
			//Calculate rotation for magnus force
			glm::vec3 angularVelocityX = glm::vec3(1, 0, 0) * allCannonBalls[i]->rotation.x;
			glm::vec3 angularVelocityY = glm::vec3(0, 1, 0) * allCannonBalls[i]->rotation.y;
			glm::vec3 angularVelocityZ = glm::vec3(0, 0, 1) * allCannonBalls[i]->rotation.z;
			//Gather the magnus forces from the rotations around each axis
			glm::vec3 magnusAroundXAxis = glm::vec3(pow(PI, 2) * pow(allCannonBalls[i]->radius, 3) * airDensity) * (cross(angularVelocityX*dt, allCannonBalls[i]->speedVector*dt));
			glm::vec3 magnusAroundYAxis = glm::vec3(pow(PI, 2) * pow(allCannonBalls[i]->radius, 3) * airDensity) * (cross(angularVelocityY*dt, allCannonBalls[i]->speedVector*dt));
			glm::vec3 magnusAroundZAxis = glm::vec3(pow(PI, 2) * pow(allCannonBalls[i]->radius, 3) * airDensity) * (cross(angularVelocityZ*dt, allCannonBalls[i]->speedVector*dt));
			//Update the total speed from all directions
			allCannonBalls[i]->velocity = sqrt(pow(allCannonBalls[i]->speedVector.z, 2) + pow(allCannonBalls[i]->speedVector.y, 2) + pow(allCannonBalls[i]->speedVector.z, 2));
			//Update the angle
			allCannonBalls[i]->direction = allCannonBalls[i]->speedVector;
			allCannonBalls[i]->direction = glm::normalize(allCannonBalls[i]->direction);
			//Update the acceleration for the ball
			//Wind and drag added to all axies accelerations
			allCannonBalls[i]->accelVector = (wind + magnusAroundXAxis + magnusAroundYAxis + magnusAroundZAxis + (-drag*pow(allCannonBalls[i]->velocity, 2)*allCannonBalls[i]->direction)) / allCannonBalls[i]->mass;
			//Gravitation only on the Y-axis
			allCannonBalls[i]->accelVector.y = -gravity;
			//Translate to new position
			allCannonBalls[i]->ballModel->setModelMatrix(
				glm::translate(allCannonBalls[i]->ballModel->getModelMatrix(), allCannonBalls[i]->speedVector*dt)
			);
		}
	}
}
void Cannon::draw(Shader shader)
{
	for (int i = 0; i < allCannonBalls.size(); i++)
	{
		glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, &allCannonBalls[i]->ballModel->getModelMatrix()[0][0]);
		allCannonBalls[i]->ballModel->draw(shader);
	}
}
void Cannon::shoot(glm::vec3 originPos, Model ball)
{
	originPos = glm::vec3(0, 0, 0);
	glm::mat4 modelMat = glm::mat4(
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		originPos.x, originPos.y+3, originPos.z, 1.0
	);
	CannonBall* newBall = new CannonBall();
	newBall->ballModel = new Model(ball, modelMat);

	newBall->velocity = 20; // m/s
	
	newBall->rotation = glm::vec3(2,0,0);

	//Angle which the ball starts from
	newBall->direction = glm::vec3(0,1,-2);
	newBall->direction = glm::normalize(newBall->direction);
	newBall->initialVelocity = 10; // m/s

	//A vector with the velocity in each direction
	newBall->speedVector = newBall->direction * newBall->initialVelocity;

	//A vector with the acceleration in each direction
	newBall->accelVector.x = 0;
	newBall->accelVector.y = 1;
	newBall->accelVector.z = -2;
	
	//Density of concrete
	newBall->density = 2000; // kg/m^3
	newBall->radius = 0.5; // m
	float volume = (4 / 3)*PI*pow(newBall->radius, 3); // m^3
	newBall->mass = volume * newBall->density; // kg
	
	newBall->loading = 0;
	newBall->time = 0.0f;
	allCannonBalls.push_back(newBall);
}